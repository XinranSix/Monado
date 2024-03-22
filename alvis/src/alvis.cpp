#include "monado/core/application.h"
#include "monado/entryPoint.h"
#include "editorLayer.h"
#include "monado/core/log.h"

class AlvisApplication : public Monado::Application {
public:
    AlvisApplication(const Monado::ApplicationProps &props) : Application(props) {}

    virtual void OnInit() override { PushLayer(new Monado::EditorLayer()); }
};

Monado::Application *Monado::CreateApplication() { return new AlvisApplication({ "Alvis", 1600, 900 }); }
