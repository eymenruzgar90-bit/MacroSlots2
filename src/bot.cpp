// ---------------------------------------------------------------------------
// bot.cpp - Slot yonetimi ve replay oynatma motoru
// ---------------------------------------------------------------------------

#include "bot.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/PlayLayer.hpp>

#include <algorithm>
#include <cctype>

using namespace geode::prelude;

namespace {

int g_activeSlot = 0;                 // 0 = kapali
gdr2::Replay g_macro;
bool g_loaded = false;
size_t g_index = 0;                   // siradaki input

} // namespace

// -------------------------------------------------------------------------
// makro klasoru
// -------------------------------------------------------------------------
std::filesystem::path bot::macrosDir() {
    return Mod::get()->getSaveDir() / "macros";
}

void bot::ensureMacrosDir() {
    std::error_code ec;
    std::filesystem::create_directories(macrosDir(), ec);
}

std::vector<std::string> bot::listMacros() {
    ensureMacrosDir();

    auto endsWith = [](const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size()
            && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    };

    std::vector<std::string> out;
    std::error_code ec;

    for (auto& entry : std::filesystem::directory_iterator(macrosDir(), ec)) {
        if (!entry.is_regular_file(ec)) continue;

        std::string name = entry.path().filename().string();
        std::string lower;
        for (char c : name) lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        // .gdr2 tercih edilir, digerleri de acilir
        if (endsWith(lower, ".gdr2") || endsWith(lower, ".gdr") || endsWith(lower, ".gdr.json"))
            out.push_back(name);
    }

    std::sort(out.begin(), out.end());
    return out;
}

// -------------------------------------------------------------------------
// slot ayarlari
// -------------------------------------------------------------------------
std::array<std::string, bot::kSlotCount> bot::loadSlots() {
    std::array<std::string, kSlotCount> out{};
    auto* mod = Mod::get();
    for (int i = 0; i < kSlotCount; i++) {
        std::string key = "slot_" + std::to_string(i + 1);
        out[i] = mod->getSavedValue<std::string>(key);
    }
    return out;
}

void bot::saveSlots(const std::array<std::string, kSlotCount>& slots) {
    auto* mod = Mod::get();
    for (int i = 0; i < kSlotCount; i++) {
        std::string key = "slot_" + std::to_string(i + 1);
        mod->setSavedValue(key, slots[i]);
    }
}

// -------------------------------------------------------------------------
// aktif slot
// -------------------------------------------------------------------------
int bot::activeSlot() { return g_activeSlot; }

bool bot::isActive() { return g_activeSlot != 0 && g_loaded; }

void bot::setActiveSlot(int slot, bool restartLevel) {
    if (slot < 0 || slot > kSlotCount) return;

    // 0 = legit mod (bot kapali)
    if (slot == 0) {
        if (g_activeSlot != 0)
            log::info("[MacroSlots] Legit moda gecildi (bot kapali)");
        g_activeSlot = 0;
        g_loaded = false;
        g_index = 0;
        return;
    }

    auto slots = loadSlots();
    const std::string& file = slots[slot - 1];

    if (file.empty()) {
        log::warn("[MacroSlots] {} slotu bos", slot);
        return;
    }

    auto path = macrosDir() / file;

    std::vector<uint8_t> data;
    if (!gdr2::loadFile(path, data)) {
        log::error("[MacroSlots] Dosya acilamadi: {}", file);
        return;
    }

    gdr2::Replay replay;
    std::string err = gdr2::parse(data, replay);
    if (!err.empty()) {
        log::error("[MacroSlots] {} - {}", file, err);
        return;
    }
    if (replay.inputs.empty()) {
        log::error("[MacroSlots] {} - makro hic input icermiyor", file);
        return;
    }

    g_macro = std::move(replay);
    g_loaded = true;
    g_activeSlot = slot;
    g_index = 0;

    log::info(
        "[MacroSlots] Slot {} aktif: {} ({} input, {}s, bot: {})",
        slot, file, g_macro.inputs.size(), g_macro.duration, g_macro.botName
    );

    // Levelin ortasindaysak bastan baslat ki macro 0. kareden oynasin
    if (restartLevel) {
        if (auto* pl = PlayLayer::get()) {
            g_index = 0;
            pl->resetLevelFromStart();
        }
    }
}

// -------------------------------------------------------------------------
// oynatma
// -------------------------------------------------------------------------
std::optional<bot::BotInput> bot::poll(uint64_t frame) {
    if (!g_loaded) return std::nullopt;
    if (g_index >= g_macro.inputs.size()) return std::nullopt;

    const gdr2::Input& in = g_macro.inputs[g_index];
    if (in.frame > frame) return std::nullopt;

    g_index++;
    if (in.button == 0) return std::nullopt;  // platformer'da "yok" butonu

    return BotInput{ in.down, static_cast<int>(in.button), in.player2 };
}

void bot::onReset(uint64_t currentFrame) {
    g_index = 0;
    // Practice modda checkpoint'ten donulduysa gecmis karelerin inputlarini atla
    while (g_index < g_macro.inputs.size() && g_macro.inputs[g_index].frame < currentFrame)
        g_index++;
}

void bot::onQuit() {
    // Slot aktifligi korunur, sadece oynatma konumu sifirlanir
    g_index = 0;
}

// -------------------------------------------------------------------------
// yardimci
// -------------------------------------------------------------------------
std::string bot::safeName(const std::string& name) {
    std::string out;
    out.reserve(name.size());
    for (unsigned char c : name) {
        if (c >= 32 && c < 127) out += static_cast<char>(c);
        else out += '_';
    }
    return out;
}
