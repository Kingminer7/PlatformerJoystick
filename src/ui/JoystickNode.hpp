#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

class JoystickNode : public cocos2d::CCLayerRGBA {
protected:
    bool init() override;
    void draw() override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    bool m_enabled = true;
public:
    static JoystickNode* create();

    void setEnabled(bool enabled);
    bool isEnabled();

    bool m_kbUp = false;
    bool m_kbDown = false;
    bool m_kbLeft = false;
    bool m_kbRight = false;

    cocos2d::CCPoint m_input{0, 0};
};