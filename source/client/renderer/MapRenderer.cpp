#include "MapRenderer.hpp"
#include "Tesselator.hpp"

MapRenderer::MapRenderer(Font* font, Options* options) : m_pFont(font), m_pOptions(options), m_pixels{}
{
}

void MapRenderer::render(const std::shared_ptr<Player>&, Textures* textures, const std::shared_ptr<MapItemSavedData>& map)
{
	for (int var4 = 0; var4 < 16384; ++var4) {
		uint8_t var5 = map->m_colors[var4];
		if (var5 / 4 == 0)
		{
			m_pixels[var4] = (var4 + var4 / 128 & 1) * 8 + 16 << 24;
		}
		else
		{
			int var6 = MapColor::mapColors[var5 / 4]->m_color;
			int var7 = var5 & 3;
			short var8 = 220;
			if (var7 == 2) {
				var8 = 255;
			}

			if (var7 == 0) {
				var8 = 180;
			}

			int red = (var6 >> 16 & 255) * var8 / 255;
			int green = (var6 >> 8 & 255) * var8 / 255;
			int blue = (var6 & 255) * var8 / 255;
			if (m_pOptions->m_bAnaglyphs.get())
			{
				int var12 = (red * 30 + green * 59 + blue * 11) / 100;
				int var13 = (red * 30 + green * 70) / 100;
				int var14 = (red * 30 + blue * 70) / 100;
				red = var12;
				green = var13;
				blue = var14;
			}

			m_pixels[var4] = -16777216 | blue << 16 | green << 8 | red;
		}
	}

	int texture = textures->loadTexture("mapcontent", m_pixels, 128, 128);
	Tesselator& t = Tesselator::instance;
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	t.begin();
	t.vertexUV(0.0f, 128.0f, -0.01F, 0.0, 1.0);
	t.vertexUV(128.0f, 128.0f, -0.01F, 1.0, 1.0);
	t.vertexUV(128.0f, 0.0f, -0.01F, 1.0, 0.0);
	t.vertexUV(0.0f, 0.0f, -0.01F, 0.0, 0.0);
	t.draw();
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	textures->loadAndBindTexture("misc/mapicons.png");

	for (auto& decoration : map->m_icons)
	{
		glPushMatrix();
		glTranslatef(decoration.m_x / 2.0F + 64.0F, decoration.m_y / 2.0F + 64.0F, -0.02F);
		glRotatef((decoration.m_rot * 360) / 16.0F, 0.0F, 0.0F, 1.0F);
		glScalef(4.0F, 4.0F, 3.0F);
		glTranslatef(-(2.0F / 16.0F), 2.0F / 16.0F, 0.0F);
		float var21 = float(decoration.m_image % 4 + 0) / 4.0F;
		float var22 = float(decoration.m_image / 4 + 0) / 4.0F;
		float var23 = float(decoration.m_image % 4 + 1) / 4.0F;
		float var24 = float(decoration.m_image / 4 + 1) / 4.0F;
		t.begin();
		t.vertexUV(-1.0, 1.0, 0.0, var21, var22);
		t.vertexUV(1.0, 1.0, 0.0, var23, var22);
		t.vertexUV(1.0, -1.0, 0.0, var23, var24);
		t.vertexUV(-1.0, -1.0, 0.0, var21, var24);
		t.draw();
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0.0F, 0.0F, -0.04F);
	glScalef(1.0F, 1.0F, 1.0F);
	m_pFont->draw(map->m_id, 0, 0, -16777216);
	glPopMatrix();
}
