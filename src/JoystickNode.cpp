#include "JoystickNode.hpp"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Popup.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/UILayer.hpp>

#ifdef GEODE_IS_DESKTOP
#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <Geode/loader/GameEvent.hpp>

$on_mod(Loaded) {
    auto mgr = keybinds::BindManager::get();
    mgr->registerBindable({
        "joystick_up"_spr,
        "Joystick Up",
        "Up for joystick",
        { 
            keybinds::Keybind::create(KEY_W, keybinds::Modifier::None),
            keybinds::ControllerBind::create(CONTROLLER_LTHUMBSTICK_UP),
            keybinds::Keybind::create(KEY_ArrowUp, keybinds::Modifier::None),
        },
        "Joystick",
        false
    });
    mgr->registerBindable({
        "joystick_down"_spr,
        "Joystick Down",
        "Down for joystick",
        { 
            keybinds::Keybind::create(KEY_S, keybinds::Modifier::None),
            keybinds::ControllerBind::create(CONTROLLER_LTHUMBSTICK_DOWN),
            keybinds::Keybind::create(KEY_ArrowDown, keybinds::Modifier::None),
        },
        "Joystick",
        false
    });
}

#endif

bool enableJoystick = false;

void runChecks(CCArray *objects) {
    enableJoystick = false;
    for (auto obj : CCArrayExt<GameObject*>(objects)) {
        if (obj->m_objectID == 914) {
            if (auto txt = static_cast<TextGameObject*>(obj)) {
                if (txt->m_text == "--enable-joystick") 
                    enableJoystick = true;;
            } 
        }
    }
}

#ifdef GEODE_IS_DESKTOP
#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(JSLIL, LevelInfoLayer) {
    struct Fields {
        bool m_hasVerified = false;
    };
    void popup(bool bounce = true) {
        auto jbinds = keybinds::BindManager::get()->getBindsFor("robtop.geometry-dash/jump-p1");
        auto mbinds = keybinds::BindManager::get()->getBindsFor("joystick_up"_spr);
        bool overlap = false;
        for (const auto& jbind : jbinds) {
            for (const auto& mbind : mbinds) {
                if (jbind->isEqual(mbind)) {
                    overlap = true;
                    break;
                }
            }
            if (overlap) break;
        }
        if (!overlap) {
            m_fields->m_hasVerified = true;
            if (GameManager::get()->getGameVariable("0128")) PlatformToolbox::hideCursor();
            playStep4();
            return;
        }
        
        auto po = createQuickPopup("Warning", "Your jump keybind seems to be set to the same as your up keybind. If you don't change it, you will not be able to input up.", "Ok", "Keybind Config", [this](auto, bool btn2){
            if (btn2) {
                popup(false);
                // :/
                (this->*(menu_selector(MoreOptionsLayer::onKeybindings)))(nullptr);
            } else {
                m_fields->m_hasVerified = true;
                if (GameManager::get()->getGameVariable("0128")) PlatformToolbox::hideCursor();
                playStep4();
            }
        }, false);
        po->m_noElasticity = !bounce;
        po->show();
    }
    void playStep4() {
        if (!enableJoystick || m_fields->m_hasVerified) {
            LevelInfoLayer::playStep4();
            return;
        }
        PlatformToolbox::showCursor();
        popup();
    }
};

#include <Geode/modify/EditLevelLayer.hpp>
class $modify(JSELL, EditLevelLayer) {
    struct Fields {
        bool m_hasVerified = false;
    };
    void popup(bool bounce = true) {
        auto jbinds = keybinds::BindManager::get()->getBindsFor("robtop.geometry-dash/jump-p1");
        auto mbinds = keybinds::BindManager::get()->getBindsFor("joystick_up"_spr);
        bool overlap = false;
        for (const auto& jbind : jbinds) {
            for (const auto& mbind : mbinds) {
                if (jbind->isEqual(mbind)) {
                    overlap = true;
                    break;
                }
            }
            if (overlap) break;
        }
        if (!overlap) {
            m_fields->m_hasVerified = true;
            if (GameManager::get()->getGameVariable("0128")) PlatformToolbox::hideCursor();
            playStep3();
            return;
        }
        
        auto po = createQuickPopup("Warning", "Your jump keybind seems to be set to the same as your up keybind. If you don't change it, you will not be able to input up.", "Ok", "Keybind Config", [this](auto, bool btn2){
            if (btn2) {
                popup(false);
                // :/
                (this->*(menu_selector(MoreOptionsLayer::onKeybindings)))(nullptr);
            } else {
                m_fields->m_hasVerified = true;
                if (GameManager::get()->getGameVariable("0128")) PlatformToolbox::hideCursor();
                playStep3();
            }
        }, false);
        po->m_noElasticity = !bounce;
        po->show();
    }
    
    void playStep3() {
        if (!enableJoystick || m_fields->m_hasVerified) {
            EditLevelLayer::playStep3();
            return;
        }
        PlatformToolbox::showCursor();
        popup();
    }
};
#endif

