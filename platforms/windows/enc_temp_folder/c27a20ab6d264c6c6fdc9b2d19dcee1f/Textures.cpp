/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Textures.hpp"
#include "common/Utils.hpp"


int Textures::loadTexture(const std::string& name, bool bIsRequired, int mipmap, bool bindTexture)
{
	auto i = m_textures.find(name);
	if (i != m_textures.end())
		return i->second;
	

	Texture t = m_pPlatform->loadTexture(name, bIsRequired);

	if (!t.m_pixels && bIsRequired) {
		t.m_hasAlpha = 1;
		t.field_D = 0;
		t.m_width = 2;
		t.m_height = 2;
		t.m_pixels = new uint32_t[4];
		t.m_pixels[0] = 0xfff800f8;
		t.m_pixels[1] = 0xff000000;
		t.m_pixels[3] = 0xfff800f8;
		t.m_pixels[2] = 0xff000000;
	}

	if (t.m_pixels) {
		return assignTexture(name, t, mipmap, bindTexture);
	} else {
		return -1;
	}
}


void Textures::prepareTextureParams()
{
	if (m_bMipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		#ifndef ANDROID
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
		#endif
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (m_bBlur)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (m_bClamp)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

int Textures::assignTexture(const std::string& name, Texture& texture, int mipmap, bool bindTexture)
{
	GLuint textureID = 0;

	glGenTextures(1, &textureID);
	loadTexture(texture, textureID, mipmap, bindTexture);
	if (!name.empty()) {
		m_textures[name] = textureID;
	}
	m_textureData[textureID] = TextureData(textureID, texture);

	return textureID;
}

void Textures::loadTexture(Texture& texture, GLuint textureID, int mipmap, bool bindTexture)
{
	if (bindTexture) {
		if (textureID != m_currBoundTex)
		{
			glBindTexture(GL_TEXTURE_2D, textureID);
			m_currBoundTex = textureID;
		}
		prepareTextureParams();
	}

	GLuint internalFormat = GL_RGB;

	if (texture.m_hasAlpha)
		internalFormat = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA, texture.m_width, texture.m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.m_pixels);
}

int Textures::loadHttpTexture(const std::string& url, const std::string& backup)
{
	HttpTexture* texture = m_httpTextures[url];
	if (texture && texture->loadedImage && !texture->isLoaded) {
		if (texture->id < 0) {
			texture->id = assignTexture("", *texture->loadedImage);
		}
		else {
			loadTexture(*texture->loadedImage, texture->id);
		}
		texture->isLoaded = true;
	}

	if (texture && texture->id >= 0) {
		return texture->id;
	}
	else {
		return backup.empty() ? -1 : loadTexture(backup);
	}
}


HttpTexture* Textures::addHttpTexture(const std::string& url, TextureProcessor* processor)
{
	HttpTexture* texture = m_httpTextures[url];
	if (!texture) {
		m_httpTextures[url] = new HttpTexture(url, processor);
	}
	else {
		++texture->count;
	}

	return texture;
}

void Textures::removeHttpTexture(const std::string& url)
{
	HttpTexture* texture = m_httpTextures[url];
	if (texture) {
		--texture->count;
		if (!texture->count) {
			if (texture->id >= 0) {
				glDeleteTextures(1, &texture->id);
			}

			m_httpTextures.erase(url);
		}
	}
}

void Textures::clear()
{
	// note: Textures::clear() does not touch the dynamic textures vector

	for (auto& it : m_textures)
		glDeleteTextures(1, &it.second);

	for (auto& it : m_textureData)
		delete[] it.second.textureData.m_pixels;

	m_textures.clear();
	m_textureData.clear();
	m_currBoundTex = -1;
}

Textures::Textures(Options* pOptions, AppPlatform* pAppPlatform)
{
	m_bClamp = false;
	m_bBlur = false;
	m_bMipmap = false;

	m_pPlatform = pAppPlatform;
	m_pOptions = pOptions;
	m_currBoundTex = -1;
}

Textures::~Textures()
{
	clear();

	for (std::vector<DynamicTexture*>::iterator it = m_dynamicTextures.begin(); it != m_dynamicTextures.end(); it++)
	{
		DynamicTexture* pDynaTex = *it;
		SAFE_DELETE(pDynaTex);
	}

	m_dynamicTextures.clear();
}

void Textures::tick()
{
	// tick dynamic textures here
	for (std::vector<DynamicTexture*>::iterator it = m_dynamicTextures.begin(); it < m_dynamicTextures.end(); it++)
	{
		DynamicTexture* pDynaTex = *it;

		pDynaTex->bindTexture(this);
		pDynaTex->m_anaglyph3d = m_pOptions->m_bAnaglyphs;
		pDynaTex->tick();

		for (int x = 0; x < pDynaTex->m_textureSize; x++)
		{
			for (int y = 0; y < pDynaTex->m_textureSize; y++)
			{
				// texture is already bound so this is fine:
				glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					16 * (x + pDynaTex->m_textureIndex % 16),
					16 * (y + pDynaTex->m_textureIndex / 16),
					16, 16,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					pDynaTex->m_pixels
				);
			}
		}
	}
}

void Textures::bind(int bind)
{
	if (m_currBoundTex != bind && bind >= 0)
	{
		m_currBoundTex = bind;
		glBindTexture(GL_TEXTURE_2D, bind);
	}
}

int Textures::loadAndBindTexture(const std::string& name, int mipmap)
{
	int id = loadTexture(name, true, mipmap);

	if (m_currBoundTex != id)
	{
		m_currBoundTex = id;
		glBindTexture(GL_TEXTURE_2D, id);
		prepareTextureParams();
	}

	return id;
}

void Textures::addDynamicTexture(DynamicTexture* pTexture)
{
	m_dynamicTextures.push_back(pTexture);
	pTexture->tick();
}

Texture* Textures::getTemporaryTextureData(GLuint id)
{
	auto i = m_textureData.find(id);
	if (i == m_textureData.end())
		return nullptr;

	return &i->second.textureData;
}
