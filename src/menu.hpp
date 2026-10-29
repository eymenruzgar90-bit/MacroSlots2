#pragma once
// ---------------------------------------------------------------------------
// menu.hpp - Sag Shift ile acilan slot menusu
// ---------------------------------------------------------------------------

#include <array>
#include <functional>
#include <string>

#include <Geode/ui/Popup.hpp>

#include "bot.hpp"

// Ana menu: 1-9 slotlari, her slotun altinda + butonu, altta Apply
class SlotsPopup : public geode::Popup {
public:
    static SlotsPopup* create(std::function<void()> onClosed);

    bool init() override;
    void onClose(cocos2d::CCObject* sender) override;

private:
    std::function<void()> m_onClosed;
    std::array<std::string, bot::kSlotCount> m_slots;  // Apply'a kadar bekleyen atamalar
    cocos2d::CCNode* m_gridNode = nullptr;

    void buildGrid();
    void onPickFile(cocos2d::CCObject* sender);
    void onApply(cocos2d::CCObject* sender);
    void onOpenFolder(cocos2d::CCObject* sender);
};

namespace menu {
    // Sag Shift ile acilir / kapanir
    void toggle();
}
