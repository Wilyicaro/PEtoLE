#include "MobSkinTextureProcessor.hpp"
#include <cstdlib>

Texture* MobSkinTextureProcessor::process(Texture& in) {
	uint32_t* newPixels = new uint32_t[width * height];
	std::memset(newPixels, 0, width * height * sizeof(uint32_t));

	int copyWidth = std::min(width, in.m_width);
	int copyHeight = std::min(height, in.m_height);

	for (int y = 0; y < copyHeight; ++y) {
		for (int x = 0; x < copyWidth; ++x) {
			newPixels[x + y * width] = in.m_pixels[x + y * in.m_width];
		}
	}

	this->pixels = reinterpret_cast<int*>(newPixels);

	setNoAlpha(0, 0, 32, 16);
	setForceAlpha(32, 0, 64, 32);
	setNoAlpha(0, 16, 64, 32);

	bool hasAlpha = false;

	for (int x = 32; x < 64; ++x) {
		for (int y = 0; y < 16; ++y) {
			int pix = pixels[x + y * width];
			if (((pix >> 24) & 0xFF) < 128) {
				hasAlpha = true;
				break;
			}
		}
		if (hasAlpha) break;
	}

	if (!hasAlpha) {
		for (int x = 32; x < 64; ++x) {
			for (int y = 0; y < 16; ++y) {
				int pix = pixels[x + y * width];
				if (((pix >> 24) & 0xFF) < 128) {
					hasAlpha = true;
					break;
				}
			}
			if (hasAlpha) break;
		}
	}

	return new Texture(width, height, newPixels, hasAlpha, 0);
}

void MobSkinTextureProcessor::setNoAlpha(int x0, int y0, int x1, int y1) {
	for (int x = x0; x < x1; ++x) {
		for (int y = y0; y < y1; ++y) {
			int index = x + y * width;
			pixels[index] |= 0xFF000000;
		}
	}
}

void MobSkinTextureProcessor::setForceAlpha(int x0, int y0, int x1, int y1) {
	if (!hasAlpha(x0, y0, x1, y1)) {
		for (int x = x0; x < x1; ++x) {
			for (int y = y0; y < y1; ++y) {
				int index = x + y * width;
				pixels[index] &= 0x00FFFFFF;
			}
		}
	}
}

bool MobSkinTextureProcessor::hasAlpha(int x0, int y0, int x1, int y1) {
	for (int x = x0; x < x1; ++x) {
		for (int y = y0; y < y1; ++y) {
			int pix = pixels[x + y * width];
			if (((pix >> 24) & 0xFF) < 128) {
				return true;
			}
		}
	}
	return false;
}
