#pragma once

#ifdef MND_PLATFORM_WINDOWS

extern Monado::Application *Monado::CreateApplication();

int main(int argc, char **argv) {
    Monado::InitializeCore();
    Monado::Application *app = Monado::CreateApplication();
    MND_CORE_ASSERT(app, "Client Application is null!");
    app->Run();
    delete app;
    Monado::ShutdownCore();
}

#endif
