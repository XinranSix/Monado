#include "monado/renderer/textureLoader.h"
// #include "stb_image.cpp"

namespace Monado {
    unsigned char *TextureLoader::Load(const char *path, bool inverse, int &width, int &height, int &nrChannels) {
        return nullptr;
        // stbi_set_flip_vertically_on_load(inverse);
        // unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        // return data;
    }

    void TextureLoader::Free(void *data) {
        // stbi_image_free(data);
    }
} // namespace Monado
