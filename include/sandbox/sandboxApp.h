#pragma once

#include "monado/core/core.h"
#include "monado/core/application.h"
#include "exampleLayer.h"

// 注意, 这里的SandboxApp只可以创建对应的Layer, 然后加入到Application的
// m_LayerStack里, 具体调用的函数在Application.cpp的Run函数里
// Run函数是不会暴露给子类去override的
class SandboxApp : public Monado::Application {
public:
    SandboxApp();

    ~SandboxApp();

private:
    // == == == == == == == == == == == std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<Monado::VertexArray> m_VertexArray;
    std::shared_ptr<Monado::IndexBuffer> m_IndexBuffer;
    std::shared_ptr<Monado::Shader> m_Shader;
    std::shared_ptr<Monado::VertexBuffer> m_VertexBuffer;
};
