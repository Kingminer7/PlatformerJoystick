#include "JoystickNode.hpp"
#include "utils.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

// #ifndef GEODE_IS_IOS
// #include <geode.custom-keybinds/include/Keybinds.hpp>

// using namespace keybinds;

// $on_mod(Loaded) {
//     keybinds::BindManager::get()->registerBindable({
//         "joystick_up"_spr,
//         "Joystick Up",
//         "Up for joystick",
//         { 
//             keybinds::Keybind::create(KEY_W, keybinds::Modifier::None),
//             keybinds::Keybind::create(CONTROLLER_Up, keybinds::Modifier::None),
//             keybinds::Keybind::create(KEY_ArrowUp, keybinds::Modifier::None),
//          },
//         "Joystick"
//     });
//     keybinds::BindManager::get()->registerBindable({
//         "joystick_down"_spr,
//         "Joystick Down",
//         "Down for joystick",
//         { 
//             keybinds::Keybind::create(KEY_S, keybinds::Modifier::None),
//             keybinds::Keybind::create(CONTROLLER_Down, keybinds::Modifier::None),
//             keybinds::Keybind::create(KEY_ArrowDown, keybinds::Modifier::None),
//          },
//         "Joystick"
//     });
//     keybinds::BindManager::get()->registerBindable({
//         "joystick_left"_spr,
//         "Joystick Left",
//         "Left for joystick",
//         { 
//             keybinds::Keybind::create(KEY_A, keybinds::Modifier::None),
//             keybinds::Keybind::create(CONTROLLER_Left, keybinds::Modifier::None),
//             keybinds::Keybind::create(KEY_ArrowLeft, keybinds::Modifier::None),
//          },
//         "Joystick"
//     });
//     keybinds::BindManager::get()->registerBindable({
//         "joystick_right"_spr,
//         "Joystick Right",
//         "Right for joystick",
//         { 
//             keybinds::Keybind::create(KEY_D, keybinds::Modifier::None),
//             keybinds::Keybind::create(CONTROLLER_Right, keybinds::Modifier::None),
//             keybinds::Keybind::create(KEY_ArrowRight, keybinds::Modifier::None),
//          },
//         "Joystick"
//     });
// }
// #endif

bool enableJoystick = false;
bool enableCounters = false;

void runChecks(CCArray *objects) {
    enableJoystick = false;
    enableCounters = false;
    for (auto obj : CCArrayExt<GameObject*>(objects)) {
        if (obj->m_objectID == 914) {
                if (auto txt = static_cast<TextGameObject*>(obj)) {
                    if (txt->m_text == "--enable-joystick") 
                        enableJoystick = true;
                    else if (txt->m_text == "--joystick-counters") 
                        enableCounters = true;
            } 
        }
    }
}

void updateVal(GJBaseGameLayer *layer, int id, int val) {
    if (enableCounters) {
        layer->m_effectManager->updateCountForItem(id, val);
        layer->updateCounters(id, val);
    }
}

class $modify(JSPL, PlayLayer) {
    struct Fields {
        bool m_hasPaused = false;
    };
    void setupHasCompleted() {
        runChecks(m_objects);
        PlayLayer::setupHasCompleted();
        updateVal(this, 3740, 1);
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        updateVal(this, 3740, 1);
    }
};

class $modify(JSLEL, LevelEditorLayer) {
    void onPlaytest() {
        runChecks(m_objects);
        LevelEditorLayer::onPlaytest();
        updateVal(this, 3740, 1);
    }
};

bool JoystickNode::init() {
    if (!CCMenu::init()) return false;
    setContentSize({100, 100});

    m_bg = CCSprite::createWithSpriteFrameName("d_circle_02_001.png");
    m_bg->setScale(getContentSize().width / m_bg->getContentSize().width);
    m_bg->setOpacity(165);
    m_bg->setColor({189, 189, 189});
    addChildAtPosition(m_bg, Anchor::Center);

    m_center = CCSprite::createWithSpriteFrameName("d_circle_02_001.png");
    m_center->setScale(getContentSize().width / m_center->getContentSize().width / 3);
    m_center->setOpacity(236);
    m_center->setZOrder(1);
    m_center->setColor({0, 0, 0});
    addChildAtPosition(m_center, Anchor::Center);

    registerWithTouchDispatcher();

    return true;
}

