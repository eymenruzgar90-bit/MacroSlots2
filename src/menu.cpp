// ---------------------------------------------------------------------------
// menu.cpp - Slot menusu ve makro dosya secici
// ---------------------------------------------------------------------------

#include "menu.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/Notification.hpp>

#ifdef GEODE_IS_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <shellapi.h>
#endif

using namespace geode::prelude;

namespace {

std::string truncate(const std::string& s, size_t n) {
    if (s.size() <= n) return s;
    return s.substr(0, n - 1) + ".";
}

} // namespace

// ===========================================================================
// Makro dosya secici
// ===========================================================================
class FilePickerPopup : public geode::Popup {
public:
    std::function<void(const std::string&)> onPick;
    int slot = 0;

    static FilePickerPopup* create(int slotNum, std::function<void(const std::string&)> cb) {
        auto* ret = new FilePickerPopup();
        ret->slot = slotNum;
        ret->onPick = std::move(cb);
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() override {
        if (!Popup::init(460.f, 300.f, "GJ_square01.png")) return false;

        setTitle("Slot " + std::to_string(slot));

        m_files = bot::listMacros();
        buildList();

        // klasor yolu
        auto path = CCLabelBMFont::create(
            bot::safeName(bot::macrosDir().string()).c_str(), "goldFont.fnt"
        );
        path->setScale(0.25f);
        path->setColor({150, 150, 150});
        path->setPosition({0.f, -122.f});
        m_buttonMenu->addChild(path);

        return true;
    }

private:
    std::vector<std::string> m_files;
    int m_page = 0;
    cocos2d::CCNode* m_listNode = nullptr;

    static constexpr int kPerPage = 6;

    void buildList() {
        if (m_listNode) m_listNode->removeFromParent();
        m_listNode = cocos2d::CCNode::create();
        m_buttonMenu->addChild(m_listNode);

        int pages = (static_cast<int>(m_files.size()) + kPerPage - 1) / kPerPage;
        if (pages < 1) pages = 1;
        if (m_page >= pages) m_page = pages - 1;
        if (m_page < 0) m_page = 0;

        // "(Bos)" - slotu bosalt
        {
            auto label = CCLabelBMFont::create("(Bos)", "goldFont.fnt");
            label->setScale(0.4f);
            label->setColor({255, 130, 130});
            auto btn = CCMenuItemSpriteExtra::create(label, this, menu_selector(FilePickerPopup::onEmpty));
            btn->setPosition({0.f, 100.f});
            m_listNode->addChild(btn);
        }

        if (m_files.empty()) {
            auto empty = CCLabelBMFont::create(
                "Makro dosyasi bulunamadi.", "goldFont.fnt"
            );
            empty->setScale(0.45f);
            empty->setPosition({0.f, 20.f});
            m_listNode->addChild(empty);

            auto hint = CCLabelBMFont::create(
                "Once klasore .gdr2 dosyalarini koy.", "goldFont.fnt"
            );
            hint->setScale(0.35f);
            hint->setColor({150, 150, 150});
            hint->setPosition({0.f, -10.f});
            m_listNode->addChild(hint);
            return;
        }

        int start = m_page * kPerPage;
        for (int i = 0; i < kPerPage; i++) {
            int idx = start + i;
            if (idx >= static_cast<int>(m_files.size())) break;

            auto label = CCLabelBMFont::create(
                truncate(bot::safeName(m_files[idx]), 34).c_str(), "goldFont.fnt"
            );
            label->setScale(0.4f);
            auto btn = CCMenuItemSpriteExtra::create(label, this, menu_selector(FilePickerPopup::onFile));
            btn->setTag(idx);
            btn->setPosition({0.f, 68.f - i * 28.f});
            m_listNode->addChild(btn);
        }

        // sayfa oklari
        auto prev = CCMenuItemSpriteExtra::create(
            CCLabelBMFont::create("<", "goldFont.fnt"), this, menu_selector(FilePickerPopup::onPrev)
        );
        prev->setPosition({-190.f, 0.f});
        prev->setEnabled(m_page > 0);
        m_listNode->addChild(prev);

        auto next = CCMenuItemSpriteExtra::create(
            CCLabelBMFont::create(">", "goldFont.fnt"), this, menu_selector(FilePickerPopup::onNext)
        );
        next->setPosition({190.f, 0.f});
        next->setEnabled(m_page < pages - 1);
        m_listNode->addChild(next);

        std::string pageText = std::to_string(m_page + 1) + "/" + std::to_string(pages);
        auto pageLabel = CCLabelBMFont::create(pageText.c_str(), "goldFont.fnt");
        pageLabel->setScale(0.35f);
        pageLabel->setColor({150, 150, 150});
        pageLabel->setPosition({0.f, -100.f});
        m_listNode->addChild(pageLabel);
    }

    void pick(const std::string& name) {
        if (onPick) onPick(name);
        onClose(nullptr);
    }

    void onFile(cocos2d::CCObject* s) {
        int idx = s->getTag();
        if (idx >= 0 && idx < static_cast<int>(m_files.size()))
            pick(m_files[idx]);
    }

    void onEmpty(cocos2d::CCObject*) { pick(""); }

    void onPrev(cocos2d::CCObject*) { m_page--; buildList(); }
    void onNext(cocos2d::CCObject*) { m_page++; buildList(); }
};

// ===========================================================================
// Ana slot menusu
// ===========================================================================
SlotsPopup* SlotsPopup::create(std::function<void()> onClosed) {
    auto* ret = new SlotsPopup();
    ret->m_onClosed = std::move(onClosed);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SlotsPopup::init() {
    if (!Popup::init(500.f, 300.f, "GJ_square01.png")) return false;

    setTitle("Macro Slots");

    m_slots = bot::loadSlots();
    buildGrid();

    // alt bilgi yazisi
    auto hint = CCLabelBMFont::create(
        "Sag Shift: menu ac/kapa   |   1-9: slot   |   0: kapat", "goldFont.fnt"
    );
    hint->setScale(0.33f);
    hint->setColor({170, 170, 170});
    hint->setPosition({0.f, -80.f});
    m_buttonMenu->addChild(hint);

    auto pathLabel = CCLabelBMFont::create(
        bot::safeName(bot::macrosDir().string()).c_str(), "goldFont.fnt"
    );
    pathLabel->setScale(0.25f);
    pathLabel->setColor({140, 140, 140});
    pathLabel->setPosition({0.f, -100.f});
    m_buttonMenu->addChild(pathLabel);

    // Apply
    auto* applyBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Apply", 0.85f), this, menu_selector(SlotsPopup::onApply)
    );
    applyBtn->setPosition({-70.f, -125.f});
    m_buttonMenu->addChild(applyBtn);

    // Klasoru Ac
    auto* folderBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Klasoru Ac", 0.75f), this, menu_selector(SlotsPopup::onOpenFolder)
    );
    folderBtn->setPosition({70.f, -125.f});
    m_buttonMenu->addChild(folderBtn);

