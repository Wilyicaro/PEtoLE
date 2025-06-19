#include "DynamicTexture.hpp"
#include "world/item/Item.hpp"
#include "client/app/Minecraft.hpp"

ClockTexture::ClockTexture(Minecraft* minecraft) : DynamicTexture(Item::clock->m_icon), m_pMinecraft(minecraft)
{
	m_data = new uint32_t[256];
    m_dialData = minecraft->m_pPlatform->loadTexture("misc/dial.png", true).m_pixels;
    m_textureId = 1;

    auto texture = minecraft->m_pPlatform->loadTexture("gui/items.png", true);

    for (int i = 0; i < 256; i++)
    {
        int x = i % 16;
        int y = i / 16;

        int texX = (m_textureIndex % 16) * 16 + x;
        int texY = (m_textureIndex / 16) * 16 + y;

        m_data[i] = texture.m_pixels[texY * texture.m_width + texX];
    }
}

ClockTexture::~ClockTexture()
{
	SAFE_DELETE(m_data);
}

void ClockTexture::tick()
{
    double rott = 0.0;
    if (m_pMinecraft->m_pLevel && m_pMinecraft->m_pLocalPlayer) {
        float time = m_pMinecraft->m_pLevel->getTimeOfDay(1.0F);
        rott = (double)(-time * M_PI * 2.0F);
        if (m_pMinecraft->m_pLevel->m_pDimension->m_bFoggy) {
            rott = Mth::random() * M_PI * 2.0;
        }
    }

    double rotd;
    for (rotd = rott - m_rot; rotd < -M_PI; rotd += 2 * M_PI) {
    }

    while (rotd >= M_PI) {
        rotd -= 2 * M_PI;
    }

    if (rotd < -1.0) {
        rotd = -1.0;
    }

    if (rotd > 1.0) {
        rotd = 1.0;
    }

    m_rota += rotd * 0.1;
    m_rota *= 0.8;
    m_rot += m_rota;
    double sin = Mth::sin(m_rot);
    double cos = Mth::cos(m_rot);

    for (int i = 0; i < 256; ++i) {
        int a = m_data[i] >> 24 & 255;
        int r = m_data[i] >> 16 & 255;
        int g = m_data[i] >> 8 & 255;
        int b = m_data[i] >> 0 & 255;
        //@NOTE Temporary fix
        std::swap(r, b);
        if (r == b && g == 0 && b > 0) {
            double xo = -((double)(i % 16) / 15.0 - 0.5);
            double yo = (double)(i / 16) / 15.0 - 0.5;
            int br = r;
            int x = (int)((xo * cos + yo * sin + 0.5) * 16.0);
            int y = (int)((yo * cos - xo * sin + 0.5) * 16.0);
            int j = (x & 15) + (y & 15) * 16;
            a = m_dialData[j] >> 24 & 255;
            r = (m_dialData[j] >> 16 & 255) * r / 255;
            g = (m_dialData[j] >> 8 & 255) * br / 255;
            b = (m_dialData[j] >> 0 & 255) * br / 255;
            std::swap(r, b);
        }

        if (m_anaglyph3d) {
            int rr = (r * 30 + g * 59 + b * 11) / 100;
            int gg = (r * 30 + g * 70) / 100;
            int bb = (r * 30 + b * 70) / 100;
            r = rr;
            g = gg;
            b = bb;
        }

        m_pixels[i * 4 + 0] = r;
        m_pixels[i * 4 + 1] = g;
        m_pixels[i * 4 + 2] = b;
        m_pixels[i * 4 + 3] = a;
    }
}
