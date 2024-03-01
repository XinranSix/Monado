#pragma once
#include "rendererAPI.h"
#include "vertexArray.h"
#include <memory>
#include "glm/glm.hpp"
#include "monado/OrthographicCameraController.h"
#include "shader.h"
#include "texture.h"

namespace Monado {
    // 由于之前把RenderCommand拆分成了与平台无关的API, 这里的Renderer和Renderer2D都只需要调用
    // 抽象层的API即可
    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCameraController &camera);
        static void EndScene();

        static void DrawQuad(const glm::vec2 &position, float rotatedAngle, const glm::vec2 &size,
                             const glm::vec4 &color);
        static void DrawQuad(const glm::vec3 &position, float rotatedAngle, const glm::vec2 &size,
                             const glm::vec4 &color);

        static void DrawQuad(const glm::vec2 &position, float rotatedAngle, const glm::vec2 &size,
                             std::shared_ptr<Texture>, const glm::vec4 &);
        static void DrawQuad(const glm::vec3 &position, float rotatedAngle, const glm::vec2 &size,
                             std::shared_ptr<Texture>, const glm::vec4 &);
    };
} // namespace Monado
