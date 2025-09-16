#include "ItemSpriteRenderer.hpp"
#include "EntityRenderDispatcher.hpp"

ItemSpriteRenderer::ItemSpriteRenderer(int sprite, float scale) : m_sprite(sprite), m_scale(scale)
{
}

void ItemSpriteRenderer::render(Entity* pEntity, const Vec3& pos, float a, float b)
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glEnable(GL_RESCALE_NORMAL);
	glScalef(m_scale, m_scale, m_scale);
	bindTexture(C_ITEMS_NAME);

	glRotatef(180.0f - m_pDispatcher->m_rot.y, 0.0f, 1.0f, 0.0f);
	glRotatef(-m_pDispatcher->m_rot.x, 1.0f, 0.0f, 0.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.normal(0.0f, 1.0f, 0.0f);
	t.vertexUV(-0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.vertexUV(-0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.draw();

	glDisable(GL_RESCALE_NORMAL);
	glPopMatrix();
}
