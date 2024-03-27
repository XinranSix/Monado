#include "monado/editor/assetManagerPanel.h"
#include "monado/core/application.h"
#include "monado/utilities/dragDropData.h"
#include "monado/asset/assetManager.h"
#include "monado/utilities/fileSystem.h"
#include "monado/editor/assetEditorPanel.h"
#include "monado/core/input.h"

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
                DrawDirectoryInfo(AssetManager::GetDirectoryInfo(dir.ChildrenIndices[j]));
            }

            ImGui::TreePop();
        }

        if (m_IsDragging && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
            m_MovePath = dir.FilePath;
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
                    /*if (AssetManager::MoveFile(file, m_MovePath))
                    {
                           MND_CORE_INFO("Moved File: " + file + " to " + m_MovePath);
                            UpdateCurrentDirectory(m_CurrentDirIndex);
                    }*/
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

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0F, 0.0F, 0.0F, 0.0F));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2F, 0.205F, 0.21F, 0.25F));

                if (Input::IsKeyPressed(KeyCode::Escape) ||
                    (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemFocused())) {
                    if (m_SelectedAsset != -1)
                        m_SelectedAsset = -1;

                    if (m_SelectedDirectory != -1)
                        m_SelectedDirectory = -1;

                    m_RenamingSelected = false;
                }

                for (DirectoryInfo &dir : m_CurrentDirChildren) {
                    if (m_DisplayListView)
                        RenderDirectoriesListView(dir);
                    else
                        RenderDirectoriesGridView(dir);

                    ImGui::NextColumn();
                }

                for (Ref<Asset> &asset : m_CurrentDirAssets) {
                    if (m_DisplayListView)
                        RenderFileListView(asset);
                    else
                        RenderFileGridView(asset);

                    ImGui::NextColumn();
                }

                if (m_IsDragging && !ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.1F)) {
                    m_IsDragging = false;
                    m_DraggedAssetId = 0;
                }

                ImGui::PopStyleColor(2);

                if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight |
                                                          ImGuiPopupFlags_NoOpenOverExistingPopup)) {
                    if (ImGui::BeginMenu("Create")) {
                        if (ImGui::MenuItem("Folder")) {
                            bool created = FileSystem::CreateFolder(m_CurrentDir.FilePath + "/New Folder");

                            if (created) {
                                // TODO(Peter): Select folder and start renaming
                            }
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

                        if (ImGui::MenuItem("Physics Material")) {
                            AssetManager::CreateAsset<PhysicsMaterial>(
                                "New Physics Material.mpm", AssetType::PhysicsMat, m_CurrentDirIndex, 0.6F, 0.6F, 0.0F);
                            UpdateCurrentDirectory(m_CurrentDirIndex);
                        }

                        ImGui::EndMenu();
                    }

                    if (ImGui::MenuItem("Import")) {
                        // std::string filename = Application::Get().OpenFile("");
                    }

                    if (ImGui::MenuItem("Refresh")) {
                        UpdateCurrentDirectory(m_CurrentDirIndex);
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
                    /*if (AssetManager::MoveFile(a, m_MovePath))
                    {
                            UpdateCurrentDirectory(m_CurrentDirIndex);
                    }*/

                    m_IsDragging = false;
                }
                ImGui::EndDragDropTarget();
            }

            UI::EndPropertyGrid();
        }
        ImGui::End();
    }

    void AssetManagerPanel::RenderDirectoriesListView(DirectoryInfo &dirInfo) {
        ImGui::Image((ImTextureID)m_FolderTex->GetRendererID(), ImVec2(30, 30));
        ImGui::SameLine();

        if (ImGui::Selectable(dirInfo.DirectoryName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick,
                              ImVec2(0, 30))) {
            if (ImGui::IsMouseDoubleClicked(0)) {
                m_PrevDirIndex = m_CurrentDirIndex;
                UpdateCurrentDirectory(dirInfo.DirectoryIndex);
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            ImGui::Image((ImTextureID)m_FolderTex->GetRendererID(), ImVec2(20, 20));
            ImGui::SameLine();
            ImGui::Text(dirInfo.DirectoryName.c_str());
            ImGui::SetDragDropPayload("selectable", &dirInfo.DirectoryIndex, sizeof(int));
            m_IsDragging = true;
            ImGui::EndDragDropSource();
        }
    }

    void AssetManagerPanel::RenderDirectoriesGridView(DirectoryInfo &dirInfo) {
        ImGui::BeginGroup();
        float columnWidth = ImGui::GetColumnWidth();

        if (m_SelectedDirectory == dirInfo.DirectoryIndex)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25F, 0.25F, 0.25F, 0.75F));

        ImGui::ImageButton((ImTextureID)m_FolderTex->GetRendererID(), { columnWidth - 15.0F, columnWidth - 15.0F });

        if (m_SelectedDirectory == dirInfo.DirectoryIndex)
            ImGui::PopStyleColor();

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                m_PrevDirIndex = m_CurrentDirIndex;
                UpdateCurrentDirectory(dirInfo.DirectoryIndex);
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_SelectedDirectory = dirInfo.DirectoryIndex;
                m_SelectedAsset = -1;
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            ImGui::Image((ImTextureID)m_FolderTex->GetRendererID(), ImVec2(20, 20));
            ImGui::SameLine();
            ImGui::Text(dirInfo.DirectoryName.c_str());
            ImGui::SetDragDropPayload("selectable_directory", &dirInfo.DirectoryIndex, sizeof(int));
            m_IsDragging = true;
            ImGui::EndDragDropSource();
        }

        ImVec2 prevCursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(prevCursorPos.x + 10.0F, prevCursorPos.y - 21.0F));
        ImGui::SetNextItemWidth(columnWidth - 15.0F);

        if (m_SelectedDirectory != dirInfo.DirectoryIndex || !m_RenamingSelected)
            ImGui::TextWrapped(dirInfo.DirectoryName.c_str());

        ImGui::SetCursorPos(prevCursorPos);

        if (m_SelectedDirectory == dirInfo.DirectoryIndex) {
            if (!m_RenamingSelected && Input::IsKeyPressed(KeyCode::F2)) {
                memset(m_RenameBuffer, 0, 512);
                memcpy(m_RenameBuffer, dirInfo.DirectoryName.c_str(), dirInfo.DirectoryName.size());
                m_RenamingSelected = true;
            }

            if (m_RenamingSelected) {
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##rename_dummy", m_RenameBuffer, 512, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    MND_CORE_INFO("Renaming to {0}", m_RenameBuffer);
                    AssetManager::Rename(dirInfo.DirectoryIndex, m_RenameBuffer);
                    m_RenamingSelected = false;
                    m_SelectedDirectory = -1;
                    UpdateCurrentDirectory(m_CurrentDirIndex);
                }
            }
        }

        ImGui::EndGroup();
    }

    void AssetManagerPanel::RenderFileListView(Ref<Asset> &asset) {
        size_t fileID = AssetTypes::GetAssetTypeID(asset->Extension);
        RendererID iconRef = m_AssetIconMap[fileID]->GetRendererID();
        ImGui::Image((ImTextureID)iconRef, ImVec2(30, 30));

        ImGui::SameLine();

        ImGui::Selectable(asset->FileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 30));

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                AssetEditorPanel::OpenEditor(asset);

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                m_SelectedAsset = asset->Handle;
        }

        HandleDragDrop(iconRef, asset);
    }

    void AssetManagerPanel::RenderFileGridView(Ref<Asset> &asset) {
        ImGui::BeginGroup();

        size_t fileID = AssetTypes::GetAssetTypeID(asset->Extension);
        fileID = m_AssetIconMap.find(fileID) != m_AssetIconMap.end() ? fileID : -1;
        RendererID iconRef = m_AssetIconMap[fileID]->GetRendererID();
        float columnWidth = ImGui::GetColumnWidth();

        if (m_SelectedAsset == asset->Handle)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25F, 0.25F, 0.25F, 0.75F));

        ImGui::ImageButton((ImTextureID)iconRef, { columnWidth - 15.0F, columnWidth - 15.0F });

        if (m_SelectedAsset == asset->Handle)
            ImGui::PopStyleColor();

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                AssetEditorPanel::OpenEditor(asset);

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_SelectedAsset = asset->Handle;
                m_SelectedDirectory = -1;
            }
        }

        HandleDragDrop(iconRef, asset);

        ImGui::SetNextItemWidth(columnWidth - 15.0F);

        if (m_SelectedAsset != asset->Handle || !m_RenamingSelected)
            ImGui::TextWrapped(asset->FileName.c_str());

        if (m_SelectedAsset == asset->Handle) {
            if (!m_RenamingSelected && Input::IsKeyPressed(KeyCode::F2)) {
                memset(m_RenameBuffer, 0, 512);
                memcpy(m_RenameBuffer, asset->FileName.c_str(), asset->FileName.size());
                m_RenamingSelected = true;
            }

            if (m_RenamingSelected) {
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##rename_dummy", m_RenameBuffer, 512, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    MND_CORE_INFO("Renaming to {0}", m_RenameBuffer);
                    AssetManager::Rename(asset, m_RenameBuffer);
                    m_RenamingSelected = false;
                    m_SelectedAsset = -1;
                }
            }
        }

        ImGui::EndGroup();
    }

    void AssetManagerPanel::HandleDragDrop(RendererID icon, Ref<Asset> &asset) {
        if (m_DraggedAssetId != 0 && m_DraggedAssetId != asset->Handle)
            return;

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped) && ImGui::IsItemClicked(ImGuiMouseButton_Left))
            m_DraggedAssetId = asset->Handle;

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::Image((ImTextureID)icon, ImVec2(20, 20));
            ImGui::SameLine();
            ImGui::Text(asset->FileName.c_str());
            ImGui::SetDragDropPayload("scene_entity_assetsP", &m_DraggedAssetId, sizeof(AssetHandle));
            m_IsDragging = true;
            ImGui::EndDragDropSource();
        }
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
                SearchResults results = AssetManager::SearchFiles(m_InputBuffer, m_CurrentDir.FilePath);

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
