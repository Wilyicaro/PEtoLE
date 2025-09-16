#include "PaintingRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "world/level/Level.hpp"

PaintingRenderer::PaintingRenderer()
{
}

void PaintingRenderer::render(Entity* pEntity, const Vec3& pos, float rot, float b)
{
    m_random.setSeed(187L);
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot, 0.0F, 1.0F, 0.0F);
    glEnable(32826);
    bindTexture("art/kz.png");
    Painting* painting = (Painting*)pEntity;
    PaintingType& motive = painting->getPaintingType();
    float s = 0.0625F;
    glScalef(s, s, s);
    renderPainting(painting, motive.m_width, motive.m_height, motive.m_uo, motive.m_vo);
    glDisable(32826);
    glPopMatrix();
}

void PaintingRenderer::renderPainting(Painting* painting, int w, int h, int uo, int vo)
{
    float xx0 = (float)(-w) / 2.0F;
    float yy0 = (float)(-h) / 2.0F;
    float z0 = -0.5F;
    float z1 = 0.5F;

    for (int xs = 0; xs < w / 16; ++xs) {
        for (int ys = 0; ys < h / 16; ++ys) {
            float x0 = xx0 + (float)((xs + 1) * 16);
            float x1 = xx0 + (float)(xs * 16);
            float y0 = yy0 + (float)((ys + 1) * 16);
            float y1 = yy0 + (float)(ys * 16);
            setBrightness(painting, (x0 + x1) / 2.0F, (y0 + y1) / 2.0F);
            float fu0 = (float)(uo + w - xs * 16) / 256.0F;
            float fu1 = (float)(uo + w - (xs + 1) * 16) / 256.0F;
            float fv0 = (float)(vo + h - ys * 16) / 256.0F;
            float fv1 = (float)(vo + h - (ys + 1) * 16) / 256.0F;
            float bu0 = 0.75F;
            float bu1 = 0.8125F;
            float bv0 = 0.0F;
            float bv1 = 0.0625F;
            float uu0 = 0.75F;
            float uu1 = 0.8125F;
            float uv0 = 0.001953125F;
            float uv1 = 0.001953125F;
            float su0 = 0.7519531F;
            float su1 = 0.7519531F;
            float sv0 = 0.0F;
            float sv1 = 0.0625F;
            Tesselator& t = Tesselator::instance;
            t.begin();
            t.normal(0.0F, 0.0F, -1.0F);
            t.vertexUV(x0, y1, z0, fu1, fv0);
            t.vertexUV(x1, y1, z0, fu0, fv0);
            t.vertexUV(x1, y0, z0, fu0, fv1);
            t.vertexUV(x0, y0, z0, fu1, fv1);
            t.normal(0.0F, 0.0F, 1.0F);
            t.vertexUV(x0, y0, z1, bu0, bv0);
            t.vertexUV(x1, y0, z1, bu1, bv0);
            t.vertexUV(x1, y1, z1, bu1, bv1);
            t.vertexUV(x0, y1, z1, bu0, bv1);
            t.normal(0.0F, -1.0F, 0.0F);
            t.vertexUV(x0, y0, z0, uu0, uv0);
            t.vertexUV(x1, y0, z0, uu1, uv0);
            t.vertexUV(x1, y0, z1, uu1, uv1);
            t.vertexUV(x0, y0, z1, uu0, uv1);
            t.normal(0.0F, 1.0F, 0.0F);
            t.vertexUV(x0, y1, z1, uu0, uv0);
            t.vertexUV(x1, y1, z1, uu1, uv0);
            t.vertexUV(x1, y1, z0, uu1, uv1);
            t.vertexUV(x0, y1, z0, uu0, uv1);
            t.normal(-1.0F, 0.0F, 0.0F);
            t.vertexUV(x0, y0, z1, su1, sv0);
            t.vertexUV(x0, y1, z1, su1, sv1);
            t.vertexUV(x0, y1, z0, su0, sv1);
            t.vertexUV(x0, y0, z0, su0, sv0);
            t.normal(1.0F, 0.0F, 0.0F);
            t.vertexUV(x1, y0, z0, su1, sv0);
            t.vertexUV(x1, y1, z0, su1, sv1);
            t.vertexUV(x1, y1, z1, su0, sv1);
            t.vertexUV(x1, y0, z1, su0, sv0);
            t.draw();
        }
    }
}

void PaintingRenderer::setBrightness(Painting* painting, float ss, float ya)
{
    TilePos tp(painting->m_pos);
    tp.y += ya / 16.0F;
    if (painting->m_dir == 0)
        tp.x = Mth::floor(painting->m_pos.x + (ss / 16.0F));

    if (painting->m_dir == 1)
        tp.z = Mth::floor(painting->m_pos.z - (ss / 16.0F));

    if (painting->m_dir == 2)
        tp.x = Mth::floor(painting->m_pos.x - (ss / 16.0F));
    
    if (painting->m_dir == 3)
        tp.z = Mth::floor(painting->m_pos.z + (ss / 16.0F));

    float br = EntityRenderDispatcher::getInstance()->m_pLevel->getBrightness(tp);
    glColor4f(br, br, br, 1.0f);
}