    return true;
}

void SlotsPopup::buildGrid() {
    if (m_gridNode) m_gridNode->removeFromParent();
    m_gridNode = cocos2d::CCNode::create();
    m_buttonMenu->addChild(m_gridNode);

    const float spacing = 52.f;
    const float startX = -208.f;   // 9 slot tek satir

    for (int i = 0; i < bot::kSlotCount; i++) {
        float x = startX + i * spacing;

        // slot numarasi
        auto* num = CCLabelBMFont::create(std::to_string(i + 1).c_str(), "goldFont.fnt");
        num->setScale(0.6f);
        num->setPosition({x, 62.f});
        m_gridNode->addChild(num);

        // atanmis makro kisaltmasi
        const std::string& file = m_slots[i];
        auto* name = CCLabelBMFont::create(
            (file.empty() ? "bos" : truncate(bot::safeName(file), 7)).c_str(), "goldFont.fnt"
        );
        name->setScale(0.28f);
        if (file.empty()) name->setColor({140, 140, 140});
        else              name->setColor({130, 255, 130});
        name->setPosition({x, 38.f});
        m_gridNode->addChild(name);

        // + butonu
        auto* btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("+", 0.75f), this, menu_selector(SlotsPopup::onPickFile)
        );
        btn->setTag(i);
        btn->setPosition({x, 5.f});
        m_gridNode->addChild(btn);
    }
}

void SlotsPopup::onPickFile(cocos2d::CCObject* sender) {
    int slot = sender->getTag();   // 0 tabanli

    auto* popup = FilePickerPopup::create(slot + 1, [this, slot](const std::string& name) {
        m_slots[slot] = name;
        buildGrid();
    });
    popup->show();
}

void SlotsPopup::onApply(cocos2d::CCObject*) {
    bot::saveSlots(m_slots);
    log::info("[MacroSlots] Slot ayarlari kaydedildi");
    Notification::create("Slotlar kaydedildi", NotificationIcon::Info, 2.f)->show();
}

void SlotsPopup::onOpenFolder(cocos2d::CCObject*) {
    bot::ensureMacrosDir();
#ifdef GEODE_IS_WINDOWS
    ShellExecuteW(nullptr, L"open", bot::macrosDir().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
    FLAlertLayer::create("Klasor", bot::macrosDir().string(), "OK")->show();
#endif
}

void SlotsPopup::onClose(cocos2d::CCObject* sender) {
    Popup::onClose(sender);
    if (m_onClosed) m_onClosed();
}

// ===========================================================================
// menu ac / kapa
// ===========================================================================
namespace {
SlotsPopup* g_openPopup = nullptr;
}

void menu::toggle() {
    if (g_openPopup) {
        g_openPopup->onClose(nullptr);
        g_openPopup = nullptr;
        return;
    }

    g_openPopup = SlotsPopup::create([] { g_openPopup = nullptr; });
    g_openPopup->show();
}
