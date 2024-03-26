#include "monado/editor/assetManagerPanel.h"
#include "monado/Core/application.h"
#include "monado/utilities/dragDropData.h"
#include "monado/utilities/assetManager.h"

namespace Monado {

    AssetManagerPanel::AssetManagerPanel() {
        AssetManager::SetAssetChangeCallback([&]() { UpdateCurrentDirectory(m_CurrentDirIndex); });

        m_FolderTex = Texture2D::Create("assets/editor/folder.png");
        m_FavoritesTex = Texture2D::Create("assets/editor/favourites.png");
        m_GoBackTex = Texture2D::Create("assets/editor/back.png");
        m_ScriptTex = Texture2D::Create("assets/editor/script.png");
        m_ResourceTex = Texture2D::Create("assets/editor/resource.png");
        m_SceneTex = Texture2D::Create("assets/editor/scene.png");

        m_AssetIconMap[-1] = Texture2D::Create("assets/editor/file.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("hdr")] = Texture2D::Create("assets/editor/file.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("fbx")] = Texture2D::Create("assets/editor/fbx.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("obj")] = Texture2D::Create("assets/editor/obj.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("wav")] = Texture2D::Create("assets/editor/wav.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("cs")] = Texture2D::Create("assets/editor/csc.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("png")] = Texture2D::Create("assets/editor/png.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("blend")] = Texture2D::Create("assets/editor/blend.png");
        m_AssetIconMap[AssetTypes::GetAssetTypeID("msc")] = Texture2D::Create("assets/editor/monado.png");

        m_BackbtnTex = Texture2D::Create("assets/editor/btn_back.png");
        m_FwrdbtnTex = Texture2D::Create("assets/editor/btn_fwrd.png");
        m_FolderRightTex = Texture2D::Create("assets/editor/folder_hierarchy.png");
        m_SearchTex = Texture2D::Create("assets/editor/search.png");
        m_TagsTex = Texture2D::Create("assets/editor/tags.png");
        m_GridView = Texture2D::Create("assets/editor/grid.png");
        m_ListView = Texture2D::Create("assets/editor/list.png");

        m_BaseDirIndex = 0;
        m_BaseProjectDir = AssetManager::GetDirectoryInfo(m_BaseDirIndex);
        UpdateCurrentDirectory(m_BaseDirIndex);

        memset(m_InputBuffer, 0, sizeof(m_InputBuffer));
    }

    void AssetManagerPanel::DrawDirectoryInfo(DirectoryInfo &dir) {
        if (ImGui::TreeNode(dir.DirectoryName.c_str())) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                UpdateCurrentDirectory(dir.DirectoryIndex);

            for (int j = 0; j < dir.ChildrenIndices.size(); j++) {
                DirectoryInfo &child = AssetManager::GetDirectoryInfo(dir.ChildrenIndices[j]);
                DrawDirectoryInfo(child);
            }

            ImGui::TreePop();
        }

        if (m_IsDragging && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
            m_MovePath = dir.Filepath;
        }
    }

