#include "flappyRocket/level.h"
#include "flappyRocket/random.h"

#include <filesystem>

static bool PointInTri(const glm::vec2 &p, glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2) {
    float s = p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y;
    float t = p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y;

    if ((s < 0) != (t < 0))
        return false;

    float A = -p1.y * p2.x + p0.y * (p2.x - p1.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y;

    return A < 0 ? (s <= 0 && s + t >= A) : (s >= 0 && s + t <= A);
}

bool Level::CollisionTest() {
    if (m_Player.GetPosition().y > 0.99f || m_Player.GetPosition().y < -0.99f)
        return true;

    for (size_t i = 0; i < m_ColumnBounds.size(); i += 3) {
        glm::vec2 a = { m_ColumnBounds[i].x, m_ColumnBounds[i].y };
        glm::vec2 b = { m_ColumnBounds[i + 1].x, m_ColumnBounds[i + 1].y };
        glm::vec2 c = { m_ColumnBounds[i + 2].x, m_ColumnBounds[i + 2].y };

        // 遍历Player周围四个Collider顶点, 是否在此三角形内
        for (size_t j = 0; j < 4; j++) {
            if (PointInTri({ m_Player.m_CurVertices[j].x, m_Player.m_CurVertices[j].y }, a, b, c))
                return true;
        }
    }

    return false;
}

void Level::CreateInitialColumns() {
    m_Collumns.clear();
    float columnIntervelX = 4.0f / 3;
    for (size_t i = 0; i < 5; i++) {
        Column c;

        // 整个Player可活动数值区间为[-1.225, 1.225]
        float deltaY = -0.5f + Random::Float();
        float gap = Random::Float() * 0.3f;

        c.topPos = { (i + 1) * columnIntervelX, 0.9f + gap + deltaY, 0 };
        c.bottomPos = { (i + 1) * columnIntervelX, -0.9f - gap + deltaY, 0 }; // 三角形的高度大概是0.85

        m_Collumns.push_back(c);
    }

    UpdateColumnBounds();
}

void Level::UpdateColumns() {
    // 从第一个Column开始, Player每走三分之一个屏幕宽度, 就Update最左边的Columns
    float currentPosX = m_Player.GetPosition().x;
    float columnIntervelX = 4.0f / 3.0f;

    int last = int(m_LastPlayerPosX / columnIntervelX);
    int now = int(currentPosX / columnIntervelX);

    if (now > last && now > 2) // 走到第三个关卡才开始更新第一个关卡
    {
        m_Collumns[0] = m_Collumns[1];
        m_Collumns[1] = m_Collumns[2];
        m_Collumns[2] = m_Collumns[3];
        m_Collumns[3] = m_Collumns[4];

        float deltaY = -0.5f + Random::Float();
        float gap = Random::Float() * 0.3f;

        m_Collumns[4].topPos.x += columnIntervelX;
        m_Collumns[4].topPos.y = 0.9f + gap + deltaY;
        m_Collumns[4].bottomPos.x += columnIntervelX;
        m_Collumns[4].bottomPos.y = -0.9f - gap + deltaY;
    }

    UpdateColumnBounds();
}

void Level::UpdateColumnBounds() {
    for (size_t i = 0; i < m_Collumns.size(); i++) {
        auto col = m_Collumns[i];

        // Upper tri
        for (size_t i = 0; i < 3; i++) {
            auto trans = glm::scale(glm::mat4(1.0f), { 1.5f, 2.0f, 1.0f });
            trans = glm::rotate(trans, glm::radians(180.0f), { 0, 0, 1 });

            glm::mat4 globalTrans = glm::translate(glm::mat4(1.0f), col.topPos);
            trans = globalTrans * trans;

            glm::vec4 pos = trans * m_TriVertices[i];
            m_ColumnBounds.push_back(pos);
        }

        // Bottom tri
        for (size_t i = 0; i < 3; i++) {
            auto trans = glm::scale(glm::mat4(1.0f), { 1.5f, 2.0f, 1.0f });
            // trans = glm::rotate(trans, glm::radians(0.0f), { 0,0,1 });

            glm::mat4 globalTrans = glm::translate(glm::mat4(1.0f), col.bottomPos);
            trans = globalTrans * trans;

            glm::vec4 pos = trans * m_TriVertices[i];
            m_ColumnBounds.push_back(pos);
        }
    }
}

void Level::GameOver() { m_GameOver = true; }

Level::Level() : m_OrthoCameraController(1.7778f, 1.0f) {
    // 因为我们的屏幕一般是16:9，或者16:10的，所以这里的横向区间比纵向区间一般要是这个比例，所以我现在把横轴长度改成4，纵轴长度改成了4/16
    // * 9 = 2.25
    m_OrthoCameraController.GetCamera() = Monado::OrthographicCamera(-2.0f, 2.0f, -1.225f, 1.225f);

    CreateInitialColumns();

    std::string texturePath = "asset/textures/Triangle.png";
    m_TriangleTexture = Monado::Texture2D::Create(texturePath);
}

void Level::Init() {}

void Level::Reset() {
    CreateInitialColumns();

    m_Player.Reset();
    m_OrthoCameraController.GetCamera() = Monado::OrthographicCamera(-2.0f, 2.0f, -1.225f, 1.225f);
    m_GameOver = false;
    m_ColumnBounds.clear();
    m_DebugCollisions.clear();
}

void Level::OnUpdate(Monado::Timestep ts) {
    m_LastPlayerPosX = m_Player.GetPosition().x;
    m_Player.OnUpdate(ts);

    // TODO: 下面的代码其实应该放到m_Player.OnUpdate函数里的
    // Update player's positiion
    auto pos = m_Player.GetPosition();
    glm::vec2 deltaPos =
        glm::vec2(1.0f * m_Player.GetVelocity().x, m_Player.GetForward().y * abs(m_Player.GetVelocity().y)) *
        ts.GetSeconds() * m_Player.GetSpeed();
    pos = pos + deltaPos;

    // 高度限制在[-1, 1]区间
    if (pos.y > 1.f) {
        pos.y = 1.0f;
        deltaPos.y = 0.0f;
    }
    if (pos.y < -1.f) {
        pos.y = -1.0f;
        deltaPos.y = 0.0f;
    }

    m_Player.SetPosition(pos);

    // 不断改变HSV里的色调Hue
    m_ColumnHSV.x += 0.1f * ts;
    if (m_ColumnHSV.x > 1.0f)
        m_ColumnHSV.x = 0.0f;

    m_DynamicColor = HSVtoRGB(m_ColumnHSV);

    // Camera跟随Player
    auto s = m_OrthoCameraController.GetCamera().GetPosition();
    s.x = s.x + deltaPos.x;
    s.y = s.y + deltaPos.y;
    m_OrthoCameraController.GetCamera().SetPosition(s);

    if (m_SpacePressed) {
        auto p = m_Player.GetVelocity();
        p.y += m_UpAcceleration * ts.GetSeconds();
        m_Player.SetVelocity(p);
    } else {
        auto p = m_Player.GetVelocity();
        p.y -= m_Gravity * ts.GetSeconds();
        m_Player.SetVelocity(p);
    }

    UpdateColumns();

    if (CollisionTest()) {
        m_DebugCollisions.push_back(m_Player.GetPosition());
        GameOver();
    }

    if (m_SpacePressed)
        m_Player.Emit();
}

glm::vec4 Level::HSVtoRGB(const glm::vec3 &hsv) {
    int H = (int)(hsv.x * 360.0f);
    double S = hsv.y;
    double V = hsv.z;

    double C = S * V;
    double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    double m = V - C;
    double Rs, Gs, Bs;

    if (H >= 0 && H < 60) {
        Rs = C;
        Gs = X;
        Bs = 0;
    } else if (H >= 60 && H < 120) {
        Rs = X;
        Gs = C;
        Bs = 0;
    } else if (H >= 120 && H < 180) {
        Rs = 0;
        Gs = C;
        Bs = X;
    } else if (H >= 180 && H < 240) {
        Rs = 0;
        Gs = X;
        Bs = C;
    } else if (H >= 240 && H < 300) {
        Rs = X;
        Gs = 0;
        Bs = C;
    } else {
        Rs = C;
        Gs = 0;
        Bs = X;
    }

    return { (Rs + m), (Gs + m), (Bs + m), 1.0f };
}

void Level::OnRender() {}

void Level::OnImGuiRender() {}
