#include "LightningBoltRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "world/level/Level.hpp"
#include "world/entity/LightningBolt.hpp"


LightningBoltRenderer::LightningBoltRenderer()
{
}

void LightningBoltRenderer::render(Entity* pEntity, const Vec3& pos, float rot, float a)
{
	Tesselator& var10 = Tesselator::instance;
	LightningBolt* lightning = (LightningBolt*)pEntity;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	real var11[8]{};
	real var12[8]{};
	real var13 = 0.0;
	real var15 = 0.0;
	Random var17(lightning->m_seed);

	for (int var18 = 7; var18 >= 0; --var18) {
		var11[var18] = var13;
		var12[var18] = var15;
		var13 += (real)(var17.nextInt(11) - 5);
		var15 += (real)(var17.nextInt(11) - 5);
	}

	for (int var45 = 0; var45 < 4; ++var45) {
		var17.setSeed(lightning->m_seed);

		for (int var19 = 0; var19 < 3; ++var19) {
			int var20 = 7;
			int var21 = 0;
			if (var19 > 0) {
				var20 = 7 - var19;
			}

			if (var19 > 0) {
				var21 = var20 - 2;
			}

			real var22 = var11[var20] - var13;
			real var24 = var12[var20] - var15;

			for (int var26 = var20; var26 >= var21; --var26) {
				real var27 = var22;
				real var29 = var24;
				if (var19 == 0) {
					var22 += (var17.nextInt(11) - 5);
					var24 += (var17.nextInt(11) - 5);
				}
				else {
					var22 += (var17.nextInt(31) - 15);
					var24 += (var17.nextInt(31) - 15);
				}

				var10.begin(5);
				float var31 = 0.5F;
				var10.color(0.9F * var31, 0.9F * var31, 1.0F * var31, 0.3F);
				real var32 = 0.1 + var45 * 0.2;
				if (var19 == 0) {
					var32 *= var26 * 0.1 + 1.0;
				}

				real var34 = 0.1 + var45 * 0.2;
				if (var19 == 0) {
					var34 *= (var26 - 1) * 0.1 + 1.0;
				}

				for (int var36 = 0; var36 < 5; ++var36) {
					float var37 = pos.x + 0.5 - var32;
					float var39 = pos.z + 0.5 - var32;
					if (var36 == 1 || var36 == 2) {
						var37 += var32 * 2.0;
					}

					if (var36 == 2 || var36 == 3) {
						var39 += var32 * 2.0;
					}

					float var41 = pos.x + 0.5 - var34;
					float var43 = pos.z + 0.5 - var34;
					if (var36 == 1 || var36 == 2) {
						var41 += var34 * 2.0;
					}

					if (var36 == 2 || var36 == 3) {
						var43 += var34 * 2.0;
					}

					var10.vertex(var41 + var22, pos.y + (var26 * 16), var43 + var24);
					var10.vertex(var37 + var27, pos.y + ((var26 + 1) * 16), var39 + var29);
				}

				var10.draw();
			}
		}
	}

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}