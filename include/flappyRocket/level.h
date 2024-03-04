#pragma once

#include "player.h"
#include "monado/renderer/orthographicCameraController.h"

// 整个游戏区间的y坐标在[-10, 10]区间内, Player从(0,0,0)开始自动向右移动
struct Column {
    glm::vec3 topPos;
    glm::vec3 bottomPos;

    glm::vec2 scale = { 1.5f, 2.0f }; // the Column can be expanded in x and y axis
};

// 对于整个关卡区间的y值:
// [-1, 1]为玩家的竖直活动区间
// [-Infinity, -1]和[1, Infinity]区间为关卡的上下边界
// 但是由于正交Camera是紧跟Player的, Camera的显示范围为横轴长度为4, 纵轴长度为2.25(16:9)
class Level {
public:
    // 默认正交相机的radio为16:9, zoom为1
    Level();

    static glm::vec4 HSVtoRGB(const glm::vec3 &hsv);

    void Init();
    void Reset();

    void OnUpdate(Monado::Timestep ts);
    void OnRender();
    void OnImGuiRender();

    bool IsGameOver() const { return m_GameOver; }

    Monado::OrthographicCameraController &GetCameraController() { return m_OrthoCameraController; }
    glm::vec4 GetDynamicCollor() { return m_DynamicColor; }

    void SetPlayer(const Player &p) { m_Player = p; }
    Player &GetPlayer() { return m_Player; }

    void SetSpacePressed(bool pressed) { m_SpacePressed = pressed; }
    std::vector<Column> &GetColumns() { return m_Collumns; }

    std::shared_ptr<Monado::Texture2D> GetTriangleTex() { return m_TriangleTexture; }

private:
    bool CollisionTest();
    void CreateInitialColumns();
    void UpdateColumns();

    void UpdateColumnBounds();

    void GameOver();

private:
    bool m_GameOver = false;

    float m_LastPlayerPosX = 0.0f;
    // 色调盘和半径都是确定的, 只有色调H会改变
    glm::vec3 m_ColumnHSV = { 0.0f, 0.8f, 0.8f }; // H: Hue, S: Saturation,  V: value
    float m_Gravity = 38.0f;
    float m_UpAcceleration = 100.0f;

    Player m_Player;
    std::vector<Column> m_Collumns;                       // 关卡信息数组
    std::shared_ptr<Monado::Texture2D> m_TriangleTexture; // 关卡对应的Texture2D数组
    bool m_SpacePressed = false;
    Monado::OrthographicCameraController m_OrthoCameraController;
    glm::vec4 m_DynamicColor = { 1.0f, 0.3f, 0.3f, 1.0f };

public:
    std::vector<glm::vec2> m_DebugCollisions; // 存储Player发生碰撞时的Player的位置

    // 向量的齐次坐标为0, 点为1
    glm::vec4 m_TriVertices[3] {
        { 0.4f, -0.4f, 0.0f, 1.0f }, // 注意, 最后一列必须都是1, 因为他们代表点而不是向量
        { 0.0f, 0.4f, 0.0f, 1.0f },
        { -0.4f, -0.4f, 0.0f, 1.0f },
    };

    std::vector<glm::vec4> m_ColumnBounds;
};
