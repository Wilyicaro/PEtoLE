/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <unordered_map>

#include "thirdparty/GL/GL.hpp"
#include "client/options/Options.hpp"
#include "client/app/AppPlatform.hpp"
#include "DynamicTexture.hpp"
#include "HttpTexture.hpp"
#include "TextureProcessor.hpp"

class DynamicTexture; // in case we are being included from DynamicTexture. We don't store complete references to that

struct TextureData
{
	int glID;
	Texture textureData;

	TextureData()
	{
		glID = 0;
	}
	TextureData(int i, Texture& x)
	{
		glID = i;
		textureData = x;
	}
};

class Textures
{
public:
	int loadTexture(const std::string& name, bool bRequired = true, int mipmap = 0, bool bindTexture = true);
	int loadBlankTexture(const std::string& name, int width, int height, int mipmap = 0, bool bindTexture = true);
	int loadTexture(const std::string& name, uint32_t* pixels, int width, int height, int mipmap = 0, bool bindTexture = true);

	int loadAndBindTerrain()
	{
	
#ifndef USE_GLES
		if (hasMipmaps())
		{
			setMipmap(true);
		}
#endif
		int id = loadAndBindTexture(C_TERRAIN_NAME);
#ifndef USE_GLES
		if (hasMipmaps())
		{
			loadTexture(C_TERRAIN_MIPMAP2_NAME, true, 1, false);
			loadTexture(C_TERRAIN_MIPMAP3_NAME, true, 2, false);
			setMipmap(false);
		}
#endif
		return id;
	}

	void bind(int bind);
	int loadAndBindTexture(const std::string& name, int mipmap = 0);

	int loadHttpTexture(const std::string& url, const std::string& backup = "");
	HttpTexture* addHttpTexture(const std::string& url, TextureProcessor*);
	void removeHttpTexture(const std::string& url);

	void clear();
	void tick();
	void addDynamicTexture(DynamicTexture* pTexture);
	Texture* getTemporaryTextureData(GLuint id);

	// set smoothing for next texture to be loaded
	void setSmoothing(bool b)
	{
		m_bBlur = b;
	}

	// set smoothing for next texture to be loaded
	void setClampToEdge(bool b)
	{
		m_bClamp = b;
	}

	void setMipmap(bool b)
	{
		m_bMipmap = b;
	}

	bool hasTerrainMipmap() const
	{
		return m_bHasTerrainMipmap;
	}

	bool hasMipmaps() const
	{
		return hasTerrainMipmap() && m_pOptions->m_bMipmaps.get();
	}

	Textures(Options*, AppPlatform*);
	~Textures();

private:
	void prepareTextureParams();
	int assignTexture(const std::string& name, Texture& t, int mipmap = 0, bool bindTexture = true);
	void loadTexture(Texture& t, GLuint textureID, int mipmap = 0, bool bindTexture = true);

protected:
	std::unordered_map<std::string, GLuint> m_textures;
	Options* m_pOptions;
	AppPlatform* m_pPlatform;
	int m_currBoundTex;
	bool m_bClamp;
	bool m_bBlur;
	bool m_bMipmap;
	bool m_bHasTerrainMipmap;
	std::unordered_map<GLuint, TextureData> m_textureData;
	std::vector<DynamicTexture*> m_dynamicTextures;
	std::unordered_map<std::string, HttpTexture*> m_httpTextures;

	friend class StartMenuScreen;
};

