#include "flappyRocket/player.h"

Player::Player(const char *name) : m_Name { name } {
    // Smoke, 每隔一段时间Emit一次, 不管按不按空格键
    m_SmokeParticleProps.Position = { -0.5f, 0.0f };
    m_SmokeParticleProps.Velocity = { -1.0f, -2.0f }, m_SmokeParticleProps.VelocityVariation = { 4.0f, 2.0f };
    m_SmokeParticleProps.SizeBegin = 0.035f, m_SmokeParticleProps.SizeEnd = 0.0f,
    m_SmokeParticleProps.SizeVariation = 0.015f;
    m_SmokeParticleProps.ColorBegin = { 0.8f, 0.8f, 0.8f, 1.0f };
    m_SmokeParticleProps.ColorEnd = { 0.6f, 0.6f, 0.6f, 1.0f };
    m_SmokeParticleProps.LifeTime = 4.0f;

    // Flames, 只要按住空格键, 则一直Emit
    m_EngineParticleProps.Position = { -1.5f, 0.0f }; // Position会跟着Player走的
    m_EngineParticleProps.Velocity = { 8.5f, -2.0f }, m_EngineParticleProps.VelocityVariation = { 5.0f, 1.0f };
    m_EngineParticleProps.SizeBegin = 0.05f, m_EngineParticleProps.SizeEnd = 0.0f,
    m_EngineParticleProps.SizeVariation = 0.03f;
    m_EngineParticleProps.ColorBegin = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
    m_EngineParticleProps.ColorEnd = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
    m_EngineParticleProps.LifeTime = 1.0f;
}

// TODO: 有点乱, 就这样吧
void Player::OnUpdate(Monado::Timestep ts) {
    m_ParticleSystem.OnUpdate(ts, m_PlayerSpeed);

    m_Time += ts;
    if (m_Time > m_SmokeEmitInterval) {
        m_Time = 0.0f;

        // Smokes
        glm::vec2 emissionPoint = { -0.0f, -0.1f };
        float rotation = glm::radians(GetRotation());
        glm::vec4 rotatedEmissionPoint =
            glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::vec4(emissionPoint, 0.0f, 1.0f);
        m_SmokeParticleProps.Position = m_Position + glm::vec2 { rotatedEmissionPoint.x, rotatedEmissionPoint.y };
        // m_SmokeParticleProps.Velocity.y = -m_Velocity.y * 0.2f - 0.2f;

        m_ParticleSystem.Emit(m_SmokeParticleProps);
    }
}

void Player::Render() { m_ParticleSystem.OnRender(); }

void Player::Reset() {
    m_Position = { 0.0f, 0.0f };
    m_Velocity = { 10.0f, 0.0f };

    // m_EnginePower = 0.5f;

    m_Time = 0.0f;
}

void Player::SetPosition(const glm::vec2 &pos) {
    m_Position = pos;

    // Update Bound Vertices
    for (size_t i = 0; i < 4; i++) {
        auto trans = glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 1.0f });
        trans = glm::rotate(trans, glm::radians(m_Velocity.y * 3.0f), { 0, 0, 1 });

        auto translation = glm::translate(glm::mat4(1.0f), { pos.x, pos.y, 0.0f });
        trans = translation * trans;

        m_CurVertices[i] = trans * m_MeshVertices[i];
    }
}

void Player::Emit() {
    // Flames
    glm::vec2 emissionPoint = { -0.0f, -0.13f };
    float rotation = glm::radians(GetRotation());
    glm::vec4 rotatedEmissionPoint =
        glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::vec4(emissionPoint, 0.0f, 1.0f);
    m_EngineParticleProps.Position = m_Position + glm::vec2 { rotatedEmissionPoint.x, rotatedEmissionPoint.y };
    m_EngineParticleProps.Velocity.y = -m_Velocity.y * 0.2f - 0.2f;

    m_ParticleSystem.Emit(m_EngineParticleProps);
}
