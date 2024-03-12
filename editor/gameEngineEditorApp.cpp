#include "monado.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "editor/editorLayer.h"

namespace Monado {

    class GameEngineEditor : public Application {
    public:
        GameEngineEditor(const ApplicationSpecification &spec) : Application(spec) { PushLayer(new EditorLayer()); }
    };

    Application *CreateApplication(ApplicationCommandLineArgs args) {
        ApplicationSpecification spec;
        spec.Name = "GameEngineEditor";
        spec.CommandLineArgs = args;

        return new GameEngineEditor(spec);
    }

} // namespace Monado
