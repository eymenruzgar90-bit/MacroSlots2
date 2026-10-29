#pragma once
// ---------------------------------------------------------------------------
// gdr2.hpp - GDR2 (.gdr2) replay dosya bicimi okuyucusu
//
// Bicim maxnut/GDReplayFormat (gdr2) reposundaki resmi libGDR ile birebir
// uyumludur. Sadece okuma (load) islemi yapar.
// ---------------------------------------------------------------------------

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gdr2 {

// Tek bir input olayi
struct Input {
    uint64_t frame = 0;  // hangi tick'te (240 TPS varsayilir)
    uint8_t button = 1;  // 1 = zipla, 2 = sol, 3 = sag
    bool player2 = false;
    bool down = false;   // true = basildi, false = birakildi
};

// Bir replay'in tum verisi
struct Replay {
    std::string author;
    std::string description;
    float duration = 0.f;        // saniye
    int gameVersion = 0;
    double framerate = 240.0;    // kayit yapilan tick hizi
    int seed = 0;
    int coins = 0;
    bool ldm = false;
    bool platformer = false;
    std::string botName;
    int botVersion = 1;
    uint32_t levelId = 0;
    std::string levelName;
    std::vector<uint64_t> deaths;
    std::vector<Input> inputs;
};

namespace detail {

// LibGDR'nin ikili akisi: varint, buyuk ucuzak (big-endian) float/double,
// varint uzunluklu string
class Reader {
public:
    Reader(const uint8_t* data, size_t size) : m_data(data), m_size(size) {}

    bool ok() const { return m_ok; }
    size_t remaining() const { return m_size - m_pos; }

    bool readRaw(void* out, size_t n) {
        if (!m_ok || m_size - m_pos < n) { m_ok = false; return false; }
        std::memcpy(out, m_data + m_pos, n);
        m_pos += n;
        return true;
    }

    bool skip(size_t n) {
        if (!m_ok || m_size - m_pos < n) { m_ok = false; return false; }
        m_pos += n;
        return true;
    }

    bool readVarint(uint64_t& out) {
        out = 0;
        for (size_t i = 0; i <= 10; i++) {
            if (!m_ok || m_pos >= m_size) { m_ok = false; return false; }
            uint8_t b = m_data[m_pos++];
            out |= static_cast<uint64_t>(b & 0x7F) << (i * 7);
            if ((b & 0x80) == 0) return true;
        }
        m_ok = false;
        return false;
    }

    bool readString(std::string& out) {
        uint64_t len = 0;
        if (!readVarint(len)) return false;
        if (len > 0xFFFF || remaining() < len) { m_ok = false; return false; }
        out.assign(reinterpret_cast<const char*>(m_data + m_pos), static_cast<size_t>(len));
        m_pos += static_cast<size_t>(len);
        return true;
    }

    // float ve double dosyada buyuk ucuzak olarak saklaniyor
    bool readFloatBE(float& out) {
        uint8_t b[4]{};
        if (!readRaw(b, 4)) return false;
        uint32_t v = (static_cast<uint32_t>(b[0]) << 24) | (static_cast<uint32_t>(b[1]) << 16)
                   | (static_cast<uint32_t>(b[2]) << 8)  |  static_cast<uint32_t>(b[3]);
        std::memcpy(&out, &v, 4);
        return true;
    }

    bool readDoubleBE(double& out) {
        uint8_t b[8]{};
        if (!readRaw(b, 8)) return false;
        uint64_t v = 0;
        for (int i = 0; i < 8; i++) v = (v << 8) | static_cast<uint64_t>(b[i]);
        std::memcpy(&out, &v, 8);
        return true;
    }

    bool readBool(bool& out) {
        uint8_t b = 0;
        if (!readRaw(&b, 1)) return false;
        out = b != 0;
        return true;
    }

private:
    const uint8_t* m_data;
    size_t m_size;
    size_t m_pos = 0;
    bool m_ok = true;
};

} // namespace detail

// Dosyayi ham baytlara okur. Basarisizsa false.
inline bool loadFile(const std::filesystem::path& path, std::vector<uint8_t>& out) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;

    f.seekg(0, std::ios::end);
    std::streamoff sz = f.tellg();
    if (sz <= 0) return false;
    f.seekg(0, std::ios::beg);

    out.resize(static_cast<size_t>(sz));
    f.read(reinterpret_cast<char*>(out.data()), sz);
    return static_cast<bool>(f);
}

