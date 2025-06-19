#include "PickaxeItem.hpp"
#include "world/tile/Tile.hpp"

PickaxeItem::PickaxeItem(int id, Tier& tier) : DiggerItem(id, 1, tier, { Tile::cobblestone, Tile::stoneSlab, Tile::stoneSlabHalf, Tile::stone, Tile::mossStone, Tile::ironOre, Tile::ironBlock, Tile::coalOre, Tile::goldBlock, Tile::goldOre, Tile::diamondOre, Tile::diamondBlock, Tile::ice, Tile::netherrack, Tile::lapisOre, Tile::lapisBlock })
{
}

bool PickaxeItem::canDestroySpecial(const Tile* tile)
{
      if (tile == Tile::obsidian) {
         return m_tier.m_level == 3;
      } else if (tile != Tile::diamondBlock && tile != Tile::diamondOre) {
         if (tile != Tile::goldBlock && tile != Tile::goldOre) {
            if (tile != Tile::ironBlock && tile != Tile::ironOre) {
               if (tile != Tile::lapisBlock && tile != Tile::lapisOre) {
                  if (tile != Tile::redstoneOre && tile != Tile::redstoneOreLit) {
                     if (tile->m_pMaterial == Material::stone) {
                        return true;
                     } else {
                        return tile->m_pMaterial == Material::metal;
                     }
                  } else {
                     return m_tier.m_level >= 2;
                  }
               } else {
                  return m_tier.m_level >= 1;
               }
            } else {
               return m_tier.m_level >= 1;
            }
         } else {
            return m_tier.m_level >= 2;
         }
      } else {
         return m_tier.m_level >= 2;
      }
}
