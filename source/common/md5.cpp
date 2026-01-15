#include "md5.hpp"
#include <iomanip>
#include <sstream>
#include <cstdint>

constexpr uint32_t s[64] =
{
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

constexpr uint32_t K[64] =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

inline uint32_t leftRotate(uint32_t x, uint32_t c)
{
    return (x << c) | (x >> (32 - c));
}

inline uint32_t byteswap32(uint32_t x)
{
    return ((x >> 24) & 0x000000FF) |
        ((x >> 8) & 0x0000FF00) |
        ((x << 8) & 0x00FF0000) |
        ((x << 24) & 0xFF000000);
}

std::string md5(const std::string& input)
{
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;

    std::string message = input;
    uint64_t originalLength = message.length() * 8;

    message += static_cast<char>(0x80);
    while ((message.length() % 64) != 56) message += static_cast<char>(0x00);

    for (uint64_t i = 0; i < 8; ++i) {
        message += static_cast<char>((originalLength >> (i * 8)) & 0xFF);
    }

    for (size_t offset = 0; offset < message.length(); offset += 64) {
        uint32_t w[16];
        for (int i = 0; i < 16; ++i) {
            w[i] = (message[offset + i * 4 + 3] << 24) |
                (message[offset + i * 4 + 2] << 16) |
                (message[offset + i * 4 + 1] << 8) |
                (message[offset + i * 4]);
        }

        uint32_t a = a0;
        uint32_t b = b0;
        uint32_t c = c0;
        uint32_t d = d0;

        for (int i = 0; i < 64; ++i) {
            uint32_t f, g;
            if (i < 16) {
                f = (b & c) | (~b & d);
                g = i;
            }
            else if (i < 32) {
                f = (d & b) | (~d & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48) {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else {
                f = c ^ (b | ~d);
                g = (7 * i) % 16;
            }

            uint32_t temp = d;
            d = c;
            c = b;
            b = b + leftRotate(a + f + K[i] + w[g], s[i]);
            a = temp;
        }

        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
    }

    std::stringstream ss;
    ss << std::hex << std::setfill('0')
        << std::setw(8) << byteswap32(a0)
        << std::setw(8) << byteswap32(b0)
        << std::setw(8) << byteswap32(c0)
        << std::setw(8) << byteswap32(d0);

    return ss.str();
}