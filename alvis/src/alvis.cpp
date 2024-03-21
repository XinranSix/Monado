#include "monado/core/application.h"
#include "monado/entryPoint.h"
#include "editorLayer.h"
#include "monado/core/log.h"

class AlvisnutApplication : public Monado::Application
{
public:
	AlvisnutApplication(const Monado::ApplicationProps& props)
		: Application(props)
	{
	}

	virtual void OnInit() override
	{
		PushLayer(new Monado::EditorLayer());
	}
};

Monado::Application* Monado::CreateApplication()
{
	return new AlvisnutApplication({"Monado", 1600, 900});
}
