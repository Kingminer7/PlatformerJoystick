#include "JoystickSettingsPopup.hpp"

#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace geode::prelude;

bool JoystickSettingsPopup::init(LevelEditorLayer* editor) {
    if (!Popup::init(250, 125)) return false;

    setTitle("Query Options");
    setID("search-popup"_spr);
    m_title->setID("title");
    m_buttonMenu->setID("back-menu");
    m_closeBtn->setID("close-btn");
    m_mainLayer->setID("main-layer");
    m_bgSprite->setID("background");

    bool countersEnabled = alpha::level_storage::getSavedValue<bool>(editor, "joystick-counters", Mod::get());

    auto advancedToggle = CCMenuItemExt::createTogglerWithStandardSprites(1.f, [editor](auto btn){
        if (btn->m_notClickable) return;
        log::info("{}", !btn->isOn());
        alpha::level_storage::setSavedValue(editor, "joystick-advanced", !btn->isOn(), Mod::get());
        log::info("{}", !btn->isOn());
    });
    advancedToggle->setCascadeOpacityEnabled(true);
    advancedToggle->toggle(alpha::level_storage::getSavedValue<bool>(editor, "joystick-advanced", Mod::get()));

    auto advancedLabel = CCLabelBMFont::create("Advanced Mode", "bigFont.fnt");
    advancedLabel->setScale(.7f);
    advancedLabel->setAnchorPoint({0, 0.5});

    auto advancedHolder = CCMenu::create();
    advancedHolder->setID("advanced");
    advancedHolder->setScale(.8);
    advancedHolder->setOpacity(editor->m_levelSettings->m_platformerMode);
    advancedHolder->setContentSize({230, 31});
    advancedHolder->addChildAtPosition(advancedToggle, Anchor::Left, {advancedToggle->getContentWidth() / 2, 0});
    advancedHolder->addChildAtPosition(advancedLabel, Anchor::Left, {advancedToggle->getContentWidth() + 5, 0});

    advancedHolder->setOpacity((editor->m_levelSettings->m_platformerMode && countersEnabled) ? 255 : 100);
    advancedToggle->setClickable(editor->m_levelSettings->m_platformerMode && countersEnabled);

    m_mainLayer->addChildAtPosition(advancedHolder, Anchor::Center, {0, -20});

    auto counterToggle = CCMenuItemExt::createTogglerWithStandardSprites(1.f, [editor, advancedHolder, advancedToggle](auto btn){
        if (btn->m_notClickable) return;
        auto newVal = !btn->isOn();
        alpha::level_storage::setSavedValue(editor, "joystick-counters", newVal, Mod::get());
        advancedHolder->setOpacity((editor->m_levelSettings->m_platformerMode && newVal) ? 255 : 100);
        advancedToggle->setClickable(editor->m_levelSettings->m_platformerMode && newVal);
    });
    counterToggle->setCascadeOpacityEnabled(true);
    counterToggle->toggle(countersEnabled);

    auto counterLabel = CCLabelBMFont::create("Enable Joystick", "bigFont.fnt");
    counterLabel->setScale(.7f);
    counterLabel->setAnchorPoint({0, 0.5});

    auto counterHolder = CCMenu::create();
    counterHolder->setID("counter");
    counterHolder->setScale(.8);
    counterHolder->setContentSize({230, 31});
    counterHolder->addChildAtPosition(counterToggle, Anchor::Left, {counterToggle->getContentWidth() / 2, 0});
    counterHolder->addChildAtPosition(counterLabel, Anchor::Left, {counterToggle->getContentWidth() + 5, 0});

    counterHolder->setOpacity(editor->m_levelSettings->m_platformerMode ? 255 : 100);
    counterToggle->setClickable(editor->m_levelSettings->m_platformerMode);

    m_mainLayer->addChildAtPosition(counterHolder, Anchor::Center, {0, 10});

    return true;
}

JoystickSettingsPopup* JoystickSettingsPopup::create(LevelEditorLayer* editor) {
    auto ret = new JoystickSettingsPopup;
    if (!ret->init(editor)) return delete ret, ret;
    return ret->autorelease(), ret;
}