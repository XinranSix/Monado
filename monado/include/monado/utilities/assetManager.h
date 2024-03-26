#pragma once

#include "monado/utilities/fileSystemWatcher.h"
#include "monado/core/uuid.h"
#include "monado/core/log.h"

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace Monado {

    enum class AssetType { Scene, Mesh, Texture, EnvMap, Audio, Script, Other };

    class AssetTypes {
    public:
        static void Init();
        static size_t GetAssetTypeID(const std::string &extension);
        static AssetType GetAssetTypeFromExtension(const std::string &extension);

    private:
        static std::map<std::string, AssetType> s_Types;
    };

    struct DirectoryInfo {
        std::string DirectoryName;
        std::string Filepath;
        int DirectoryIndex;
        int ParentIndex;
        std::vector<int> ChildrenIndices;
    };

    struct Asset {
        UUID ID;
        std::string FilePath;
        std::string FileName;
        std::string Extension;
        AssetType Type;
        int ParentDirectory;
        void *Data;
    };

    struct SearchResults {
        std::vector<DirectoryInfo> Directories;
        std::vector<Asset> Assets;
    };

    class AssetManager {
    public:
        using AssetsChangeEventFn = std::function<void()>;

    public:
        static void Init();
        static void SetAssetChangeCallback(const AssetsChangeEventFn &callback);
        static DirectoryInfo &GetDirectoryInfo(int index);
        static std::vector<Asset> GetAssetsInDirectory(int dirIndex);
        static std::vector<std::string> GetDirectoryNames(const std::string &filepath);

        static SearchResults SearchFiles(const std::string &query, const std::string &searchPath);
        static std::string GetParentPath(const std::string &path);

        template <typename T>
        static Ref<T> InstantiateAsset(UUID assetId) {
            MND_CORE_ASSERT(s_LoadedAssets.find(assetId) != s_LoadedAssets.end());
            return Ref<T>((T *)s_LoadedAssets[assetId].Data);
        }

        static Asset &GetAssetFromId(UUID assetId) {
            MND_CORE_ASSERT(s_LoadedAssets.find(assetId) != s_LoadedAssets.end());
            return s_LoadedAssets[assetId];
        }

        // TODO: This will NOT live here
        static bool MoveFile(const std::string &originalPath, const std::string &dest);

        static std::string StripExtras(const std::string &filename);

    private:
        static std::string ParseFilename(const std::string &filepath, const char &delim);
        static std::string ParseFileType(const std::string &filename);
        static std::string RemoveExtension(const std::string &filename);

        static void ImportAsset(const std::string &filepath, bool reimport = false, int parentIndex = -1);
        static void ConvertAsset(const std::string &assetPath, const std::string &conversionType);
        static int ProcessDirectory(const std::string &directoryPath, int parentIndex = -1);
        static void ReloadAssets();
        // static void WriteAssetsToDisk();

        static void OnFileSystemChanged(FileSystemChangedEvent e);

    private:
        static std::unordered_map<size_t, Asset> s_LoadedAssets;
        static std::vector<DirectoryInfo> s_Directories;
        static AssetsChangeEventFn s_AssetsChangeCallback;
    };

} // namespace Monado
