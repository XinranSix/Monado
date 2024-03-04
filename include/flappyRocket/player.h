#pragma once

#include <memory>

#include "glm//gtx/transform.hpp"

#include "particleSystem.h"
#include "monado/renderer/texture.h"

class Player {
public:
    Player(const char *name = "Default Player");

    void OnUpdate(Monado::Timestep ts);
    void Render();
    void Reset();

    void SetTexture(std::shared_ptr<Monado::Texture2D> tex) { m_RocketTexture = tex; }
    std::shared_ptr<Monado::Texture2D> GetTexture() { return m_RocketTexture; }

    float GetRotation() {
        if (m_Velocity.y * 3.0f - 90.0f < -180)
            m_Velocity.y = -90.0f / 3.0f;

        if (m_Velocity.y * 3.0f - 90.0f > 0)
            m_Velocity.y = 90.0f / 3.0f;

        return m_Velocity.y * 3.0f - 90.0f;
    }
    const glm::vec2 &GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec2 &pos);

    glm::vec4 GetForward() {
        return glm::rotate(glm::mat4(1.0f), glm::radians(m_Velocity.y * 3.0f), { 0, 0, 1 }) * glm::vec4(1, 0, 0, 0);
    }
    glm::vec2 GetVelocity() { return m_Velocity; }
    void SetVelocity(const glm::vec2 &p) { m_Velocity = p; }
    uint32_t GetScore() const { return (uint32_t)((m_Position.x) / (4.0f / 3.0f)); }
    float GetSpeed() { return m_PlayerSpeed; }

    void Emit();

private:
    glm::vec2 m_Position = { 0.0f, 0.0f };
    glm::vec2 m_Velocity = { 10.0f, 0.0f };

    float m_EnginePower = 0.5f;

    float m_Time = 0.0f;
    float m_SmokeEmitInterval = 0.4f;

    ParticleProperties m_SmokeParticleProps, m_EngineParticleProps;
    ParticleSystem m_ParticleSystem;

    std::shared_ptr<Monado::Texture2D> m_RocketTexture;
    std::string m_Name;

public:
    // 向量的齐次坐标为0, 点为1
    glm::vec4 m_MeshVertices[4] { { -0.4f, -0.20f, 0.0f, 1.0f }, // 注意, 最后一列必须都是1, 因为他们代表点而不是向量
                                  { 0.4f, -0.20f, 0.0f, 1.0f },
                                  { 0.4f, 0.20f, 0.0f, 1.0f },
                                  { -0.4f, 0.20f, 0.0f, 1.0f } };

    glm::vec4 m_CurVertices[4];
    float m_PlayerSpeed = 0.075f;
};
