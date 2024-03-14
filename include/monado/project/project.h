#pragma once

#include "monado/core/base.h"

#include <string>
#include <filesystem>

namespace Monado {

    struct ProjectConfig {
        std::string Name = "Untitled";

        std::filesystem::path StartScene;

        std::filesystem::path AssetDirectory;
        std::filesystem::path ScriptModulePath;
    };

    class Project {
    public:
        static const std::filesystem::path &GetProjectDirectory() {
            MND_CORE_ASSERT(s_ActiveProject, "asd");
            return s_ActiveProject->m_ProjectDirectory;
        }

        static std::filesystem::path GetAssetDirectory() {
            MND_CORE_ASSERT(s_ActiveProject, "asd");
            return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
        }

        // TODO(Yan): move to asset manager when we have one
        static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path &path) {
            MND_CORE_ASSERT(s_ActiveProject, "asd");
            return GetAssetDirectory() / path;
        }

        ProjectConfig &GetConfig() { return m_Config; }

        static Ref<Project> GetActive() { return s_ActiveProject; }

        static Ref<Project> New();
        static Ref<Project> Load(const std::filesystem::path &path);
        static bool SaveActive(const std::filesystem::path &path);

    private:
        ProjectConfig m_Config;
        std::filesystem::path m_ProjectDirectory;

        inline static Ref<Project> s_ActiveProject;
    };

} // namespace Monado
