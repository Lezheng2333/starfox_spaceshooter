#pragma once

// ============================================================
// 极简 PNG 写出（zlib deflate，无第三方依赖）
//
// SDL2_image 未安装，SDL 自带只能存 BMP；
// 这里用 zlib 直接写 PNG，省掉外部转换进程。
// ============================================================

#include <zlib.h>
#include <cstdio>
#include <string>
#include <vector>

namespace as_png {

inline void be32(std::vector<unsigned char>& v, unsigned int x) {
    v.push_back((unsigned char)((x >> 24) & 0xFF));
    v.push_back((unsigned char)((x >> 16) & 0xFF));
    v.push_back((unsigned char)((x >>  8) & 0xFF));
    v.push_back((unsigned char)( x        & 0xFF));
}

inline void putChunk(std::vector<unsigned char>& out, const char* type,
                     const std::vector<unsigned char>& data) {
    be32(out, (unsigned int)data.size());
    size_t start = out.size();
    out.insert(out.end(), type, type + 4);
    out.insert(out.end(), data.begin(), data.end());
    uLong c = crc32(0L, Z_NULL, 0);
    c = crc32(c, out.data() + start, (uInt)(out.size() - start));
    be32(out, (unsigned int)c);
}

// rgb: w*h*3 的 RGB 字节流
inline bool write(const std::string& path, const unsigned char* rgb, int w, int h) {
    if (w <= 0 || h <= 0) return false;

    std::vector<unsigned char> raw;
    raw.reserve((size_t)h * (1 + (size_t)w * 3));
    for (int y = 0; y < h; ++y) {
        raw.push_back(0);  // filter: none
        const unsigned char* row = rgb + (size_t)y * w * 3;
        raw.insert(raw.end(), row, row + (size_t)w * 3);
    }

    uLongf clen = compressBound((uLong)raw.size());
    std::vector<unsigned char> comp(clen);
    if (compress2(comp.data(), &clen, raw.data(), (uLong)raw.size(), 6) != Z_OK) return false;
    comp.resize((size_t)clen);

    std::vector<unsigned char> out;
    const unsigned char sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    out.insert(out.end(), sig, sig + 8);

    std::vector<unsigned char> ihdr;
    be32(ihdr, (unsigned int)w);
    be32(ihdr, (unsigned int)h);
    ihdr.push_back(8);   // bit depth
    ihdr.push_back(2);   // color type: truecolor RGB
    ihdr.push_back(0);   // compression
    ihdr.push_back(0);   // filter
    ihdr.push_back(0);   // interlace
    putChunk(out, "IHDR", ihdr);
    putChunk(out, "IDAT", comp);
    putChunk(out, "IEND", std::vector<unsigned char>());

    FILE* f = fopen(path.c_str(), "wb");
    if (!f) return false;
    size_t n = fwrite(out.data(), 1, out.size(), f);
    fclose(f);
    return n == out.size();
}

}  // namespace as_png
