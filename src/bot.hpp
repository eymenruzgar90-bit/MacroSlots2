#pragma once
// ---------------------------------------------------------------------------
// bot.hpp - Slot yonetimi ve replay oynatma motoru
// ---------------------------------------------------------------------------

#include "gdr2.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace bot {

inline constexpr int kSlotCount = 9;

// Oynatilan input (main.cpp hook'u bunu GD'ye gonderir)
struct BotInput {
    bool down;
    int button;
    bool player2;
};

// --- makro klasoru ---
std::filesystem::path macrosDir();
void ensureMacrosDir();
std::vector<std::string> listMacros();   // makro dosya adlari (sirali)

// --- slot ayarlari (kayitli / menu icinde gecici) ---
std::array<std::string, kSlotCount> loadSlots();
void saveSlots(const std::array<std::string, kSlotCount>& slots);

// --- aktif slot ---
int activeSlot();                        // 0 = kapali (legit mod)
bool isActive();
void setActiveSlot(int slot, bool restartLevel = true);

// --- oynatma ---
std::optional<BotInput> poll(uint64_t frame);
void onReset(uint64_t currentFrame);
void onQuit();

// --- yardimci ---
std::string safeName(const std::string& name);  // GD fontlarinda gorunmeyen karakterleri temizler

} // namespace bot
