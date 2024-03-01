#include "monado/math/random.h"

std::mt19937 Monado::Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Monado::Random::s_Distribution;