JoystickNode *JoystickNode::create() {
    auto ret = new JoystickNode;
    if (!ret->init()) {
        delete ret;
        return nullptr;
    }
    ret->autorelease();
    return ret;
}

bool JoystickNode::ccTouchBegan(CCTouch *touch, CCEvent *event) {
    if (!isTouchEnabled() || !nodeIsVisible(this)) return false;
    if (ccpDistance(getPosition(), touch->getLocation()) <= getScaledContentSize().width / 2) {
        ccTouchMoved(touch, event);
        return true;
    }
    return false;
}

void JoystickNode::handleInput(GJBaseGameLayer *layer, CCPoint input, CCPoint old) {
    if (old.x == 1) {
        layer->queueButton(3, false, false);
    } else if (old.x == -1) {
        layer->queueButton(2, false, false);
    }
    if (!fastGetSetting<"disable-updown", bool>() || m_twoPlayer) {
        if (old.y == 1) {
            layer->queueButton(3, false, true);
        } else if (old.y == -1) { 
            layer->queueButton(2, false, true);
        }
    }

    if (input.x == 1) {
        layer->queueButton(3, true, false);
    } else if (input.x == -1) {
        layer->queueButton(2, true, false);
    }
    updateVal(layer, 3741, input.x);
    if (!fastGetSetting<"disable-updown", bool>() || m_twoPlayer) {
        if (input.y == 1) {
            layer->queueButton(3, true, true);
        } else if (input.y == -1) {
            layer->queueButton(2, true, true);
        }
        updateVal(layer, 3742, input.y);
    }
}

void JoystickNode::ccTouchEnded(CCTouch *touch, CCEvent *event) {
    if (auto uil = UILayer::get(); uil && uil->m_gameLayer) {
        handleInput(uil->m_gameLayer, {0, 0}, m_currentInput);
    }

    m_currentInput = CCPoint{0, 0};

    m_center->setPosition(getScaledContentSize() / 2);
}

void JoystickNode::ccTouchMoved(CCTouch *touch, CCEvent *event) {
    auto pos = convertToNodeSpace(touch->getLocation());

    auto fromCenter = pos - getScaledContentSize() / 2;

    if (fromCenter.getLength() > getScaledContentWidth() / 2 - m_center->getScaledContentWidth() / 2) {
        fromCenter = fromCenter.normalize() * (getScaledContentWidth() / 2 - m_center->getScaledContentWidth() / 2);
        pos = fromCenter + getScaledContentSize() / 2;
    }

    CCPoint inp = {0, 0};

    auto angle = atan2(fromCenter.y, fromCenter.x);

    if (std::abs(fromCenter.x) > std::abs(fromCenter.normalize().x) * 15) {
        if (angle > 5 * -M_PI / 12 && angle < 5 * M_PI / 12) {
            inp.x = 1;
        } else if (angle > 7 * M_PI / 12 || angle < 7 * -M_PI / 12) {
            inp.x = -1;
        }
    }

    if (std::abs(fromCenter.y) > std::abs(fromCenter.normalize().y) * 15) {
        if (angle > M_PI / 12 && angle < 11 * M_PI / 12) {
            inp.y = 1;
        } else if (angle < -M_PI / 12 && angle > -11 * M_PI / 12) {
            inp.y = -1;
        }
    }

    if (inp != m_currentInput) {
        if (auto uil = UILayer::get(); uil && uil->m_gameLayer) {
            handleInput(uil->m_gameLayer, inp, m_currentInput);
        }
    }
    m_currentInput = inp;

    m_center->setPosition(pos);
}

void JoystickNode::ccTouchCancelled(CCTouch *touch, CCEvent *event) {
    ccTouchEnded(touch, event);
}

void JoystickNode::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, -512, true);
}

