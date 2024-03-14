#include "monado/scripting/scriptGlue.h"
#include "monado/scripting/scriptEngine.h"
#include "monado/core/uuid.h"
#include "monado/scene/scene.h"
#include "monado/core/input.h"
#include "monado/core/keyCodes.h"
#include "monado/core/log.h"

#include "glm/glm.hpp"
#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include "mono/utils/mono-publib.h"
#include "mono/metadata/loader.h"

#include "box2d/b2_body.h"

namespace Monado {

#define MND_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Monado.InternalCalls::" #Name, Name)

    static void NativeLog(MonoString *string, int parameter) {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        std::cout << str << ", " << parameter << std::endl;
    }

    static void NativeLog_Vector(glm::vec3 *parameter, glm::vec3 *outResult) {
        // MND_CORE_WARN("Value: {0}", *parameter);
        *outResult = glm::normalize(*parameter);
    }

    static float NativeLog_VectorDot(glm::vec3 *parameter) {
        // MND_CORE_WARN("Value: {0}", *parameter);
        return glm::dot(*parameter, *parameter);
    }

    void ScriptGlue::RegisterFunctions() {
        MND_ADD_INTERNAL_CALL(NativeLog);
        MND_ADD_INTERNAL_CALL(NativeLog_Vector);
        MND_ADD_INTERNAL_CALL(NativeLog_VectorDot);
    }

} // namespace Monado
