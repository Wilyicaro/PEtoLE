#pragma once

#include "TextureProcessor.hpp"
#include "Texture.hpp"
#include <cstdint>
#include <cstring>

class MobSkinTextureProcessor : public TextureProcessor {
private:
	int* pixels;
	int width = 64;
	int height = 32;

public:
	MobSkinTextureProcessor() = default;

	Texture* process(Texture& in) override;

private:
	void setNoAlpha(int x0, int y0, int x1, int y1);
	void setForceAlpha(int x0, int y0, int x1, int y1);
	bool hasAlpha(int x0, int y0, int x1, int y1);
};
