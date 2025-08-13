#include "DynamicTexture.hpp"
#include "world/tile/PortalTile.hpp"

PortalTexture::PortalTexture() : DynamicTexture(Tile::portal->m_TextureFrame)
{
    Random random(100L);

    for (int time = 0; time < 32; ++time) {
        for (int x = 0; x < 16; ++x) {
            for (int y = 0; y < 16; ++y) {
                float pow = 0.0F;

                for (int i = 0; i < 2; ++i) {
                    float xo = (float)(i * 8);
                    float yo = (float)(i * 8);
                    float xd = ((float)x - xo) / 16.0F * 2.0F;
                    float yd = ((float)y - yo) / 16.0F * 2.0F;
                    if (xd < -1.0F) {
                        xd += 2.0F;
                    }

                    if (xd >= 1.0F) {
                        xd -= 2.0F;
                    }

                    if (yd < -1.0F) {
                        yd += 2.0F;
                    }

                    if (yd >= 1.0F) {
                        yd -= 2.0F;
                    }

                    float dd = xd * xd + yd * yd;
                    float pp = Mth::atan2((double)yd, (double)xd) + ((float)time / 32.0F * M_PI * 2.0F - dd * 10.0F + (float)(i * 2)) * (float)(i * 2 - 1);
                    pp = (Mth::sin(pp) + 1.0F) / 2.0F;
                    pp /= dd + 1.0F;
                    pow += pp * 0.5F;
                }

                pow += random.nextFloat() * 0.1F;
                int r = (int)(pow * pow * 200.0F + 55.0F);
                int g = (int)(pow * pow * pow * pow * 255.0F);
                int b = (int)(pow * 100.0F + 155.0F);
                int a = (int)(pow * 100.0F + 155.0F);
                int i = y * 16 + x;
                m_frames[time][i * 4 + 0] = r;
                m_frames[time][i * 4 + 1] = g;
                m_frames[time][i * 4 + 2] = b;
                m_frames[time][i * 4 + 3] = a;
            }
        }
    }
}

void PortalTexture::tick()
{
    ++m_time;
    auto& source = m_frames[m_time & 31];

    for (int i = 0; i < 256; ++i) {
        int r = source[i * 4 + 0] & 255;
        int g = source[i * 4 + 1] & 255;
        int b = source[i * 4 + 2] & 255;
        int a = source[i * 4 + 3] & 255;
        if (m_bAnaglyph3d) {
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