void JoystickNode::draw() {
    CCMenu::draw();

    #if defined(DEBUG_BUILD) && !defined(GITHUB_ACTIONS)
    {
        // Draw green arc for horizontal movement (right)
        ccDrawColor4B(0, 255, 0, 100); // Set color to green with some transparency
        auto center = getScaledContentSize() / 2; // Center of the circle
        auto radius = getScaledContentWidth() / 2; // Radius of the circle
        auto startAngle = 5 * -M_PI / 12; // Start angle
        auto sweepAngle = 5 * M_PI / 12 - 5 * -M_PI / 12; // Sweep angle
        int segments = 50; // Number of segments for smoothness

        std::vector<CCPoint> vertices;
        vertices.push_back(center); // Add the center point

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.push_back({
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            });
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(0, 1, 0, 0.15f));
    }

    {
        // Draw green arc for horizontal movement (Left)
        ccDrawColor4B(0, 255, 0, 100); // Set color to green with some transparency
        auto center = getScaledContentSize() / 2; // Center of the circle
        auto radius = getScaledContentWidth() / 2; // Radius of the circle
        auto startAngle = 7 * M_PI / 12; // Start angle
        auto sweepAngle = 11 * M_PI / 12 - M_PI / 12; // Sweep angle
        int segments = 50; // Number of segments for smoothness

        std::vector<CCPoint> vertices;
        vertices.push_back(center); // Add the center point

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.push_back({
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            });
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(0, 1, 0, 0.15f));
    }

    {
        // Draw red arc for vertical movement (up)
        ccDrawColor4B(255, 0, 0, 100); // Set color to red with some transparency
        auto center = getScaledContentSize() / 2; // Center of the circle
        auto radius = getScaledContentWidth() / 2; // Radius of the circle
        auto startAngle = M_PI / 12; // Start angle
        auto sweepAngle = 11 * M_PI / 12 - M_PI / 12; // Sweep angle
        int segments = 50; // Number of segments for smoothness

        std::vector<CCPoint> vertices;
        vertices.push_back(center); // Add the center point

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.push_back({
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            });
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(1, 0, 0, 0.15f));
    }

    {
        // Draw red arc for vertical movement (down)
        ccDrawColor4B(255, 0, 0, 100); // Set color to red with some transparency
        auto center = getScaledContentSize() / 2; // Center of the circle
        auto radius = getScaledContentWidth() / 2; // Radius of the circle
        auto startAngle = -11 * M_PI / 12; // Start angle
        auto sweepAngle = -M_PI / 12 - -11 * M_PI / 12; // Sweep angle
        int segments = 50; // Number of segments for smoothness

        std::vector<CCPoint> vertices;
        vertices.push_back(center); // Add the center point

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.push_back({
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            });
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(1, 0, 0, 0.15f));
    }
    #endif
}

#include <Geode/modify/UILayer.hpp>
class $modify(JSUILayer, UILayer) {

    struct Fields {
        JoystickNode *m_joystickNode;
    };

    static void onModify(auto& self) {
        if (auto res = self.setHookPriorityBefore("UILayer::init", "geode.custom-keybinds"); res.isErr()) {
            geode::log::warn("Failed to set hook priority: {}", res.unwrapErr());
        }
    }

