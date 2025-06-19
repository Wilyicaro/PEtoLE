#include "HttpTexture.hpp"

#include <cstring>
#include <algorithm>
#include <thirdparty/stb_image/include/stb_image.h>
#include "common/Utils.hpp"

HttpTexture::HttpTexture(const std::string& url, TextureProcessor* processor) {
    std::thread([this, url, processor]() {
        std::vector<unsigned char> imgData;
        if (!w_get(url, imgData)) {
            return;
        }

        int width, height, channels;
        unsigned char* pixels = stbi_load_from_memory(imgData.data(), imgData.size(), &width, &height, &channels, 4);
        if (!pixels) return;

        Texture tex(width, height, pixels, true, 0);
        if (processor)
            loadedImage = processor->process(tex);
        else
            loadedImage = &tex;

        stbi_image_free(pixels);
        isLoaded = true;
        }).detach();
}
