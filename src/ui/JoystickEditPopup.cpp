#include "JoystickEditPopup.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool JoystickEditPopup::init() {
    auto ws = CCDirector::get()->getWinSize();
    if (!Popup::init(ws.width, ws.height)) return false;
    m_bgSprite->removeFromParent();
    setColor(ccColor3B{51, 68, 153});
    setOpacity(255);
    setID("JoystickEditPopup"_spr);
    setTitle("Edit Joystick");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_closeBtn->setID("close-button");
    m_title->setID("title");
    m_noElasticity = true;
    m_closeBtn->setSprite(CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"));
    static_cast<AnchorLayoutOptions*>(m_closeBtn->getLayoutOptions())->setOffset(CCPoint{24.25, -23});
    m_buttonMenu->updateLayout();

    m_joystick = FakeJoystick::create(this);
    m_mainLayer->addChildAtPosition(m_joystick, Anchor::BottomLeft, CCPoint{Mod::get()->getSavedValue<float>("joystick-x", 100), Mod::get()->getSavedValue<float>("joystick-y", 80)});

    return true;
}

JoystickEditPopup* JoystickEditPopup::create() {
    auto ret = new JoystickEditPopup;
    if (!ret->init()) {
        delete ret;
        return nullptr;
    }
    ret->autorelease();
    return ret;
}

$on_mod(Loaded) {
    ButtonSettingPressedEventV3(Mod::get(), "pos").listen([] (auto buttonKey) {
        JoystickEditPopup::create()->show();
    }).leak();
}


bool FakeJoystick::init(JoystickEditPopup* popup) {
    if (!CCLayer::init()) return false;

    m_popup = popup;

    setID("joystick"_spr);
    setTouchEnabled(true);
    setTouchMode(ccTouchesMode::kCCTouchesOneByOne);
    setContentSize({100, 100});
    setAnchorPoint({.5, .5});
    ignoreAnchorPointForPosition(false);
    
    return true;
}

void FakeJoystick::draw() {
    CCLayerRGBA::draw();
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ccDrawColor4B(ccColor4B{_displayedColor.r, _displayedColor.g, _displayedColor.b, (GLubyte) (105 * _displayedOpacity / 255)});
    ccDrawFilledCircle(getContentSize() / 2, getContentWidth() / 2, 0, 64);
    ccDrawColor4B(ccColor4B{0, 0, 0, (GLubyte) (226 * _displayedOpacity / 255)});
    ccDrawFilledCircle(getContentSize() / 2, getContentWidth() / 6, 0, 64);
}

bool FakeJoystick::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto local = convertTouchToNodeSpace(touch) - getContentSize() / 2;
    if (local.getLength() < getContentWidth() / 2) {
        return true;
    }
    return false;
}

void FakeJoystick::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    setPosition(CCPoint{
        std::clamp(getPosition().x + touch->getDelta().x, 0.f, getParent()->getContentWidth()), 
        std::clamp(getPosition().y + touch->getDelta().y, 0.f, getParent()->getContentHeight())
    });
}

void FakeJoystick::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    Mod::get()->setSavedValue<float>("joystick-x", getPositionX());
    Mod::get()->setSavedValue<float>("joystick-y", getPositionY());
}

void FakeJoystick::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

FakeJoystick* FakeJoystick::create(JoystickEditPopup* popup) {
    auto ret = new FakeJoystick();
    if (!ret->init(popup)) {
        delete ret;
        return nullptr;
    }
    ret->autorelease();
    return ret;
}