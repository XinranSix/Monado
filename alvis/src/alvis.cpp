#include "monado/core/application.h"
#include "monado/entryPoint.h"
#include "editorLayer.h"
#include "monado/core/log.h"
#include "monado/renderer/rendererAPI.h"

class AlvisApplication : public Monado::Application {
public:
    AlvisApplication(const Monado::ApplicationProps &props) : Application(props) {}

    virtual void OnInit() override { PushLayer(new Monado::EditorLayer()); }
};

Monado::Application *Monado::CreateApplication(int argc, char **argv) {
    RendererAPI::SetAPI(RendererAPIType::Vulkan);
    return new AlvisApplication({ "Alvis", 1600, 900 });
}
