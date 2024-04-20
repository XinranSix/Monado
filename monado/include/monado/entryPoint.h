#pragma once

#ifdef MND_PLATFORM_WINDOWS

extern Monado::Application *Monado::CreateApplication(int argc, char **argv);
bool g_ApplicationRunning = true;

int main(int argc, char **argv) {
    Monado::InitializeCore();
    Monado::Application *app = Monado::CreateApplication(argc, argv);
    MND_CORE_ASSERT(app, "Client Application is null!");
    app->Run();
    delete app;
    Monado::ShutdownCore();
}
#endif
