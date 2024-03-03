#include "monado/renderer/renderCommand.h"
#include "platform/opengl/openGLRendererAPI.h"

namespace Monado {
    RendererAPI *RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}