    void AssetManagerPanel::OnImGuiRender() {
        ImGui::Begin("Project", NULL, ImGuiWindowFlags_None);
        {
            UI::BeginPropertyGrid();
            ImGui::SetColumnOffset(1, 240);

            ImGui::BeginChild("##folders_common");
            {
                if (ImGui::CollapsingHeader("Content", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
                    for (int i = 0; i < m_BaseProjectDir.ChildrenIndices.size(); i++) {
                        DrawDirectoryInfo(AssetManager::GetDirectoryInfo(m_BaseProjectDir.ChildrenIndices[i]));
                    }
                }
            }
            ImGui::EndChild();

            if (ImGui::BeginDragDropTarget()) {
                auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
                if (data) {
                    std::string file = (char *)data->Data;
                    if (AssetManager::MoveFile(file, m_MovePath)) {
                        MND_CORE_INFO("Moved File: " + file + " to " + m_MovePath);
                        UpdateCurrentDirectory(m_CurrentDirIndex);
                    }
                    m_IsDragging = false;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::NextColumn();

            ImGui::BeginChild("##directory_structure",
                              ImVec2(ImGui::GetColumnWidth() - 12, ImGui::GetWindowHeight() - 50));
            {
                ImGui::BeginChild("##directory_breadcrumbs", ImVec2(ImGui::GetColumnWidth() - 100, 30));
                RenderBreadCrumbs();
                ImGui::EndChild();

                ImGui::BeginChild("Scrolling");

                if (!m_DisplayListView)
                    ImGui::Columns(10, nullptr, false);

                for (DirectoryInfo &dir : m_CurrentDirChildren) {
                    if (m_DisplayListView)
                        RenderDirectoriesListView(dir.DirectoryIndex);
                    else
                        RenderDirectoriesGridView(dir.DirectoryIndex);

                    ImGui::NextColumn();
                }

                for (Asset &asset : m_CurrentDirAssets) {
                    if (m_DisplayListView)
                        RenderFileListView(asset);
                    else
                        RenderFileGridView(asset);

                    ImGui::NextColumn();
                }

                if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight |
                                                          ImGuiPopupFlags_NoOpenOverExistingPopup)) {
                    if (ImGui::BeginMenu("New")) {
                        if (ImGui::MenuItem("Folder")) {
                            MND_CORE_INFO("Creating Folder...");
                        }

                        if (ImGui::MenuItem("Scene")) {
                            MND_CORE_INFO("Creating Scene...");
                        }

                        if (ImGui::MenuItem("Script")) {
                            MND_CORE_INFO("Creating Script...");
                        }

                        if (ImGui::MenuItem("Prefab")) {
                            MND_CORE_INFO("Creating Prefab...");
                        }

                        ImGui::EndMenu();
                    }

                    ImGui::EndPopup();
                }

                ImGui::EndChild();
            }
            ImGui::EndChild();

            if (ImGui::BeginDragDropTarget()) {
                auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
                if (data) {
                    std::string a = (char *)data->Data;

                    if (AssetManager::MoveFile(a, m_MovePath)) {
                        UpdateCurrentDirectory(m_CurrentDirIndex);
                    }

                    m_IsDragging = false;
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Create")) {
                    if (ImGui::MenuItem("Import New Asset", "Ctrl + O")) {
                        std::string filename = Application::Get().OpenFile("");
                        // m_AssetManager.ProcessAsset(filename);
                    }

                    if (ImGui::MenuItem("Refresh", "Ctrl + R")) {
                        m_BaseProjectDir = AssetManager::GetDirectoryInfo(m_BaseDirIndex);
                        UpdateCurrentDirectory(m_CurrentDirIndex);
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            UI::EndPropertyGrid();
        }
        ImGui::End();
    }

    void AssetManagerPanel::RenderFileListView(Asset &asset) {
        size_t fileID = AssetTypes::GetAssetTypeID(asset.Extension);
        RendererID iconRef = m_AssetIconMap[fileID]->GetRendererID();
        ImGui::Image((ImTextureID)iconRef, ImVec2(20, 20));

        ImGui::SameLine();

        if (ImGui::Selectable(asset.FileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            /*if (ImGui::IsMouseDoubleClicked(0))
                    m_AssetManager.HandleAsset(m_CurrentDir[dirIndex].AbsolutePath);*/
        }

        HandleDragDrop(iconRef, asset);
    }

    void AssetManagerPanel::RenderFileGridView(Asset &asset) {
        ImGui::BeginGroup();

        size_t fileID = AssetTypes::GetAssetTypeID(asset.Extension);
        RendererID iconRef = m_AssetIconMap[fileID]->GetRendererID();
        float columnWidth = ImGui::GetColumnWidth();

        ImGui::ImageButton((ImTextureID)iconRef, { columnWidth - 10.0F, columnWidth - 10.0F });

        HandleDragDrop(iconRef, asset);

        std::string newFileName = AssetManager::StripExtras(asset.FileName);
        ImGui::TextWrapped(newFileName.c_str());
        ImGui::EndGroup();
    }

    void AssetManagerPanel::HandleDragDrop(RendererID icon, Asset &asset) {
        // Drag 'n' Drop Implementation For File Moving
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::Image((ImTextureID)icon, ImVec2(20, 20));
            ImGui::SameLine();
            ImGui::Text(asset.FileName.c_str());
            int size = sizeof(const char *) + strlen(asset.FilePath.c_str());
            ImGui::SetDragDropPayload("selectable", asset.FilePath.c_str(), size);
            m_IsDragging = true;
            ImGui::EndDragDropSource();
        }

        // Drag 'n' Drop Implementation For Asset Handling In Scene Viewport
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::Image((ImTextureID)icon, ImVec2(20, 20));
            ImGui::SameLine();
            ImGui::Text(asset.FileName.c_str());
            ImGui::SetDragDropPayload("scene_entity_assetsP", &asset.ID, sizeof(UUID));
            m_IsDragging = true;

            ImGui::EndDragDropSource();
        }
    }

    void AssetManagerPanel::RenderDirectoriesListView(int dirIndex) {
        ImGui::Image((ImTextureID)m_FolderTex->GetRendererID(), ImVec2(20, 20));
        ImGui::SameLine();

        auto &folderData = AssetManager::GetDirectoryInfo(dirIndex);

        if (ImGui::Selectable(folderData.DirectoryName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(0)) {
                m_PrevDirIndex = m_CurrentDirIndex;
                UpdateCurrentDirectory(dirIndex);
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            ImGui::Image((ImTextureID)m_FolderTex->GetRendererID(), ImVec2(20, 20));
            ImGui::SameLine();
            ImGui::Text(folderData.DirectoryName.c_str());
            ImGui::SetDragDropPayload("selectable", &dirIndex, sizeof(int));
            m_IsDragging = true;
            ImGui::EndDragDropSource();
        }
    }

    void AssetManagerPanel::RenderDirectoriesGridView(int dirIndex) {
        ImGui::BeginGroup();

        float columnWidth = ImGui::GetColumnWidth();
        ImGui::ImageButton((ImTextureID)m_FolderTex->GetRendererID(), { columnWidth - 10.0F, columnWidth - 10.0F });

        auto &folderData = AssetManager::GetDirectoryInfo(dirIndex);

        if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
            m_PrevDirIndex = m_CurrentDirIndex;
            UpdateCurrentDirectory(dirIndex);
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            ImGui::Image((ImTextureID)m_FolderTex->GetRendererID(), ImVec2(20, 20));
            ImGui::SameLine();
            ImGui::Text(folderData.DirectoryName.c_str());
            ImGui::SetDragDropPayload("selectable_directory", &dirIndex, sizeof(int));
            m_IsDragging = true;
            ImGui::EndDragDropSource();
        }

        ImGui::TextWrapped(folderData.DirectoryName.c_str());
        ImGui::EndGroup();
    }

    void AssetManagerPanel::RenderBreadCrumbs() {
        RendererID viewTexture = m_DisplayListView ? m_ListView->GetRendererID() : m_GridView->GetRendererID();
        if (ImGui::ImageButton((ImTextureID)viewTexture, ImVec2(20, 18))) {
            m_DisplayListView = !m_DisplayListView;
        }

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)m_BackbtnTex->GetRendererID(), ImVec2(20, 18))) {
            if (m_CurrentDirIndex == m_BaseDirIndex)
                return;
            m_NextDirIndex = m_CurrentDirIndex;
            m_PrevDirIndex = AssetManager::GetDirectoryInfo(m_CurrentDirIndex).ParentIndex;
            UpdateCurrentDirectory(m_PrevDirIndex);
        }

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)m_FwrdbtnTex->GetRendererID(), ImVec2(20, 18))) {
            UpdateCurrentDirectory(m_NextDirIndex);
        }

        ImGui::SameLine();

        {
            ImGui::PushItemWidth(200);

            if (ImGui::InputTextWithHint("", "Search...", m_InputBuffer, 100)) {
                SearchResults results = AssetManager::SearchFiles(m_InputBuffer, m_CurrentDir.Filepath);

                if (strlen(m_InputBuffer) == 0) {
                    UpdateCurrentDirectory(m_CurrentDirIndex);
                } else {
                    m_CurrentDirChildren = results.Directories;
                    m_CurrentDirAssets = results.Assets;
                }
            }

            ImGui::PopItemWidth();
        }

        ImGui::SameLine();

        if (m_UpdateBreadCrumbs) {
            m_BreadCrumbData.clear();

            int currentDirIndex = m_CurrentDirIndex;
            while (currentDirIndex != -1) {
                DirectoryInfo &dirInfo = AssetManager::GetDirectoryInfo(currentDirIndex);
                m_BreadCrumbData.push_back(dirInfo);
                currentDirIndex = dirInfo.ParentIndex;
            }

            std::reverse(m_BreadCrumbData.begin(), m_BreadCrumbData.end());

            m_UpdateBreadCrumbs = false;
        }

        for (int i = 0; i < m_BreadCrumbData.size(); i++) {
            if (m_BreadCrumbData[i].DirectoryName != "assets")
                ImGui::Text("/");

            ImGui::SameLine();

            int size = strlen(m_BreadCrumbData[i].DirectoryName.c_str()) * 7;

            if (ImGui::Selectable(m_BreadCrumbData[i].DirectoryName.c_str(), false, 0, ImVec2(size, 22))) {
                UpdateCurrentDirectory(m_BreadCrumbData[i].DirectoryIndex);
            }

            ImGui::SameLine();
        }

        ImGui::SameLine();

        ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - 400, 0));
        ImGui::SameLine();
    }

    void AssetManagerPanel::RenderBottom() {
        /* Will be used for object select indication. Ex. 3 folders 1 file selected */
        ImGui::BeginChild("##nav", ImVec2(ImGui::GetColumnWidth() - 12, 23));
        { ImGui::EndChild(); }
    }

    void AssetManagerPanel::UpdateCurrentDirectory(int dirIndex) {
        if (m_CurrentDirIndex != dirIndex)
            m_UpdateBreadCrumbs = true;

        m_CurrentDirChildren.clear();
        m_CurrentDirAssets.clear();

        m_CurrentDirIndex = dirIndex;
        m_CurrentDir = AssetManager::GetDirectoryInfo(m_CurrentDirIndex);

        for (int childIndex : m_CurrentDir.ChildrenIndices)
            m_CurrentDirChildren.push_back(AssetManager::GetDirectoryInfo(childIndex));

        m_CurrentDirAssets = AssetManager::GetAssetsInDirectory(m_CurrentDirIndex);
    }

} // namespace Monado
