#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class JoystickNode : public CCMenu {
    protected:
        bool init() override;
        void draw() override;

        bool ccTouchBegan(CCTouch *touch, CCEvent *event) override;
        void ccTouchEnded(CCTouch *touch, CCEvent *event) override;
        void ccTouchMoved(CCTouch *touch, CCEvent *event) override;
        void ccTouchCancelled(CCTouch *touch, CCEvent *event) override;

        CCSprite *m_bg = nullptr;
        CCSprite *m_center = nullptr;
    public:
        static JoystickNode *create();
        void handleInput(GJBaseGameLayer *layer, const CCPoint& input, const CCPoint& old, double stamp) const;
        void fakePosition();
        
        bool m_twoPlayer = false;
        CCPoint m_currentInput = {0, 0};
};

template <string::ConstexprString S, typename T>
T const& getSettingFast() {
    static T value = (
        listenForSettingChanges<T>(S.data(), [](T val) {
            value = std::move(val);
        }),
        getMod()->getSettingValue<T>(S.data())
    );
    return value;
}