// ---------------------------------------------------------------------------
// main.cpp - Hook'lar: klavye (Sag Shift / 1-9 / 0) ve oynatma motoru
// ---------------------------------------------------------------------------

#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

#include "bot.hpp"
#include "menu.hpp"

using namespace geode::prelude;

// ===========================================================================
// Oynatma: her physics tick'inde (240 TPS) siradaki inputlari GD'ye gonder
// ===========================================================================
class $modify(MacroBGL, GJBaseGameLayer) {

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);

        if (!bot::isActive() || !m_player1) return;

        // Eclipse ve diger botlarla ayni frame sayaci:
        // level basinda 0, her tick'ta 1 artar
        uint64_t frame = m_gameState.m_currentProgress;

        std::optional<bot::BotInput> in;
        while ((in = bot::poll(frame))) {
            // 3. parametre "1. oyuncu mu" oldugu icin player2 ters cevrilir
            GJBaseGameLayer::handleButton(in->down, in->button, !in->player2);
        }
    }
};

// ===========================================================================
// Level sifirlandiginda oynatma konumunu da sifirla
// ===========================================================================
class $modify(MacroPL, PlayLayer) {

    void resetLevel() {
        PlayLayer::resetLevel();
        // Practice modda checkpoint'ten donulduysa dogru kareden devam et
        bot::onReset(m_gameState.m_currentProgress);
    }

    void onQuit() {
        bot::onQuit();
        PlayLayer::onQuit();
    }
};

// ===========================================================================
// Klavye: Sag Shift menu, 1-9 slot, 0 legit mod
// ===========================================================================
class $modify(MacroKeys, cocos2d::CCKeyboardDispatcher) {

    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double p1) {
        if (isKeyDown && !isKeyRepeat) {
            using KK = cocos2d::enumKeyCodes;

            if (key == KK::KEY_RightShift) {
                menu::toggle();
            }
            else if (key >= KK::KEY_One && key <= KK::KEY_Nine) {
                int slot = static_cast<int>(key) - static_cast<int>(KK::KEY_One) + 1;
                bot::setActiveSlot(slot);
            }
            else if (key == KK::KEY_Zero) {
                bot::setActiveSlot(0);
            }
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, p1);
    }
};

$execute {
    bot::ensureMacrosDir();
    log::info("[MacroSlots] Yuklendi! Menu icin Sag Shift, slot icin 1-9, kapatmak icin 0.");
}
