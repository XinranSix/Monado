#pragma once
#ifdef MND_PLATFORM_WINDOWS
    #include <iostream>
    #include "application.h"
    #include "monado/debug/instrumentor.h"
extern Monado::Application *Monado::CreateApplication();

int main(int argc, char **argv) {
    Monado::Log::Init();

    MND_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
    auto app = Monado::CreateApplication();
    MND_PROFILE_END_SESSION();

    MND_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
    app->Run();
    MND_PROFILE_END_SESSION();

    MND_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Shutdown.json");
    delete app;
    MND_PROFILE_END_SESSION();
}
#endif
