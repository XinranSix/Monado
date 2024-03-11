#include "monado.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "editor/editorLayer.h"

namespace Monado {
    class GameEngineEditor : public Application {
    public:
        GameEngineEditor(ApplicationCommandLineArgs args) : Application("GameEngineEditor", args) {
            PushLayer(new EditorLayer());
        }

        ~GameEngineEditor() {}
    };

    Application *CreateApplication(ApplicationCommandLineArgs args) { return new GameEngineEditor(args); }

} // namespace Monado
