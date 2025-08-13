#include "PaintingType.hpp"

std::vector<PaintingType> PaintingType::types = {
    {"Kebab", 16, 16, 0, 0},
    {"Aztec", 16, 16, 16, 0},
    {"Alban", 16, 16, 32, 0},
    {"Aztec2", 16, 16, 48, 0},
    {"Bomb", 16, 16, 64, 0},
    {"Plant", 16, 16, 80, 0},
    {"Wasteland", 16, 16, 96, 0},
    {"Pool", 32, 16, 0, 32},
    {"Courbet", 32, 16, 32, 32},
    {"Sea", 32, 16, 64, 32},
    {"Sunset", 32, 16, 96, 32},
    {"Creebet", 32, 16, 128, 32},
    {"Wanderer", 16, 32, 0, 64},
    {"Graham", 16, 32, 16, 64},
    {"Match", 32, 32, 0, 128},
    {"Bust", 32, 32, 32, 128},
    {"Stage", 32, 32, 64, 128},
    {"Void", 32, 32, 96, 128},
    {"SkullAndRoses", 32, 32, 128, 128},
    {"Fighters", 64, 32, 0, 96},
    {"Pointer", 64, 64, 0, 192},
    {"Pigscene", 64, 64, 64, 192},
    {"BurningSkull", 64, 64, 128, 192},
    {"Skeleton", 64, 48, 192, 64},
    {"DonkeyKong", 64, 48, 192, 112}
};

PaintingType::PaintingType(const std::string& name, int width, int height, int uo, int vo) : m_name(name), m_width(width), m_height(height), m_uo(uo), m_vo(vo)
{
}