#include "monado/renderer/editorCamera.h"

#include "monado/core/input.h"
#include "monado/core/keyCodes.h"
#include "monado/core/mouseCodes.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Monado {
    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
          Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)) {
        UpdateView();
    }
    void EditorCamera::UpdateProjection() {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }
    void EditorCamera::UpdateView() {
        // m_Yaw = m_Pitch = 0.0f; // �������������ת�Ƕ� = ����ת
        // �����������ķ���õ��������λ��
        m_Position = CalculatePosition();

        glm::quat orientation = GetOrientation();
        m_ViewMatrix =
            glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation); // ��ͼ���� = λ�þ��� * ��ת����
        m_ViewMatrix = glm::inverse(m_ViewMatrix); // ȡ������������ͼ����
    }
    std::pair<float, float> EditorCamera::PanSpeed() const {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // ���2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // ���2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
        return { xFactor, yFactor };
    }
    float EditorCamera::RotationSpeed() const { return 0.8f; }
    float EditorCamera::ZoomSpeed() const {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f); // ��С0
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // ���100
        return speed;
    }
    void EditorCamera::OnUpdate(Timestep ts) {
        if (Input::IsKeyPressed(Key::LeftAlt)) {
            const glm::vec2 &mouse { Input::GetMouseX(), Input::GetMouseY() };
            /*
                    ����������ƶ�,mouse{1,0} - m_InitialMousePosition{0,0}= {1,0}
            */
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
            m_InitialMousePosition = mouse;

            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle)) {
                MousePan(delta); // �м�ƽ��
            } else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft)) {
                MouseRotate(delta); // �����ת
            } else if (Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
                MouseZoom(delta.y); // �Ҽ�����
            }
        }
        UpdateView();
    }
    void EditorCamera::OnEvent(Event &e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(MND_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
    }
    bool EditorCamera::OnMouseScroll(MouseScrolledEvent &e) {
        // delta ���Ի����ǰ�������
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    // �ƶ����㣬��ԭ��00�ƶ���xy��
    void EditorCamera::MousePan(const glm::vec2 &delta) {
        auto [xSpeed, ySpeed] = PanSpeed();
        /*
                ���ϸ�������Ϊ���������ҷ���(1, 0, 0)�ƶ���(1, 0)��ʹ����������Ч��
                        �ɼ������ƶ������������ƶ������෴�����ϸ��Ų���ʹ�ý�������ʱ��Ϊ�����Ƶ�(-1,0)��ʹ������Ϊ����
        */
        /*
                ������������ƶ�,delta{ 1,0},-GetRightDirection(){
           1��0}->{-1,0},m_FocalPoint=m_FocalPoint{0,0}+{-1,0}={-1,0}, �������ƶ���-1,0��λ��(��)��ʹ�����������ƶ�Ч��
                ������������ƶ�,delta{-1,0},-GetRightDirection(){-1��0}->{ 1,0},m_FocalPoint=m_FocalPoint{0,0}+{ 1,0}={1,
           0}, �������ƶ��� 1,0��λ��(��)��ʹ�����������ƶ�Ч��
        */
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        // ���Ӹ�������Ϊ���������Ϸ���(0,1,0)�ƶ���(0,1)��ʹ����������Ч��
        /*
                ������������ƶ�,delta{ 1,0},GetUpDirection(){ 1,0},m_FocalPoint=m_FocalPoint{0,0}+{ 1,0}={ 1,0}
                        �������ƶ��� 1,0(��)��λ����ʹ�����������ƶ�Ч��
                ������������ƶ�,delta{-1,0},GetUpDirection(){-1,0},m_FocalPoint=m_FocalPoint{0,0}+{-1,0}={-1,0}
                        �������ƶ���-1,0(��)��λ����ʹ�����������ƶ�Ч��
        */
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }
    void EditorCamera::MouseRotate(const glm::vec2 &delta) {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }
    // �޸����š�����m_Distance���ɣ���Ϊ��CalculatePosition��m_Distance�������������zλ�ã���ʹʵ������Ч��
    void EditorCamera::MouseZoom(float delta) {
        /*
                ��ǰ�������������Ǹ�
                m_Distance - �� ʹ��m_Distance��С��
           ���������λ�û῿�����㣨Ĭ��ԭ�㣩 m_Distance - ��
           ʹ��m_Distance���� ���������λ�û�Զ�뽹�㣨Ĭ��ԭ�㣩
        */
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f) {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }
    // �����ת��������y���������
    // ����vec3(0, 1, 0);���� vec3(0, -1, 0)�õ������ϻ������µ�����
    glm::vec3 EditorCamera::GetUpDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    // �����ת��������x���������
    // ����vec3(1, 0, 0)(��);���� vec3(-1, 0, 0)(��)�õ������һ������������
    glm::vec3 EditorCamera::GetRightDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    // �����ת��������z���������
    // ����vec3(0, 0, 1)(��);���� vec3(0, 0, -1)(ǰ)�õ����������ǰ������
    // ������z����˸��ţ�ʹ��vec3(0, 0, 1)(ǰ)��vec3(0, 0, -1)(��)
    glm::vec3 EditorCamera::GetForwardDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }
    glm::quat EditorCamera::GetOrientation() const { return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f)); }
    // ����������λ�á�m_Distance�������������zλ�ã���ʹʵ������Ч��
    glm::vec3 EditorCamera::CalculatePosition() const {
        // ��m_Pitch��m_Yaw = 0 �õ�vec3(0,0,-1)��������
        // temp = vec3(0,0,-1) * 10 = vec3(0,0,-10)��
        glm::vec3 temp = GetForwardDirection() * m_Distance;
        /*
                m_FocalPoint(0,0,0) - temp(0,0,-10) = (0,0,10)
                ��������ƶ�10���õ��������λ�ã�(0,0,10)
        */
        return m_FocalPoint - temp;
        // return m_FocalPoint - GetForwardDirection() * m_Distance;
    }
} // namespace Monado
