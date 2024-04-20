#include "monado/platform/vulkan/vulkan.h"
#include "monado/platform/vulkan/vulkanContext.h"
#include "monado/platform/vulkan/vulkanDiagnostics.h"

namespace Monado::Utils {

    static const char *StageToString(VkPipelineStageFlagBits stage) {
        switch (stage) {
        case VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT: return "VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT";
        case VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT: return "VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT";
        }
        MND_CORE_ASSERT(false);
        return nullptr;
    }

    void RetrieveDiagnosticCheckpoints() {
        {
            const uint32_t checkpointCount = 4;
            VkCheckpointDataNV data[checkpointCount];
            for (uint32_t i = 0; i < checkpointCount; i++)
                data[i].sType = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;

            uint32_t retrievedCount = checkpointCount;
            vkGetQueueCheckpointDataNV(::Monado::VulkanContext::GetCurrentDevice()->GetQueue(), &retrievedCount, data);
            MND_CORE_ERROR("RetrieveDiagnosticCheckpoints (Graphics Queue):");
            for (uint32_t i = 0; i < retrievedCount; i++) {
                VulkanCheckpointData *checkpoint = (VulkanCheckpointData *)data[i].pCheckpointMarker;
                MND_CORE_ERROR("Checkpoint: {0} (stage: {1})", checkpoint->Data, StageToString(data[i].stage));
            }
        }
        {
            const uint32_t checkpointCount = 4;
            VkCheckpointDataNV data[checkpointCount];
            for (uint32_t i = 0; i < checkpointCount; i++)
                data[i].sType = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;

            uint32_t retrievedCount = checkpointCount;
            vkGetQueueCheckpointDataNV(::Monado::VulkanContext::GetCurrentDevice()->GetComputeQueue(), &retrievedCount,
                                       data);
            MND_CORE_ERROR("RetrieveDiagnosticCheckpoints (Compute Queue):");
            for (uint32_t i = 0; i < retrievedCount; i++) {
                VulkanCheckpointData *checkpoint = (VulkanCheckpointData *)data[i].pCheckpointMarker;
                MND_CORE_ERROR("Checkpoint: {0} (stage: {1})", checkpoint->Data, StageToString(data[i].stage));
            }
        }
        __debugbreak();
    }

} // namespace Monado::Utils