    bool init(GJBaseGameLayer *gjbgl) {
        if (!UILayer::init(gjbgl)) return false;
        queueInMainThread([this, gjbgl](){
            m_fields->m_joystickNode = JoystickNode::create();
            m_fields->m_joystickNode->m_twoPlayer = gjbgl->m_level->m_twoPlayerMode;
            addChildAtPosition(m_fields->m_joystickNode, Anchor::BottomLeft, {75, 75}, false);

            fixVisibility();

            // #ifndef GEODE_IS_IOS
            // // No
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([=](keybinds::InvokeBindEvent* event) {
            //     log::info("Test - Left p1");
            //     return ListenerResult::Stop;
            // }, "robtop.geometry-dash/move-left-p1");
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([=](keybinds::InvokeBindEvent* event) {
            //     log::info("Test - Right p1");
            //     return ListenerResult::Stop;
            // }, "robtop.geometry-dash/move-right-p1");
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([=](keybinds::InvokeBindEvent* event) {
            //     log::info("Test - Left p2");
            //     return ListenerResult::Stop;
            // }, "robtop.geometry-dash/move-left-p2");
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([=](keybinds::InvokeBindEvent* event) {
            //     log::info("Test - Right p2");
            //     return ListenerResult::Stop;
            // }, "robtop.geometry-dash/move-right-p2");

            // // mine
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
            //     if (auto jsLayer = static_cast<JSUILayer*>(getChildByType<UILayer>(0))) {
            //         if (auto node = jsLayer->m_fields->m_joystickNode) {
            //             auto old = node->m_currentInput;
            //             if (event->isDown()) node->m_currentInput.x += 1;
            //             else node->m_currentInput.x -= 1;
            //             jsLayer->m_fields->m_joystickNode->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
            //         }
            //     }
            //     return ListenerResult::Stop;
            // }, "joystick_right"_spr);
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
            //     if (auto jsLayer = static_cast<JSUILayer*>(getChildByType<UILayer>(0))) {
            //         if (auto node = jsLayer->m_fields->m_joystickNode) {
            //             auto old = node->m_currentInput;
            //             if (event->isDown()) node->m_currentInput.x -= 1;
            //             else node->m_currentInput.x += 1;
            //             jsLayer->m_fields->m_joystickNode->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
            //         }
            //     }
            //     return ListenerResult::Stop;
            // }, "joystick_left"_spr);
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
            //     if (auto jsLayer = static_cast<JSUILayer*>(getChildByType<UILayer>(0))) {
            //         if (auto node = jsLayer->m_fields->m_joystickNode) {
            //             auto old = node->m_currentInput;
            //             if (event->isDown()) node->m_currentInput.y += 1;
            //             else node->m_currentInput.y -= 1;
            //             jsLayer->m_fields->m_joystickNode->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
            //         }
            //     }
            //     return ListenerResult::Stop;
            // }, "joystick_up"_spr);
            // PlayLayer::get()->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
            //     if (auto jsLayer = static_cast<JSUILayer*>(getChildByType<UILayer>(0))) {
            //         if (auto node = jsLayer->m_fields->m_joystickNode) {
            //             auto old = node->m_currentInput;
            //             if (event->isDown()) node->m_currentInput.y -= 1;
            //             else node->m_currentInput.y += 1;
            //             jsLayer->m_fields->m_joystickNode->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
            //         }
            //     }
            //     return ListenerResult::Stop;
            // }, "joystick_down"_spr);
            // #endif

        });
        return true;
    }

    void fixVisibility() {
        if (!m_fields->m_joystickNode) {
            return;
        }
        if (!enableJoystick || !m_inPlatformer) { // fastGetSetting<"enabled", bool>()
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

    // // refreshDpad is inlined :pensive:
    // #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ARM_MAC)

    // void togglePlatformerMode(bool p0) {    
    //     UILayer::togglePlatformerMode(p0);

    //     fixVisibility();
    // }

    // #else

    void refreshDpad() {
        UILayer::refreshDpad();
        
        fixVisibility();
    }

    // #endif
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


// #include <ninxout.options_api/include/API.hpp>

// $on_mod(Loaded) {
//     OptionsAPI::addPreLevelSetting<bool>(
//         "Platformer Joystick",
//         "enable"_spr,
//         [](GJGameLevel *level) {
//             auto key = fmt::format("joystick-on-{}", level->m_levelID);
//             Mod::get()->setSavedValue<bool>(key, !Mod::get()->getSavedValue<bool>(key, true));
//         },
//         [](GJGameLevel *level) {
//             return Mod::get()->getSavedValue<bool>(fmt::format("joystick-on-{}", level->m_levelID), true);
//         },
//         "Enables the joystick in this level. (Platformer only)"
//     );
//     OptionsAPI::addMidLevelSetting<bool>(
//         "Platformer Joystick (Global)",
//         "enable"_spr,
//         [](GJBaseGameLayer*) { Mod::get()->setSettingValue<bool>("enabled", !fastGetSetting<"enabled", bool>()); },
//         [](GJBaseGameLayer*) { return fastGetSetting<"enabled", bool>(); },
//         "Enables the joystick in platformer levels."
//     );
// }
