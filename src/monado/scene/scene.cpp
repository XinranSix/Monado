#include "monado/scene/scene.h"
#include "monado/scene/entity.h"
#include "monado/scene/components.h"
#include "monado/scene/scriptableEntity.h"
#include "monado/renderer/renderer2D.h"
#include "monado/scene/sceneCamera.h"

// #include "monado/scripting/scriptEngine.h"

#include "glm/glm.hpp"

/* #include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h" */

namespace Monado {

    // TODO:��������ԭ��
    // Ϊ���Ƴ�����ʵ�������ĸ�������
    template <typename... Component>
    static void CopyComponent(entt::registry &dst, entt::registry &src,
                              const std::unordered_map<UUID, entt::entity> &enttMap) {
        (
            [&]() {
                auto view = src.view<Component>();
                // 2.1�����ɳ�������uuid����ľ�ʵ��
                for (auto srcEntity : view) {
                    // 2.2��** ��ʵ���uuid - map - ��Ӧ��ʵ�� * *
                    //                    entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
                    // 3.1��ȡ��ʵ������
                    //      auto &srcComponent = src.get<Component>(srcEntity);
                    // 3.2Ȼ����API��** ���ƾ�ʵ����������ʵ��**
                    // dst.emplace_or_replace<Component>(dstEntity, srcComponent);
                }
            }(),
            ...);
    }

    template <typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry &dst, entt::registry &src,
                              const std::unordered_map<UUID, entt::entity> &enttMap) {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template <typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src) {
        CopyComponentIfExists<Component...>(dst, src);
    }

    // TODO: 待完善
    Ref<Scene> Scene::Copy(Ref<Scene> other) {
        /*   // 1.1�����³���
          Ref<Scene> newScene = CreateRef<Scene>();

          newScene->m_ViewportWidth = other->m_ViewportWidth;
          newScene->m_ViewportHeight = other->m_ViewportHeight;

          auto &srcSceneRegistry = other->m_Registry;
          auto &dstSceneRegistry = newScene->m_Registry;
          std::unordered_map<UUID, entt::entity> enttMap;

          auto idView = srcSceneRegistry.view<IDComponent>();
          for (auto e : idView) {
              UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
              const auto &name = srcSceneRegistry.get<TagComponent>(e).Tag;
              // 1.2Ϊ�³��������;ɳ���ͬ����uuid��ʵ��
              Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
              // 1.3����**map���루��ʵ���uuid��Ӧ��ʵ�壩�Ĺ�ϵ**
              enttMap[uuid] = (entt::entity)newEntity; // UUID����Ҫ��ϣ
          }

          // �������������IDcomponent��tagcomponent����CreateEntityWithUUID������
          CopyComponent(AllComponents {}, dstSceneRegistry, srcSceneRegistry, enttMap);

          return newScene; */
        return {};
    }

    Scene::Scene() {}

    Scene::~Scene() {}

    // TODO: 待完善
    void Scene::DuplicateEntity(Entity entity) {
        /*   // 1.������ʵ��ͬ������ʵ��
          std::string name = entity.GetName();
          Entity newEntity = CreateEntity(name);
          // 2.�������
          // Copy components (except IDComponent and TagComponent)
          CopyComponentIfExists(AllComponents {}, newEntity, entity); */
    }

    Entity Scene::CreateEnitty(std::string name) {
        // 添加默认组件
        Entity entity = { m_Registry.create(), this };
        // 一般来说一个实体必然有一个 TransformComponent
        entity.AddComponent<TransformComponent>();
        auto &tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity) { m_Registry.destroy((entt::entity)entity); }

    void Scene::OnRuntimeStart() {}
    void Scene::OnRuntimeStop() {}

    void Scene::OnSimulationStart() {}

    void Scene::OnSimulationStop() {}

    void Scene::OnUpdateRuntime(Timestep ts) {
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto &nsc) {
                if (!nsc.Instance) {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity { entity, this };
                    // 执行CameraController脚本的OnCreate函数，由虚函数指定
                    nsc.Instance->OnCreate();
                }
                // 执行CameraController脚本的OnUpdate函数
                nsc.Instance->OnUpdate(ts);
            });
        }

        Camera *mainCamera {};
        glm::mat4 cameraTransform {};
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view) {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
                if (camera.primary) {
                    mainCamera = &camera.camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }

        if (mainCamera) {

            Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);
            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group) {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
            }
            Renderer2D::EndScene();
        }
    }

    void Scene::OnUpdateSimulation(Timestep ts, EditorCamera &camera) {}

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera) {
        Renderer2D::BeginScene(camera);
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
        }
        Renderer2D::EndScene();
    }

    Entity Scene::GetPrimaryCameraEntity() {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            const auto &camera = view.get<CameraComponent>(entity);
            if (camera.primary) {
                return Entity { entity, this };
            }
        }
        return {};
    }

    void Scene::OnUpdate(Timestep ts) {
        // 获取到主摄像机，并且获取到摄像机的位置，用来计算投影矩阵projection
        /*       Camera *mainCamera = nullptr;
              glm::mat4 cameraTransform;
              {
                  auto group = m_Registry.view<TransformComponent, CameraComponent>();
                  for (auto entity : group) {
                      auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

                      if (camera.primary) {
                          mainCamera = &camera.camera;
                          cameraTransform = transform.GetTransform();
                      }
                  }
              }
              ///
              // 注意这，BeginScene中传入主摄像机的投影矩阵与主摄像机的transform矩阵
              if (mainCamera) {
                  Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);
                  auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
                  for (auto entity : group) {
                      auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                      Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
                  }
                  Renderer2D::EndScene();
              } */
        /*
                m_Registry.view<NativeScriptComponent>().each([=, this](auto entity, auto &nsc) {
                    if (!nsc.Instance) {
                        nsc.Instance = nsc.InstantiateScript();
                        nsc.Instance->m_Entity = Entity { entity, this };
                        // 执行CameraController脚本的OnCreate函数，由虚函数指定
                        nsc.Instance->OnCreate();
                    }
                    // 执行CameraController脚本的OnUpdate函数
                    nsc.Instance->OnUpdate(ts);
                }); */
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            auto &cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.fixedAspectRatio) {
                cameraComponent.camera.SetViewportSize(width, height);
            }
        }
    }

    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T &component) {
        // 静态断言：false，代表在编译前就会执行， 但是编译器这里不会报错，说明这段代码不会编译吧。。
        // 而且打了断点，也不行，证明这段代码只是声明作用吧。
        // FIXME: 为什么会执行到这里？？？
        // static_assert(false);
    }

    template <>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent &component) {}

    template <>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent &component) {
        component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template <>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component) {}

    template <>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent &component) {}

    template <>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent &component) {}

    void Scene::OnPhysics2DStart() {}

    void Scene::OnPhysics2DStop() {}

} // namespace Monado
