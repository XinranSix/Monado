#include "monado/core/math/noise.h"

#include "FastNoise.h"

namespace Monado {

    static FastNoise s_FastNoise;

    /**
     * @brief 根据 x 和 y 的值返回 PerlinNoise 的值
     *
     * @param x
     * @param y
     * @return float PerlinNoise 的值，范围为 [-1, 1]
     */
    float Noise::PerlinNoise(float x, float y) {
        s_FastNoise.SetNoiseType(FastNoise::Perlin);
        float result = s_FastNoise.GetNoise(x, y); // This returns a value between -1 and 1
        return result;
    }

} // namespace Monado
