#include <Geode/Geode.hpp>
#include "JoystickNode.hpp"
#include "utils.hpp"
#ifndef GEODE_IS_WINDOWS
#include <cxxabi.h>
#endif

using namespace geode::prelude;

#include <Geode/modify/UILayer.hpp>
class $modify(JSUILayer, UILayer) {

    struct Fields {
        JoystickNode *m_joystickNode;
    };

    bool init(GJBaseGameLayer *gjbgl) {
        if (!UILayer::init(gjbgl)) return false;

        m_fields->m_joystickNode = JoystickNode::create();
        addChildAtPosition(m_fields->m_joystickNode, Anchor::BottomLeft, {50, 50}, false);

        fixVisibility();

        return true;
    }

    void fixVisibility() {
        if (!fastGetSetting<"enabled", bool>() || !m_inPlatformer) {
            if (!m_fields->m_joystickNode) {
                return;
            }
            m_fields->m_joystickNode->setVisible(false);
            m_fields->m_joystickNode->setTouchEnabled(false);
            return;
        }

        m_fields->m_joystickNode->setVisible(true);
        m_fields->m_joystickNode->setTouchEnabled(true);    

        if (auto p1move = getChildByID("platformer-p1-move-button")) {
            p1move->setPosition({10000, 10000});
        }

        if (auto p2move = getChildByID("platformer-p2-move-button")) {
            p2move->setPosition({10000, 10000});
        }
    }

    // refreshDpad is inlined :pensive:
    #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ARM_MAC)

    void togglePlatformerMode(bool p0) {    
        UILayer::togglePlatformerMode(p0);

        fixVisibility();
    }

    #else

    void refreshDpad() {
        UILayer::refreshDpad();

        fixVisibility();
    }

    #endif
};

#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ARM_MAC)

#include <Geode/modify/PlayLayer.hpp>
class $modify (JSPlayLayer, PlayLayer) {
    void resume() {
        PlayLayer::resume();
        if (auto layer = getChildByType<UILayer>(0)) {
            auto jsLayer = static_cast<JSUILayer*>(layer);
            if (jsLayer) {
                jsLayer->fixVisibility();
            }
        }
    }
};

#endif