#pragma once

#include "glm/fwd.hpp"
#include "monado/core/layer.h"
#include "monado/OrthographicCameraController.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/texture.h"
#include "monado/debug/instrumentor.h"

#include <memory>
#include <vector>

class ExampleLayer : public Monado::Layer {
public:
    ExampleLayer();
    ~ExampleLayer();

private:
    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Monado::Event &e) override;
    void OnUpdate(const Monado::Timestep &step) override;
    void OnImGuiRender() override;

    // 相机、绘制的VertexArray、对应的Shader和Camera都属于单独的一个Layer
    // 这里的Camera是属于Layer的, 没有存在Application或Sandbox类里
private:
    Monado::OrthographicCameraController m_Camera;
    std::shared_ptr<Monado::Shader> m_Shader;
    std::shared_ptr<Monado::Shader> m_BlueShader;

    std::shared_ptr<Monado::VertexArray> m_VertexArray;
    std::shared_ptr<Monado::VertexArray> m_QuadVertexArray;

    std::shared_ptr<Monado::Texture2D> m_Texture1;

    // std::vector<Monado::ProfileResult> m_ProfileResults;
    Monado::ProfileResult m_ProfileResult;
    glm::vec4 m_FlatColor;
};