void updateVal(GJBaseGameLayer *layer, int id, int val) {
    if (enableJoystick) {
        layer->m_effectManager->updateCountForItem(id, val);
        layer->updateCounters(id, val);
    }
}

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
    if (input.x == 1) {
        layer->queueButton(3, true, false);
    } else if (input.x == -1) {
        layer->queueButton(2, true, false);
    }
    updateVal(layer, 3741, input.x);
    updateVal(layer, 3742, input.y);
}

void JoystickNode::fakePosition() {
    CCPoint pos = getContentSize() / 2 + m_currentInput * getContentWidth() / 2;

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

    m_center->setPosition(pos);
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
        ccDrawColor4B(0, 255, 0, 100);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = 5 * -M_PI / 12;
        auto sweepAngle = 5 * M_PI / 12 - 5 * -M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.push_back(center);

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
        ccDrawColor4B(0, 255, 0, 100);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = 7 * M_PI / 12;
        auto sweepAngle = 11 * M_PI / 12 - M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.push_back(center);

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
        ccDrawColor4B(255, 0, 0, 100);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = M_PI / 12;
        auto sweepAngle = 11 * M_PI / 12 - M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.push_back(center);

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
        ccDrawColor4B(255, 0, 0, 100);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = -11 * M_PI / 12;
        auto sweepAngle = -M_PI / 12 - -11 * M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.push_back(center);

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

class $modify(JSUILayer, UILayer) {

    struct Fields {
        JoystickNode *m_joystickNode;
        bool m_left = false;
        bool m_right = false;
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
            #ifdef GEODE_IS_DESKTOP

            // mine
            m_gameLayer->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
                if (!enableJoystick) return ListenerResult::Propagate;
                if (auto jsLayer = static_cast<JSUILayer*>(this)) {
                    if (auto node = jsLayer->m_fields->m_joystickNode) {
                        auto old = node->m_currentInput;
                        if (event->isDown()) node->m_currentInput.y += 1;
                        else node->m_currentInput.y -= 1;
                        node->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
                        node->fakePosition();
                    }
                }
                return ListenerResult::Stop;
            }, "joystick_up"_spr);
            m_gameLayer->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
                if (!enableJoystick) return ListenerResult::Propagate;
                if (auto jsLayer = static_cast<JSUILayer*>(this)) {
                    if (auto node = jsLayer->m_fields->m_joystickNode) {
                        auto old = node->m_currentInput;
                        if (event->isDown()) node->m_currentInput.y -= 1;
                        else node->m_currentInput.y += 1;
                        node->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
                        node->fakePosition();
                    }
                }
                return ListenerResult::Stop;
            }, "joystick_down"_spr);

            m_gameLayer->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
                if (!enableJoystick) return ListenerResult::Propagate;
                if (auto jsLayer = static_cast<JSUILayer*>(this)) {
                    if (auto node = jsLayer->m_fields->m_joystickNode) {
                        auto old = node->m_currentInput;
                        if (event->isDown()) node->m_currentInput.x -= 1;
                        else node->m_currentInput.x += 1;
                        node->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
                        node->fakePosition();
                    }
                }
                return ListenerResult::Stop;
            }, "robtop.geometry-dash/move-left-p1");
            m_gameLayer->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
                if (!enableJoystick) return ListenerResult::Propagate;
                if (auto jsLayer = static_cast<JSUILayer*>(this)) {
                    if (auto node = jsLayer->m_fields->m_joystickNode) {
                        auto old = node->m_currentInput;
                        if (event->isDown()) node->m_currentInput.x += 1;
                        else node->m_currentInput.x -= 1;
                        node->handleInput(jsLayer->m_gameLayer, node->m_currentInput, old);
                        node->fakePosition();
                    }
                }
                return ListenerResult::Stop;
            }, "robtop.geometry-dash/move-right-p1");
            #endif
        });
        return true;
    }

    void fixVisibility() {
        if (!m_fields->m_joystickNode) {
            return;
        }
        if (!enableJoystick || !m_inPlatformer) {
            m_fields->m_joystickNode->setVisible(false);
            m_fields->m_joystickNode->setTouchEnabled(false);
            return;
        }

        m_fields->m_joystickNode->setVisible(true);
        m_fields->m_joystickNode->setTouchEnabled(true);    

        if (auto p1move = getChildByID("platformer-p1-move-button")) {
            p1move->setPosition({10000, 10000});
        }
    }

    #if !defined(GEODE_IS_WINDOWS) && !defined(GEODE_IS_ARM_MAC)

    void refreshDpad() {
        UILayer::refreshDpad();
        
        fixVisibility();
    }

    #endif
};

class $modify(JSLEL, LevelEditorLayer) {
    void onPlaytest() {
        runChecks(m_objects);
        LevelEditorLayer::onPlaytest();
        if (auto jsLayer = static_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
        }
        updateVal(this, 3740, 1);
    }
};


class $modify(JSPL, PlayLayer) {
    void setupHasCompleted() {
        runChecks(m_objects);
        PlayLayer::setupHasCompleted();
        updateVal(this, 3740, 1);
        if (auto jsLayer = static_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        updateVal(this, 3740, 1);
        if (auto jsLayer = static_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
        }
    }
    #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ARM_MAC)
    void resume() {
        PlayLayer::resume();
        if (auto jsLayer = static_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
        }
    }
    #endif
};