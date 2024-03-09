#include "monado.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "editor/editorLayer.h"

namespace Monado {
    class GameEngineEditor : public Application {
    public:
        // GameEngineEditor(ApplicationCommandLineArgs args)
        // 	: Application("GameEngine Editor", args)
        GameEngineEditor(const ApplicationSpecification &spec) : Application(spec) { PushLayer(new EditorLayer()); }
        ~GameEngineEditor() {}
    };

    // Monado::Application *Monado::CreateApplication() { return new GameEngineEditor(); }
    Application *CreateApplication(ApplicationCommandLineArgs args) {
        ApplicationSpecification spec;
        spec.Name = "GameEngine Editor";
        spec.CommandLineArgs = args;

        return new GameEngineEditor { spec };
    }
} // namespace Monado
