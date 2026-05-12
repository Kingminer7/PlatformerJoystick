#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

class JoystickNode : public cocos2d::CCLayerRGBA {
protected:
    using GJBGL = GJBaseGameLayer;
    bool init(GJBGL* bgl);
    void draw() override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    bool m_enabled = true;
    bool m_counters = true;
    bool m_advCounters = true;
public:
    static JoystickNode* create(GJBGL* bgl);

    void setEnabled(bool enabled);
    bool isEnabled();

    void setCountersEnabled(bool enabled);
    bool isCountersEnabled();

    void setAdvancedCounters(bool advanced);
    bool isAdvancedCounters();

    void updateCounters();

    void onMove();
    void setInput(const cocos2d::CCPoint& where, double timestamp);
    void updateKeyboard(double timestamp);

    cocos2d::CCPoint m_input{0, 0};
    bool m_kbUp, m_kbDown, m_kbLeft, m_kbRight = false;
    GJBGL* m_bgl;
};