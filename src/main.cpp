#include <Geode/Geode.hpp>
#include <Geode/loader/GameEvent.hpp>
#include "JoystickNode.hpp"
#include "utils.hpp"

using namespace geode::prelude;

#include <Geode/modify/UILayer.hpp>
class $modify(JSUILayer, UILayer) {

    struct Fields {
        JoystickNode *m_joystickNode;
    };

    bool init(GJBaseGameLayer *gjbgl) {
        if (!UILayer::init(gjbgl)) return false;
        queueInMainThread([this, gjbgl](){
            m_fields->m_joystickNode = JoystickNode::create();
            log::info("{}", gjbgl);
            log::info("{}", gjbgl->m_level);
            log::info("{}", gjbgl->m_level->m_twoPlayerMode);
            m_fields->m_joystickNode->m_twoPlayer = gjbgl->m_level->m_twoPlayerMode;
            addChildAtPosition(m_fields->m_joystickNode, Anchor::BottomLeft, {75, 75}, false);

            fixVisibility();

        });
        return true;
    }

    void fixVisibility() {
        if (!m_fields->m_joystickNode) {
            return;
        }
        if (!fastGetSetting<"enabled", bool>() || !m_inPlatformer) {
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

    void refreshDpad() {
        UILayer::refreshDpad();
        fixVisibility();
    }
};

#include <ninxout.options_api/include/API.hpp>

$on_mod(Loaded) {
    // update: ninx made it early load but im too lazy to delete the line via web editor so comment it is
    // options api isnt early load so if i didnt do this it would just not register the settings
    // (new geode::EventListener<geode::GameEventFilter>(geode::GameEventType::Loaded))->bind([](geode::GameEvent* ev) {
        OptionsAPI::addPreLevelSetting<bool>(
            "Platformer Joystick",
            "enable"_spr,
            [](GJGameLevel*) {
                Mod::get()->setSettingValue<bool>("enabled", !fastGetSetting<"enabled", bool>());
            },
            [](GJGameLevel*) { return fastGetSetting<"enabled", bool>(); },
            "Enables the joystick in platformer levels."
        );
        OptionsAPI::addMidLevelSetting<bool>(
            "Platformer Joystick",
            "enable"_spr,
            [](GJBaseGameLayer*) { Mod::get()->setSettingValue<bool>("enabled", !fastGetSetting<"enabled", bool>()); },
            [](GJBaseGameLayer*) { return fastGetSetting<"enabled", bool>(); },
            "Enables the joystick in platformer levels."
        );
    // });
}
