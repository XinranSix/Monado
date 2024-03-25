#include "monado/renderer/sceneEnvironment.h"
#include "monado/renderer/sceneRenderer.h"

namespace Monado {

    Environment Environment::Load(const std::string &filepath) {
        auto [radiance, irradiance] = SceneRenderer::CreateEnvironmentMap(filepath);
        return { filepath, radiance, irradiance };
    }

} // namespace Monado
