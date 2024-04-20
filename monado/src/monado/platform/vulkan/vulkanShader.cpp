#include "monado/platform/vulkan/vulkan.h"
#include "monado/platform/vulkan/vulkanDiagnostics.h"
#include "monado/platform/vulkan/vulkanContext.h"
#include "monado/platform/vulkan/vulkanComputePipeline.h"
#include "monado/platform/vulkan/vulkanDevice.h"
#include "monado/platform/vulkan/vulkanIndexBuffer.h"
#include "monado/platform/vulkan/vulkanMaterial.h"
#include "monado/platform/vulkan/vulkanShader.h"
#include "monado/platform/vulkan/vulkanPipeline.h"
#include "monado/platform/vulkan/vulkanFramebuffer.h"

#include "monado/renderer/renderer.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include <filesystem>
#include <fstream>

namespace Monado {

    namespace Utils {

        static const char *GetCacheDirectory() {
            // TODO: make sure the assets directory is valid
            return "assets/cache/shader/vulkan";
        }

        static void CreateCacheDirectoryIfNeeded() {
            std::string cacheDirectory = GetCacheDirectory();
            if (!std::filesystem::exists(cacheDirectory))
                std::filesystem::create_directories(cacheDirectory);
        }

        static ShaderUniformType SPIRTypeToShaderUniformType(spirv_cross::SPIRType type) {
            switch (type.basetype) {
            case spirv_cross::SPIRType::Boolean: return ShaderUniformType::Bool;
            case spirv_cross::SPIRType::Int: return ShaderUniformType::Int;
            case spirv_cross::SPIRType::UInt: return ShaderUniformType::UInt;
            case spirv_cross::SPIRType::Float:
                if (type.vecsize == 1)
                    return ShaderUniformType::Float;
                if (type.vecsize == 2)
                    return ShaderUniformType::Vec2;
                if (type.vecsize == 3)
                    return ShaderUniformType::Vec3;
                if (type.vecsize == 4)
                    return ShaderUniformType::Vec4;

                if (type.columns == 3)
                    return ShaderUniformType::Mat3;
                if (type.columns == 4)
                    return ShaderUniformType::Mat4;
                break;
            }
            MND_CORE_ASSERT(false, "Unknown type!");
            return ShaderUniformType::None;
        }

    } // namespace Utils

    static std::unordered_map<uint32_t, std::unordered_map<uint32_t, VulkanShader::UniformBuffer *>>
        s_UniformBuffers; // set -> binding point -> buffer

    VulkanShader::VulkanShader(const std::string &path, bool forceCompile) : m_AssetPath(path) {
        // TODO: This should be more "general"
        size_t found = path.find_last_of("/\\");
        m_Name = found != std::string::npos ? path.substr(found + 1) : path;
        found = m_Name.find_last_of(".");
        m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;

        Reload(forceCompile);
    }

    VulkanShader::~VulkanShader() {}

    void VulkanShader::ClearUniformBuffers() { s_UniformBuffers.clear(); }

    static std::string ReadShaderFromFile(const std::string &filepath) {
        std::string result;
        std::ifstream in(filepath, std::ios::in );
        if (in) {
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
        } else {
            MND_CORE_ASSERT(false, "Could not load shader!");
        }
        in.close();
        return result;
    }

    void VulkanShader::Reload(bool forceCompile) {
        Ref<VulkanShader> instance = this;
        Renderer::Submit([instance, forceCompile]() mutable {
            // Clear old shader
            instance->m_ShaderDescriptorSets.clear();
            instance->m_Resources.clear();
            instance->m_PushConstantRanges.clear();
            instance->m_PipelineShaderStageCreateInfos.clear();
            instance->m_DescriptorSetLayouts.clear();
            instance->m_ShaderSource.clear();
            instance->m_Buffers.clear();
            instance->m_TypeCounts.clear();

            Utils::CreateCacheDirectoryIfNeeded();

            // Vertex and Fragment for now
            std::string source = ReadShaderFromFile(instance->m_AssetPath);
            instance->m_ShaderSource = instance->PreProcess(source);
            std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> shaderData;
            instance->CompileOrGetVulkanBinary(shaderData, forceCompile);
            instance->LoadAndCreateShaders(shaderData);
            instance->ReflectAllShaderStages(shaderData);
            instance->CreateDescriptors();

            Renderer::OnShaderReloaded(instance->GetHash());
        });
    }

