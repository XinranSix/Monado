#include "monado/core/application.h"
#include "monado/core/log.h"
#include "monado/event/event.h"
#include "monado/event/applicationEvent.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <cmath>
#include <iostream>
#include <memory>

namespace Monado {

    Application::Application() {}

    Application::~Application() {}

    void Application::Run() {
        WindowResizeEvent e { 1280, 720 };            // 使用自定义事件
        if (e.IsInCategory(EventCategoryApplication)) // 判断是否对应的分类
        {
            MND_TRACE(e); // 输出事件
        }
        if (e.IsInCategory(EventCategoryInput)) {
            MND_TRACE(e);
        }
        while (true)
            ;
    }

} // namespace Monado
