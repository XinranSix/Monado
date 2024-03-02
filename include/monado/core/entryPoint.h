#pragma once
#ifdef MND_PLATFORM_WINDOWS
    #include <iostream>
    #include "application.h"
extern Monado::Application *Monado::CreateApplication();
int main(int argc, char **argv) {
    Monado::Log::Init();
    // 原本代码输出日志
    Monado::Log::GetCoreLogger()->warn("原始代码输出日志");
    Monado::Log::GetClientLogger()->error("原始代码输出日志");

    // 用宏定义，输出日志
    MND_CORE_WARN("Initialized Log!");
    int a = 5;
    MND_INFO("Hello! Var={0}", a);

    auto app = Monado::CreateApplication();
    app->Run();
    delete app;
}
#endif