    size_t VulkanShader::GetHash() const { return std::hash<std::string> {}(m_AssetPath); }

    void VulkanShader::LoadAndCreateShaders(
        const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> &shaderData) {
        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();
        m_PipelineShaderStageCreateInfos.clear();
        for (auto [stage, data] : shaderData) {
            MND_CORE_ASSERT(data.size());
            // Create a new shader module that will be used for pipeline creation
            VkShaderModuleCreateInfo moduleCreateInfo {};
            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.codeSize = data.size() * sizeof(uint32_t);
            moduleCreateInfo.pCode = data.data();

            VkShaderModule shaderModule;
            VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule));

            VkPipelineShaderStageCreateInfo &shaderStage = m_PipelineShaderStageCreateInfos.emplace_back();
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = stage;
            shaderStage.module = shaderModule;
            shaderStage.pName = "main";
        }
    }

    void VulkanShader::ReflectAllShaderStages(
        const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> &shaderData) {
        m_Resources.clear();

        for (auto [stage, data] : shaderData) {
            Reflect(stage, data);
        }
    }

    void VulkanShader::Reflect(VkShaderStageFlagBits shaderStage, const std::vector<uint32_t> &shaderData) {
        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

        MND_CORE_TRACE("===========================");
        MND_CORE_TRACE(" Vulkan Shader Reflection");
        MND_CORE_TRACE(" {0}", m_AssetPath);
        MND_CORE_TRACE("===========================");

        spirv_cross::Compiler compiler(shaderData);
        auto resources = compiler.get_shader_resources();

        MND_CORE_TRACE("Uniform Buffers:");
        for (const auto &resource : resources.uniform_buffers) {
            const auto &name = resource.name;
            auto &bufferType = compiler.get_type(resource.base_type_id);
            int memberCount = bufferType.member_types.size();
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint32_t size = compiler.get_declared_struct_size(bufferType);

            ShaderDescriptorSet &shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
            // MND_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) ==
            // shaderDescriptorSet.UniformBuffers.end());
            if (s_UniformBuffers[descriptorSet].find(binding) == s_UniformBuffers[descriptorSet].end()) {
                UniformBuffer *uniformBuffer = new UniformBuffer();
                uniformBuffer->BindingPoint = binding;
                uniformBuffer->Size = size;
                uniformBuffer->Name = name;
                uniformBuffer->ShaderStage = shaderStage;
                s_UniformBuffers.at(descriptorSet)[binding] = uniformBuffer;

                AllocateUniformBuffer(*uniformBuffer);
            } else {
                UniformBuffer *uniformBuffer = s_UniformBuffers.at(descriptorSet).at(binding);
                if (size > uniformBuffer->Size) {
                    MND_CORE_TRACE("Resizing uniform buffer (binding = {0}, set = {1}) to {2} bytes", binding,
                                   descriptorSet, size);
                    uniformBuffer->Size = size;
                    AllocateUniformBuffer(*uniformBuffer);
                }
            }

            shaderDescriptorSet.UniformBuffers[binding] = s_UniformBuffers.at(descriptorSet).at(binding);

            MND_CORE_TRACE("  {0} ({1}, {2})", name, descriptorSet, binding);
            MND_CORE_TRACE("  Member Count: {0}", memberCount);
            MND_CORE_TRACE("  Size: {0}", size);
            MND_CORE_TRACE("-------------------");
        }

        MND_CORE_TRACE("Push Constant Buffers:");
        for (const auto &resource : resources.push_constant_buffers) {
            const auto &bufferName = resource.name;
            auto &bufferType = compiler.get_type(resource.base_type_id);
            auto bufferSize = compiler.get_declared_struct_size(bufferType);
            int memberCount = bufferType.member_types.size();
            uint32_t bufferOffset = 0;
            if (m_PushConstantRanges.size())
                bufferOffset = m_PushConstantRanges.back().Offset + m_PushConstantRanges.back().Size;

            auto &pushConstantRange = m_PushConstantRanges.emplace_back();
            pushConstantRange.ShaderStage = shaderStage;
            pushConstantRange.Size = bufferSize;
            pushConstantRange.Offset = bufferOffset;

            // Skip empty push constant buffers - these are for the renderer only
            if (bufferName.empty() || bufferName == "u_Renderer")
                continue;

            ShaderBuffer &buffer = m_Buffers[bufferName];
            buffer.Name = bufferName;
            buffer.Size = bufferSize - bufferOffset;

            MND_CORE_TRACE("  Name: {0}", bufferName);
            MND_CORE_TRACE("  Member Count: {0}", memberCount);
            MND_CORE_TRACE("  Size: {0}", bufferSize);

            for (int i = 0; i < memberCount; i++) {
                auto type = compiler.get_type(bufferType.member_types[i]);
                const auto &memberName = compiler.get_member_name(bufferType.self, i);
                auto size = compiler.get_declared_struct_member_size(bufferType, i);
                auto offset = compiler.type_struct_member_offset(bufferType, i) - bufferOffset;

                std::string uniformName = bufferName + "." + memberName;
                buffer.Uniforms[uniformName] =
                    ShaderUniform(uniformName, Utils::SPIRTypeToShaderUniformType(type), size, offset);
            }
        }

        MND_CORE_TRACE("Sampled Images:");
        for (const auto &resource : resources.sampled_images) {
            const auto &name = resource.name;
            auto &type = compiler.get_type(resource.base_type_id);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint32_t dimension = type.image.dim;

            ShaderDescriptorSet &shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
            auto &imageSampler = shaderDescriptorSet.ImageSamplers[binding];
            imageSampler.BindingPoint = binding;
            imageSampler.DescriptorSet = descriptorSet;
            imageSampler.Name = name;
            imageSampler.ShaderStage = shaderStage;

            m_Resources[name] = ShaderResourceDeclaration(name, binding, 1);

            MND_CORE_TRACE("  {0} ({1}, {2})", name, descriptorSet, binding);
        }

        MND_CORE_TRACE("Storage Images:");
        for (const auto &resource : resources.storage_images) {
            const auto &name = resource.name;
            auto &type = compiler.get_type(resource.base_type_id);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint32_t dimension = type.image.dim;

            ShaderDescriptorSet &shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
            auto &imageSampler = shaderDescriptorSet.StorageImages[binding];
            imageSampler.BindingPoint = binding;
            imageSampler.DescriptorSet = descriptorSet;
            imageSampler.Name = name;
            imageSampler.ShaderStage = shaderStage;

            m_Resources[name] = ShaderResourceDeclaration(name, binding, 1);

            MND_CORE_TRACE("  {0} ({1}, {2})", name, descriptorSet, binding);
        }

        MND_CORE_TRACE("===========================");
    }

    void VulkanShader::CreateDescriptors() {
        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

        //////////////////////////////////////////////////////////////////////
        // Descriptor Pool
        //////////////////////////////////////////////////////////////////////

        m_TypeCounts.clear();
        for (auto &&[set, shaderDescriptorSet] : m_ShaderDescriptorSets) {
            if (shaderDescriptorSet.UniformBuffers.size()) {
                VkDescriptorPoolSize &typeCount = m_TypeCounts[set].emplace_back();
                typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                typeCount.descriptorCount = shaderDescriptorSet.UniformBuffers.size();
            }
            if (shaderDescriptorSet.ImageSamplers.size()) {
                VkDescriptorPoolSize &typeCount = m_TypeCounts[set].emplace_back();
                typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                typeCount.descriptorCount = shaderDescriptorSet.ImageSamplers.size();
            }
            if (shaderDescriptorSet.StorageImages.size()) {
                VkDescriptorPoolSize &typeCount = m_TypeCounts[set].emplace_back();
                typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                typeCount.descriptorCount = shaderDescriptorSet.StorageImages.size();
            }

#if 0
			// TODO: Move this to the centralized renderer
			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.pNext = nullptr;
			descriptorPoolInfo.poolSizeCount = m_TypeCounts.size();
			descriptorPoolInfo.pPoolSizes = m_TypeCounts.data();
			descriptorPoolInfo.maxSets = 1;

			VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool));