// Ham baytlari parse eder. Hata varsa Turkce mesaj, yoksa bos string doner.
inline std::string parse(const std::vector<uint8_t>& data, Replay& r) {
    detail::Reader s(data.data(), data.size());

    char magic[3]{};
    if (!s.readRaw(magic, 3) || std::string_view(magic, 3) != "GDR")
        return "Bu dosya GDR2 formatinda degil";

    uint64_t version = 0;
    std::string inputTag;
    if (!s.readVarint(version))            return "Bozuk dosya (surum)";
    if (!s.readString(inputTag))           return "Bozuk dosya (input tag)";
    if (!s.readString(r.author))           return "Bozuk dosya (yazar)";
    if (!s.readString(r.description))      return "Bozuk dosya (aciklama)";
    if (!s.readFloatBE(r.duration))        return "Bozuk dosya (sure)";

    uint64_t tmp = 0;
    if (!s.readVarint(tmp))                return "Bozuk dosya (oyun surumu)";
    r.gameVersion = static_cast<int>(tmp);

    if (!s.readDoubleBE(r.framerate))      return "Bozuk dosya (tick hizi)";
    if (!s.readVarint(tmp))                return "Bozuk dosya (seed)";
    r.seed = static_cast<int>(tmp);
    if (!s.readVarint(tmp))                return "Bozuk dosya (coin)";
    r.coins = static_cast<int>(tmp);
    if (!s.readBool(r.ldm))                return "Bozuk dosya (ldm)";
    if (!s.readBool(r.platformer))         return "Bozuk dosya (platformer)";

    if (!s.readString(r.botName))          return "Bozuk dosya (bot adi)";
    if (!s.readVarint(tmp))                return "Bozuk dosya (bot surumu)";
    r.botVersion = static_cast<int>(tmp);

    if (!s.readVarint(tmp))                return "Bozuk dosya (level id)";
    r.levelId = static_cast<uint32_t>(tmp);
    if (!s.readString(r.levelName))        return "Bozuk dosya (level adi)";

    // replay uzantisi (bizim icin onemsiz, atliyoruz)
    uint64_t extSize = 0;
    if (!s.readVarint(extSize))            return "Bozuk dosya (uzanti)";
    if (!s.skip(static_cast<size_t>(extSize))) return "Bozuk dosya (uzanti verisi)";

    // olum kareleri
    uint64_t deathCount = 0;
    if (!s.readVarint(deathCount))         return "Bozuk dosya (olum sayisi)";
    uint64_t acc = 0;
    for (uint64_t i = 0; i < deathCount; i++) {
        uint64_t d = 0;
        if (!s.readVarint(d))              return "Bozuk dosya (olum karesi)";
        acc += d;
        r.deaths.push_back(acc);
    }

    // input verisi
    uint64_t inputCount = 0;
    uint64_t p1Count = 0;
    if (!s.readVarint(inputCount))         return "Bozuk dosya (input sayisi)";
    if (!s.readVarint(p1Count))            return "Bozuk dosya (p1 input sayisi)";

    if (inputCount > 5'000'000) return "Dosya fazla buyuk gorunuyor";
    r.inputs.reserve(static_cast<size_t>(inputCount));

    uint64_t p = 0;
    while (s.remaining() > 0) {
        uint64_t packed = 0;
        if (!s.readVarint(packed))         return "Bozuk dosya (input)";

        Input in;
        if (r.platformer) {
            // [ ...delta | button(2 bit) | down ]
            p += packed >> 3;
            in.button = static_cast<uint8_t>((packed >> 1) & 3);
            in.down   = (packed & 1) != 0;
        } else {
            // [ ...delta | down ]  (buton hep ziplama)
            p += packed >> 1;
            in.button = 1;
            in.down   = (packed & 1) != 0;
        }
        in.frame   = p;
        in.player2 = (p1Count == 0);

        // input uzantisi varsa atla
        if (!inputTag.empty()) {
            uint64_t inExt = 0;
            if (!s.readVarint(inExt))       return "Bozuk dosya (input uzantisi)";
            if (!s.skip(static_cast<size_t>(inExt))) return "Bozuk dosya (input uzantisi verisi)";
        }

        r.inputs.push_back(in);

        // p1 inputlari bitince p2'ye gec, sayac sifirla
        if (p1Count > 0) {
            p1Count--;
            if (p1Count == 0) p = 0;
        }
    }

    // p1 ve p2 ayri saklandigi icin kare sirasina gore sirala
    std::stable_sort(
        r.inputs.begin(), r.inputs.end(),
        [](const Input& a, const Input& b) { return a.frame < b.frame; }
    );

    return "";
}

} // namespace gdr2
