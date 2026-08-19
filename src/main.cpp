#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <vector>
#include <string>
#include <algorithm>

using namespace geode::prelude;

// Available preset tags
const std::vector<std::string> AVAILABLE_TAGS = {
    "Memory", "Gimmick", "Nostalgic", "XL", "Sync", "Fast-Paced", "Learny", "Decoration"
};

// ==========================================
// Helper Functions for Persistence & Tag Logic
// ==========================================

static std::string getLevelKey(GJGameLevel* level) {
    if (!level) return "";
    return "tags_level_" + std::to_string(level->m_levelID.value());
}

static std::vector<std::string> getTagsForLevel(GJGameLevel* level) {
    if (!level) return {};
    std::string key = getLevelKey(level);
    return Mod::get()->getSavedValue<std::vector<std::string>>(key, {});
}

static void saveTagsForLevel(GJGameLevel* level, const std::vector<std::string>& tags) {
    if (!level) return;
    std::string key = getLevelKey(level);
    Mod::get()->setSavedValue(key, tags);
}

static bool levelHasTag(GJGameLevel* level, const std::string& tag) {
    auto tags = getTagsForLevel(level);
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

static void toggleTagForLevel(GJGameLevel* level, const std::string& tag) {
    auto tags = getTagsForLevel(level);
    auto it = std::find(tags.begin(), tags.end(), tag);
    if (it != tags.end()) {
        tags.erase(it);
    } else {
        tags.push_back(tag);
    }
    saveTagsForLevel(level, tags);
}

// Global active filter tags selected by user
static std::vector<std::string> g_activeFilters;

// ==========================================
// Tag Manager Popup UI
// ==========================================

class TagPopup : public Popup<GJGameLevel*> {
protected:
    GJGameLevel* m_level = nullptr;
    std::function<void()> m_onCloseCallback;

    bool setup(GJGameLevel* level) override {
        m_level = level;
        this->setTitle("Manage Level Tags");

        auto winSize = m_mainLayer->getContentSize();

        // Create container menu for toggle buttons
        auto menu = CCMenu::create();
        menu->setContentSize({winSize.width - 40.f, winSize.height - 70.f});
        menu->setPosition({winSize.width / 2.f, winSize.height / 2.f - 10.f});
        
        // Arrange tags in a neat grid layout
        auto layout = RowLayout::create();
        layout->setGap(8.f);
        layout->setFlexWrap(FlexWrap::Wrap);
        layout->setAxisAlignment(AxisAlignment::Center);
        menu->setLayout(layout);

        m_mainLayer->addChild(menu);

        refreshButtons(menu);
        return true;
    }

    void refreshButtons(CCMenu* menu) {
        menu->removeAllChildren();

        auto currentTags = getTagsForLevel(m_level);

        for (const auto& tag : AVAILABLE_TAGS) {
            bool hasTag = std::find(currentTags.begin(), currentTags.end(), tag) != currentTags.end();

            // Use GD Sprites for toggle-style display
            auto label = CCLabelBMFont::create(tag.c_str(), "bigFont.fnt");
            label->setScale(0.4f);

            auto btnSpr = ButtonSprite::create(
                tag.c_str(),
                80,
                true,
                "bigFont.fnt",
                hasTag ? "GJ_button_02.png" : "GJ_button_01.png",
                25.f,
                0.4f
            );

            auto btn = CCMenuItemSpriteExtra::create(
                btnSpr,
                this,
                menu_selector(TagPopup::onToggleTag)
            );
            btn->setUserObject(CCString::create(tag));
            menu->addChild(btn);
        }

        menu->updateLayout();
    }

    void onToggleTag(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto tagObj = static_cast<CCString*>(btn->getUserObject());
        if (!tagObj) return;

        std::string tag = tagObj->getCString();
        toggleTagForLevel(m_level, tag);

        // Refresh UI
        refreshButtons(static_cast<CCMenu*>(btn->getParent()));
    }

public:
    static TagPopup* create(GJGameLevel* level, std::function<void()> onClose = nullptr) {
        auto ret = new TagPopup();
        ret->m_onCloseCallback = onClose;
        if (ret && ret->initAnchored(320.f, 220.f, level)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onClose(CCObject* sender) override {
        if (m_onCloseCallback) {
            m_onCloseCallback();
        }
        Popup::onClose(sender);
    }
};

// ==========================================
// LevelInfoLayer Hook (Tag Display & Button)
// ==========================================

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCNode* m_tagContainer = nullptr;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Add 'Tags' button to the right side menu
        auto rightMenu = this->getChildByID("right-side-menu");
        if (rightMenu) {
            auto tagSpr = ButtonSprite::create("Tags", "goldFont.fnt", "GJ_button_01.png", .7f);
            tagSpr->setScale(0.7f);

            auto tagBtn = CCMenuItemSpriteExtra::create(
                tagSpr,
                this,
                menu_selector(MyLevelInfoLayer::onOpenTags)
            );
            tagBtn->setID("tags-button"_spr);
            rightMenu->addChild(tagBtn);
            rightMenu->updateLayout();
        }

        // Create persistent container node for tag badges
        m_fields->m_tagContainer = CCNode::create();
        m_fields->m_tagContainer->setPosition({winSize.width / 2.f, 80.f});
        
        auto layout = RowLayout::create();
        layout->setGap(5.f);
        layout->setAxisAlignment(AxisAlignment::Center);
        m_fields->m_tagContainer->setLayout(layout);
        
        this->addChild(m_fields->m_tagContainer);

        updateTagBadges();
        return true;
    }

    void onOpenTags(CCObject* sender) {
        auto popup = TagPopup::create(m_level, [this]() {
            this->updateTagBadges();
        });
        popup->show();
    }

    void updateTagBadges() {
        if (!m_fields->m_tagContainer || !m_level) return;

        m_fields->m_tagContainer->removeAllChildren();
        auto tags = getTagsForLevel(m_level);

        for (const auto& tag : tags) {
            auto bg = CCScale9Sprite::create("square02_001.png");
            bg->setOpacity(150);
            bg->setContentSize({60.f, 18.f});

            auto lbl = CCLabelBMFont::create(tag.c_str(), "smallFont.fnt");
            lbl->setScale(0.5f);
            lbl->setPosition(bg->getContentSize() / 2.f);
            bg->addChild(lbl);

            m_fields->m_tagContainer->addChild(bg);
        }

        m_fields->m_tagContainer->setContentSize({300.f, 25.f});
        m_fields->m_tagContainer->updateLayout();
    }
};

// ==========================================
// LevelBrowserLayer Hook (Filtering System)
// ==========================================

class FilterPopup : public Popup<CCArray*> {
protected:
    std::function<void()> m_onApplyFilter;

    bool setup(CCArray* levels) override {
        this->setTitle("Filter Levels by Tags");
        auto winSize = m_mainLayer->getContentSize();

        auto menu = CCMenu::create();
        menu->setContentSize({winSize.width - 40.f, winSize.height - 70.f});
        menu->setPosition({winSize.width / 2.f, winSize.height / 2.f});

        auto layout = RowLayout::create();
        layout->setGap(8.f);
        layout->setFlexWrap(FlexWrap::Wrap);
        layout->setAxisAlignment(AxisAlignment::Center);
        menu->setLayout(layout);

        for (const auto& tag : AVAILABLE_TAGS) {
            bool isActive = std::find(g_activeFilters.begin(), g_activeFilters.end(), tag) != g_activeFilters.end();

            auto btnSpr = ButtonSprite::create(
                tag.c_str(),
                80,
                true,
                "bigFont.fnt",
                isActive ? "GJ_button_02.png" : "GJ_button_04.png",
                25.f,
                0.4f
            );

            auto btn = CCMenuItemSpriteExtra::create(
                btnSpr,
                this,
                menu_selector(FilterPopup::onToggleFilterTag)
            );
            btn->setUserObject(CCString::create(tag));
            menu->addChild(btn);
        }

        menu->updateLayout();
        m_mainLayer->addChild(menu);
        return true;
    }

    void onToggleFilterTag(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto tagObj = static_cast<CCString*>(btn->getUserObject());
        if (!tagObj) return;

        std::string tag = tagObj->getCString();
        auto it = std::find(g_activeFilters.begin(), g_activeFilters.end(), tag);
        if (it != g_activeFilters.end()) {
            g_activeFilters.erase(it);
        } else {
            g_activeFilters.push_back(tag);
        }

        if (m_onApplyFilter) {
            m_onApplyFilter();
        }
        this->onClose(sender);
    }

public:
    static FilterPopup* create(CCArray* levels, std::function<void()> onApply) {
        auto ret = new FilterPopup();
        ret->m_onApplyFilter = onApply;
        if (ret && ret->initAnchored(320.f, 220.f, levels)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Add Filter button to browser header
        auto menu = CCMenu::create();
        menu->setPosition({winSize.width - 40.f, winSize.height - 30.f});

        auto filterSpr = ButtonSprite::create("Tag Filter", "goldFont.fnt", "GJ_button_01.png", .6f);
        filterSpr->setScale(0.6f);

        auto filterBtn = CCMenuItemSpriteExtra::create(
            filterSpr,
            this,
            menu_selector(MyLevelBrowserLayer::onOpenFilter)
        );
        menu->addChild(filterBtn);
        this->addChild(menu);

        return true;
    }

    void onOpenFilter(CCObject* sender) {
        auto popup = FilterPopup::create(m_levels, [this]() {
            this->setupLevelBrowser(m_levels);
        });
        popup->show();
    }

    void setupLevelBrowser(CCArray* levels) override {
        // Filter out levels that don't match selected active tags
        if (!g_activeFilters.empty() && levels) {
            auto filtered = CCArray::create();
            for (auto obj : CCDirectiveEnumerate(levels, GJGameLevel*)) {
                bool matchesAll = true;
                for (const auto& tag : g_activeFilters) {
                    if (!levelHasTag(obj, tag)) {
                        matchesAll = false;
                        break;
                    }
                }
                if (matchesAll) {
                    filtered->addObject(obj);
                }
            }
            LevelBrowserLayer::setupLevelBrowser(filtered);
            return;
        }

        LevelBrowserLayer::setupLevelBrowser(levels);
    }
};