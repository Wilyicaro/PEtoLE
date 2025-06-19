#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <cstdint>
#include "Texture.hpp"
#include "TextureProcessor.hpp"

class HttpTexture {
public:
    Texture* loadedImage = nullptr;
    int count = 1;
    GLuint id = -1;
    std::atomic<bool> isLoaded = false;

    HttpTexture(const std::string& url, TextureProcessor* processor = nullptr);
};
