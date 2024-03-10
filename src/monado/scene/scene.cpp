#include "monado/scene/scene.h"
#include "monado/scene/entity.h"
#include "monado/scene/components.h"
#include "monado/scene/scriptableEntity.h"
#include "monado/renderer/renderer2D.h"
#include "monado/scene/sceneCamera.h"

// #include "monado/scripting/scriptEngine.h"

#include "glm/glm.hpp"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Monado {

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

    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType) {
        switch (bodyType) {
        case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
        case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
        case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
        }

        MND_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
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

    Entity Scene::CreateEntity(std::string name) {
        // 添加默认组件
        Entity entity = { m_Registry.create(), this };
        // 一般来说一个实体必然有一个 TransformComponent
        entity.AddComponent<TransformComponent>();
        auto &tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity) { m_Registry.destroy((entt::entity)entity); }

    void Scene::OnRuntimeStart() {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view) {
            Entity entity = { e, this };
            auto &transform = entity.GetComponent<TransformComponent>();
            auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef;
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z;

            b2Body *body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;

            if (entity.HasComponent<BoxCollider2DComponent>()) {
                auto &bc2d = entity.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.Density;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    void Scene::OnRuntimeStop() {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }

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
                nsc.Instance->OnUpdate(ts);
            });
        }

        // Physics
        {
            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2DComponent>();
            for (auto e : view) {
                Entity entity = { e, this };
                auto &transform = entity.GetComponent<TransformComponent>();
                auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

                b2Body *body = (b2Body *)rb2d.RuntimeBody;
                const auto &position = body->GetPosition();
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = body->GetAngle();
            }
        }

        Camera *mainCamera {};
        glm::mat4 cameraTransform {};
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view) {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
                if (camera.Primary) {
                    mainCamera = &camera.Camera;
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
            Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
        }
        Renderer2D::EndScene();
    }

    Entity Scene::GetPrimaryCameraEntity() {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            const auto &camera = view.get<CameraComponent>(entity);
            if (camera.Primary) {
                return Entity { entity, this };
            }
        }
        return {};
    }

    void Scene::OnUpdate(Timestep ts) {}

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            auto &cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio) {
                cameraComponent.Camera.SetViewportSize(width, height);
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
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template <>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component) {}

    template <>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent &component) {}

    template <>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent &component) {}

    template <>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent &component) {}

    template <>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent &component) {}

    void Scene::OnPhysics2DStart() {}

    void Scene::OnPhysics2DStop() {}

} // namespace Monado
