#pragma once

#include <Geode/ui/Popup.hpp>
#include "JoystickNode.hpp"

class FakeJoystick;

class JoystickEditPopup : public geode::Popup {
protected:
    bool init();

    FakeJoystick* m_joystick;
public:
    static JoystickEditPopup* create();
};

class FakeJoystick : public cocos2d::CCLayerRGBA {
protected:
    bool init(JoystickEditPopup* popup);
    void draw() override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    JoystickEditPopup* m_popup;
public:
    static FakeJoystick* create(JoystickEditPopup* popup);
};