#endif

            //////////////////////////////////////////////////////////////////////
            // Descriptor Set Layout
            //////////////////////////////////////////////////////////////////////

            std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
            for (auto &[binding, uniformBuffer] : shaderDescriptorSet.UniformBuffers) {
                VkDescriptorSetLayoutBinding &layoutBinding = layoutBindings.emplace_back();
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                layoutBinding.descriptorCount = 1;
                layoutBinding.stageFlags = uniformBuffer->ShaderStage;
                layoutBinding.pImmutableSamplers = nullptr;
                layoutBinding.binding = binding;

                VkWriteDescriptorSet &set = shaderDescriptorSet.WriteDescriptorSets[uniformBuffer->Name];
                set = {};
                set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                set.descriptorType = layoutBinding.descriptorType;
                set.descriptorCount = 1;
                set.dstBinding = layoutBinding.binding;

                // if (!uniformBuffer->Memory)
                //	AllocateUniformBuffer(*uniformBuffer);
            }

            for (auto &[binding, imageSampler] : shaderDescriptorSet.ImageSamplers) {
                auto &layoutBinding = layoutBindings.emplace_back();
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                layoutBinding.descriptorCount = 1;
                layoutBinding.stageFlags = imageSampler.ShaderStage;
                layoutBinding.pImmutableSamplers = nullptr;
                layoutBinding.binding = binding;

                MND_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) ==
                                    shaderDescriptorSet.UniformBuffers.end(),
                                "Binding is already present!");

                VkWriteDescriptorSet &set = shaderDescriptorSet.WriteDescriptorSets[imageSampler.Name];
                set = {};
                set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                set.descriptorType = layoutBinding.descriptorType;
                set.descriptorCount = 1;
                set.dstBinding = layoutBinding.binding;
            }

            for (auto &[bindingAndSet, imageSampler] : shaderDescriptorSet.StorageImages) {
                auto &layoutBinding = layoutBindings.emplace_back();
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                layoutBinding.descriptorCount = 1;
                layoutBinding.stageFlags = imageSampler.ShaderStage;
                layoutBinding.pImmutableSamplers = nullptr;

                uint32_t binding = bindingAndSet & 0xffffffff;
                uint32_t descriptorSet = (bindingAndSet >> 32);
                layoutBinding.binding = binding;

                MND_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) ==
                                    shaderDescriptorSet.UniformBuffers.end(),
                                "Binding is already present!");
                MND_CORE_ASSERT(shaderDescriptorSet.ImageSamplers.find(binding) ==
                                    shaderDescriptorSet.ImageSamplers.end(),
                                "Binding is already present!");

                VkWriteDescriptorSet &set = shaderDescriptorSet.WriteDescriptorSets[imageSampler.Name];
                set = {};
                set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                set.descriptorType = layoutBinding.descriptorType;
                set.descriptorCount = 1;
                set.dstBinding = layoutBinding.binding;
            }

            VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
            descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorLayout.pNext = nullptr;
            descriptorLayout.bindingCount = layoutBindings.size();
            descriptorLayout.pBindings = layoutBindings.data();

            MND_CORE_INFO("Creating descriptor set {0} with {1} ubos, {2} samplers and {3} storage images", set,
                          shaderDescriptorSet.UniformBuffers.size(), shaderDescriptorSet.ImageSamplers.size(),
                          shaderDescriptorSet.StorageImages.size());
            VK_CHECK_RESULT(
                vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &m_DescriptorSetLayouts[set]));
        }
    }

    VulkanShader::ShaderMaterialDescriptorSet VulkanShader::CreateDescriptorSets(uint32_t set) {
        ShaderMaterialDescriptorSet result;

        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

        MND_CORE_ASSERT(m_TypeCounts.find(set) != m_TypeCounts.end());

        // TODO: Move this to the centralized renderer
        VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.pNext = nullptr;
        descriptorPoolInfo.poolSizeCount = m_TypeCounts.at(set).size();
        descriptorPoolInfo.pPoolSizes = m_TypeCounts.at(set).data();
        descriptorPoolInfo.maxSets = 1;

        VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &result.Pool));

        // Allocate a new descriptor set from the global descriptor pool
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = result.Pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];

        result.DescriptorSets.emplace_back();
        VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, result.DescriptorSets.data()));
        return result;
    }

    VulkanShader::ShaderMaterialDescriptorSet VulkanShader::CreateDescriptorSets(uint32_t set, uint32_t numberOfSets) {
        ShaderMaterialDescriptorSet result;

        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

        std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> poolSizes;
        for (auto &&[set, shaderDescriptorSet] : m_ShaderDescriptorSets) {
            if (shaderDescriptorSet.UniformBuffers.size()) {
                VkDescriptorPoolSize &typeCount = poolSizes[set].emplace_back();
                typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                typeCount.descriptorCount = shaderDescriptorSet.UniformBuffers.size() * numberOfSets;
            }
            if (shaderDescriptorSet.ImageSamplers.size()) {
                VkDescriptorPoolSize &typeCount = poolSizes[set].emplace_back();
                typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                typeCount.descriptorCount = shaderDescriptorSet.ImageSamplers.size() * numberOfSets;
            }
            if (shaderDescriptorSet.StorageImages.size()) {
                VkDescriptorPoolSize &typeCount = poolSizes[set].emplace_back();
                typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                typeCount.descriptorCount = shaderDescriptorSet.StorageImages.size() * numberOfSets;
            }
        }

        MND_CORE_ASSERT(poolSizes.find(set) != poolSizes.end());

        // TODO: Move this to the centralized renderer
        VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.pNext = nullptr;
        descriptorPoolInfo.poolSizeCount = poolSizes.at(set).size();
        descriptorPoolInfo.pPoolSizes = poolSizes.at(set).data();
        descriptorPoolInfo.maxSets = numberOfSets;

        VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &result.Pool));

        result.DescriptorSets.resize(numberOfSets);

        for (uint32_t i = 0; i < numberOfSets; i++) {
            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = result.Pool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];

            VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &result.DescriptorSets[i]));
        }
        return result;
    }

    const VkWriteDescriptorSet *VulkanShader::GetDescriptorSet(const std::string &name, uint32_t set) const {
        MND_CORE_ASSERT(m_ShaderDescriptorSets.find(set) != m_ShaderDescriptorSets.end());
        if (m_ShaderDescriptorSets.at(set).WriteDescriptorSets.find(name) ==
            m_ShaderDescriptorSets.at(set).WriteDescriptorSets.end()) {
            MND_CORE_WARN("Shader {0} does not contain requested descriptor set {1}", m_Name, name);
            return nullptr;
        }
        return &m_ShaderDescriptorSets.at(set).WriteDescriptorSets.at(name);
    }

    std::vector<VkDescriptorSetLayout> VulkanShader::GetAllDescriptorSetLayouts() {
        std::vector<VkDescriptorSetLayout> result;
        result.reserve(m_DescriptorSetLayouts.size());
        for (auto [set, layout] : m_DescriptorSetLayouts)
            result.emplace_back(layout);

        return result;
    }

    void VulkanShader::AllocateUniformBuffer(UniformBuffer &dst) {
        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

        UniformBuffer &uniformBuffer = dst;

        // Vertex shader uniform buffer block
        VkBufferCreateInfo bufferInfo = {};
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = 0;
        allocInfo.memoryTypeIndex = 0;

        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = uniformBuffer.Size;
        // This buffer will be used as a uniform buffer
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VulkanAllocator allocator("UniformBuffer");

        // Create a new buffer
        VK_CHECK_RESULT(vkCreateBuffer(device, &bufferInfo, nullptr, &uniformBuffer.Buffer));

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, uniformBuffer.Buffer, &memoryRequirements);
        allocInfo.allocationSize = memoryRequirements.size;

        allocator.Allocate(memoryRequirements, &uniformBuffer.Memory,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        VK_CHECK_RESULT(vkBindBufferMemory(device, uniformBuffer.Buffer, uniformBuffer.Memory, 0));

        // Store information in the uniform's descriptor that is used by the descriptor set
        uniformBuffer.Descriptor.buffer = uniformBuffer.Buffer;
        uniformBuffer.Descriptor.offset = 0;
        uniformBuffer.Descriptor.range = uniformBuffer.Size;
    }

    static const char *VkShaderStageCachedFileExtension(VkShaderStageFlagBits stage) {
        switch (stage) {
        case VK_SHADER_STAGE_VERTEX_BIT: return ".cached_vulkan.vert";
        case VK_SHADER_STAGE_FRAGMENT_BIT: return ".cached_vulkan.frag";
        case VK_SHADER_STAGE_COMPUTE_BIT: return ".cached_vulkan.comp";
        }
        MND_CORE_ASSERT(false);
        return "";
    }

    static shaderc_shader_kind VkShaderStageToShaderC(VkShaderStageFlagBits stage) {
        switch (stage) {
        case VK_SHADER_STAGE_VERTEX_BIT: return shaderc_vertex_shader;
        case VK_SHADER_STAGE_FRAGMENT_BIT: return shaderc_fragment_shader;
        case VK_SHADER_STAGE_COMPUTE_BIT: return shaderc_compute_shader;
        }
        MND_CORE_ASSERT(false);
        return (shaderc_shader_kind)0;
    }

    void VulkanShader::CompileOrGetVulkanBinary(
        std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> &outputBinary, bool forceCompile) {
        std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
        for (auto [stage, source] : m_ShaderSource) {
            auto extension = VkShaderStageCachedFileExtension(stage);
            if (!forceCompile) {
                std::filesystem::path p = m_AssetPath;
                auto path = cacheDirectory / (p.filename().string() + extension);
                std::string cachedFilePath = path.string();

                FILE *f = fopen(cachedFilePath.c_str(), "rb");
                if (f) {
                    fseek(f, 0, SEEK_END);
                    uint64_t size = ftell(f);
                    fseek(f, 0, SEEK_SET);
                    outputBinary[stage] = std::vector<uint32_t>(size / sizeof(uint32_t));
                    fread(outputBinary[stage].data(), sizeof(uint32_t), outputBinary[stage].size(), f);
                    fclose(f);
                }
            }

            if (outputBinary[stage].size() == 0) {
                // Do we need to init a compiler for each stage?
                shaderc::Compiler compiler;
                shaderc::CompileOptions options;
                options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

                const bool optimize = false;
                if (optimize)
                    options.SetOptimizationLevel(shaderc_optimization_level_performance);

                // Compile shader
                {
                    auto &shaderSource = m_ShaderSource.at(stage);
                    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
                        shaderSource, VkShaderStageToShaderC(stage), m_AssetPath.c_str(), options);

                    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                        MND_CORE_ERROR(module.GetErrorMessage());
                        MND_CORE_ASSERT(false);
                    }

                    const uint8_t *begin = (const uint8_t *)module.cbegin();
                    const uint8_t *end = (const uint8_t *)module.cend();
                    const ptrdiff_t size = end - begin;

                    outputBinary[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
                }

                // Cache compiled shader
                {
                    std::filesystem::path p = m_AssetPath;
                    auto path = cacheDirectory / (p.filename().string() + extension);
                    std::string cachedFilePath = path.string();

                    FILE *f = fopen(cachedFilePath.c_str(), "wb");
                    fwrite(outputBinary[stage].data(), sizeof(uint32_t), outputBinary[stage].size(), f);
                    fclose(f);
                }
            }
        }
    }

    static VkShaderStageFlagBits ShaderTypeFromString(const std::string &type) {
        if (type == "vertex")
            return VK_SHADER_STAGE_VERTEX_BIT;
        if (type == "fragment" || type == "pixel")
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        if (type == "compute")
            return VK_SHADER_STAGE_COMPUTE_BIT;

        return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }

    std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShader::PreProcess(const std::string &source) {
        std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;

        const char *typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);
            MND_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            MND_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute",
                            "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            auto shaderType = ShaderTypeFromString(type);
            shaderSources[shaderType] =
                source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        }

        return shaderSources;
    }

    void VulkanShader::Bind() {}

    RendererID VulkanShader::GetRendererID() const { return 0; }

    void VulkanShader::SetUniformBuffer(const std::string &name, const void *data, uint32_t size) {}

    void VulkanShader::SetUniform(const std::string &fullname, float value) {}

    void VulkanShader::SetUniform(const std::string &fullname, int value) {}

    void VulkanShader::SetUniform(const std::string &fullname, const glm::vec2 &value) {}

    void VulkanShader::SetUniform(const std::string &fullname, const glm::vec3 &value) {}

    void VulkanShader::SetUniform(const std::string &fullname, const glm::vec4 &value) {}

    void VulkanShader::SetUniform(const std::string &fullname, const glm::mat3 &value) {}

    void VulkanShader::SetUniform(const std::string &fullname, const glm::mat4 &value) {}

    void VulkanShader::SetUniform(const std::string &fullname, uint32_t value) {}

    void VulkanShader::SetUInt(const std::string &name, uint32_t value) {}

    void VulkanShader::SetFloat(const std::string &name, float value) {}

    void VulkanShader::SetInt(const std::string &name, int value) {}

    void VulkanShader::SetFloat2(const std::string &name, const glm::vec2 &value) {}

    void VulkanShader::SetFloat3(const std::string &name, const glm::vec3 &value) {}

    void VulkanShader::SetMat4(const std::string &name, const glm::mat4 &value) {}

    void VulkanShader::SetMat4FromRenderThread(const std::string &name, const glm::mat4 &value, bool bind /*= true*/) {}

    void VulkanShader::SetIntArray(const std::string &name, int *values, uint32_t size) {}

    const std::unordered_map<std::string, ShaderResourceDeclaration> &VulkanShader::GetResources() const {
        return m_Resources;
    }

    void VulkanShader::AddShaderReloadedCallback(const ShaderReloadedCallback &callback) {}

    void *VulkanShader::MapUniformBuffer(uint32_t bindingPoint, uint32_t set) {
        MND_CORE_ASSERT(s_UniformBuffers.find(set) != s_UniformBuffers.end());
        MND_CORE_ASSERT(s_UniformBuffers.at(set).find(bindingPoint) != s_UniformBuffers.at(set).end());
        MND_CORE_ASSERT(s_UniformBuffers.at(set).at(bindingPoint));

        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();
        uint8_t *pData;
        VK_CHECK_RESULT(vkMapMemory(device, s_UniformBuffers.at(set).at(bindingPoint)->Memory, 0,
                                    s_UniformBuffers.at(set).at(bindingPoint)->Size, 0, (void **)&pData));
        return pData;
    }

    void VulkanShader::UnmapUniformBuffer(uint32_t bindingPoint, uint32_t set) {
        MND_CORE_ASSERT(s_UniformBuffers.find(set) != s_UniformBuffers.end());
        MND_CORE_ASSERT(s_UniformBuffers.at(set).find(bindingPoint) != s_UniformBuffers.at(set).end());
        MND_CORE_ASSERT(s_UniformBuffers.at(set).at(bindingPoint));

        VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();
        vkUnmapMemory(device, s_UniformBuffers.at(set).at(bindingPoint)->Memory);
    }

} // namespace Monado
