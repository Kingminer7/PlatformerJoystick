// ReSharper disable CppHidingFunction
#include "JoystickNode.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

bool enableJoystick = false;

void runChecks(CCArray *objects) {
    enableJoystick = false;
    for (auto obj : CCArrayExt<GameObject*>(objects)) {
        if (obj->m_objectID == 914) {
            if (static_cast<TextGameObject*>(obj)->m_text == "--enable-joystick") {
                enableJoystick = true;
            }
        }
    }
}

void updateVal(GJBaseGameLayer *layer, const int id, const int val) {
    if (enableJoystick) {
        layer->m_effectManager->updateCountForItem(id, val);
        layer->updateCounters(id, val);
    }
}

bool JoystickNode::init() {
    if (!CCNode::init()) return false;
    setContentSize({100, 100});
    setAnchorPoint({.5, .5});

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
    if (!nodeIsVisible(this)) return false;
    const auto r = (convertToNodeSpace(touch->getLocation()) - getScaledContentSize() / 2).getLength();
    if (r > 0 && r < getScaledContentSize().width / 2) {
        ccTouchMoved(touch, event);
        return true;
    }
    return false;
}

void JoystickNode::handleInput(GJBaseGameLayer *layer, const CCPoint& input, const CCPoint& old, const double stamp) const {
        if (input.x != old.x) {
            if (old.x == 1) {
                layer->queueButton(3, false, false, stamp);
            } else if (old.x == -1) {
                layer->queueButton(2, false, false, stamp);
            }
            if (input.x == 1) {
                layer->queueButton(3, true, false, stamp);
            } else if (input.x == -1) {
                layer->queueButton(2, true, false, stamp);
            }
        }
    if (layer->m_player1 && !layer->m_player1->m_controlsDisabled) {
        if (input.x != old.x) updateVal(layer, 3741, input.x);
        if (input.y != old.y) updateVal(layer, 3742, input.y);
    }
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
        handleInput(uil->m_gameLayer, {0, 0}, m_currentInput, touch->getTimestamp());
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
            handleInput(uil->m_gameLayer, inp, m_currentInput, touch->getTimestamp());
        }
    }
    m_currentInput = inp;

    m_center->setPosition(pos);
}

void JoystickNode::ccTouchCancelled(CCTouch *touch, CCEvent *event) {
    ccTouchEnded(touch, event);
}

void JoystickNode::onEnter()
{
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, -512, true);
}

void JoystickNode::onExit()
{
    CCTouchDispatcher::get()->removeDelegate(this);
    CCNode::onExit();
}

void JoystickNode::draw() {
    CCNode::draw();

    #if defined(DEBUG_BUILD) && !defined(GITHUB_ACTIONS)
    {
        ccDrawColor4B(0, 255, 0, 255);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = 5 * -M_PI / 12;
        auto sweepAngle = 5 * M_PI / 12 - 5 * -M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.emplace_back(center);

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.emplace_back(
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            );
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(0, 1, 0, 0.15f));
    }

    {
        ccDrawColor4B(0, 255, 0, 255);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = 7 * M_PI / 12;
        auto sweepAngle = 11 * M_PI / 12 - M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.emplace_back(center);

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.emplace_back(
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            );
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(0, 1, 0, 0.15f));
    }

    {
        ccDrawColor4B(255, 0, 0, 255);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = M_PI / 12;
        auto sweepAngle = 11 * M_PI / 12 - M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.emplace_back(center);

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.emplace_back(
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            );
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(1, 0, 0, 0.15f));
    }

    {
        ccDrawColor4B(255, 0, 0, 255);
        auto center = getScaledContentSize() / 2;
        auto radius = getScaledContentWidth() / 2;
        auto startAngle = -11 * M_PI / 12;
        auto sweepAngle = -M_PI / 12 - -11 * M_PI / 12;
        int segments = 50;

        std::vector<CCPoint> vertices;
        vertices.emplace_back(center);

        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (sweepAngle * i / segments);
            vertices.emplace_back(
                center.width + radius * cos(angle),
                center.height + radius * sin(angle)
            );
        }

        ccDrawSolidPoly(vertices.data(), vertices.size(), ccc4f(1, 0, 0, 0.15f));
    }
    #endif
}

class $modify(JSUILayer, UILayer) {

    struct Fields {
        JoystickNode *m_joystickNode = nullptr;
        CCPoint m_keyboardPos = {0, 0};
    };

    bool init(GJBaseGameLayer *gjbgl) {
        if (!UILayer::init(gjbgl)) return false;
        queueInMainThread([this, gjbgl]{
            m_fields->m_joystickNode = JoystickNode::create();
            m_fields->m_joystickNode->m_twoPlayer = gjbgl->m_level->m_twoPlayerMode;
            addChildAtPosition(m_fields->m_joystickNode, Anchor::BottomLeft, {75, 75}, false);

            fixVisibility();

            m_gameLayer->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "up-bind"), [this](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
                if (repeat || !enableJoystick) return ListenerResult::Propagate;
                if (auto node = m_fields->m_joystickNode) {
                    auto old = node->m_currentInput;
                    if (down) m_fields->m_keyboardPos.y += 1;
                    else m_fields->m_keyboardPos.y -= 1;
                    node->m_currentInput.y = std::clamp(m_fields->m_keyboardPos.y, -1.f, 1.f);
                    node->handleInput(m_gameLayer, node->m_currentInput, old, stamp);
                    node->fakePosition();
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            }, -100);

            m_gameLayer->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "down-bind"), [this](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
                if (repeat || !enableJoystick) return ListenerResult::Propagate;
                if (auto node = m_fields->m_joystickNode) {
                    auto old = node->m_currentInput;
                    if (down) m_fields->m_keyboardPos.y -= 1;
                    else m_fields->m_keyboardPos.y += 1;
                    node->m_currentInput.y = std::clamp(m_fields->m_keyboardPos.y, -1.f, 1.f);
                    node->handleInput(m_gameLayer, node->m_currentInput, old, stamp);
                    node->fakePosition();
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            }, -100);
            m_gameLayer->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "right-bind"), [this](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
                if (repeat || !enableJoystick) return ListenerResult::Propagate;
                if (auto node = m_fields->m_joystickNode) {
                    auto old = node->m_currentInput;
                    if (down) m_fields->m_keyboardPos.x += 1;
                    else m_fields->m_keyboardPos.x -= 1;
                    node->m_currentInput.x = std::clamp(m_fields->m_keyboardPos.x, -1.f, 1.f);
                    node->handleInput(m_gameLayer, node->m_currentInput, old, stamp);
                    node->fakePosition();
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            }, -100);
            m_gameLayer->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "left-bind"), [this](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
                if (repeat || !enableJoystick) return ListenerResult::Propagate;
                if (auto node = m_fields->m_joystickNode) {
                    auto old = node->m_currentInput;
                    if (down) m_fields->m_keyboardPos.x -= 1;
                    else m_fields->m_keyboardPos.x += 1;
                    node->m_currentInput.x = std::clamp(m_fields->m_keyboardPos.x, -1.f, 1.f);
                    node->handleInput(m_gameLayer, node->m_currentInput, old, stamp);
                    node->fakePosition();
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            }, -100);
        });
        return true;
    }

    void fixVisibility() {
        if (!m_fields->m_joystickNode) {
            return;
        }
        if (!enableJoystick || !m_inPlatformer) {
            m_fields->m_joystickNode->setVisible(false);
            //m_fields->m_joystickNode->setTouchEnabled(false);
            return;
        }

        m_fields->m_joystickNode->setVisible(true);
        //m_fields->m_joystickNode->setTouchEnabled(true);

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
    struct Fields {
        bool m_hasVerified = false;
    };
    void onPlaytest() {
        if (!m_objects) return LevelEditorLayer::onPlaytest();
        runChecks(m_objects);
        LevelEditorLayer::onPlaytest();
        if (auto jsLayer = reinterpret_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
            if (auto node = jsLayer->m_fields->m_joystickNode) {
                node->m_currentInput = CCPoint{0, 0};
            }
        }
        updateVal(this, 3740, 1);
    }
};

class $modify(JSPL, PlayLayer) {
    struct Fields {
        bool m_hasVerified = false;
    };

    void setupHasCompleted() {
        if (!m_objects) return PlayLayer::setupHasCompleted();
        runChecks(m_objects);
        PlayLayer::setupHasCompleted();
        updateVal(this, 3740, 1);
        if (auto jsLayer = reinterpret_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        updateVal(this, 3740, 1);
        if (auto jsLayer = reinterpret_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
        }
    }
    #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ARM_MAC)
    void resume() {
        PlayLayer::resume();
        if (auto jsLayer = reinterpret_cast<JSUILayer*>(m_uiLayer)) {
            jsLayer->fixVisibility();
        }
    }
    #endif
};

class $modify(LTLSL, LevelSettingsLayer) {
    
    struct Fields {
        TextGameObject* m_obj = nullptr;
        CCMenuItemToggler* m_toggle = nullptr;
    };

    bool init(LevelSettingsObject* settings, LevelEditorLayer* editor) {
        if (!LevelSettingsLayer::init(settings, editor)) return false;
        if (!editor || !editor->m_objects) return true;

        for (auto obj : CCArrayExt<GameObject*>(editor->m_objects)) {
            if (obj->m_objectID ==914) {
                const auto txt = static_cast<TextGameObject*>(obj);
                if (txt->m_text == "--enable-joystick") {
                    m_fields->m_obj = txt;
                }
            } 
        }

        CCMenuItemToggler* toggler = CCMenuItemExt::createTogglerWithStandardSprites(.7f, [this, editor](auto){
            if (m_fields->m_obj) {
                editor->m_editorUI->deselectObject(m_fields->m_obj);
                editor->m_editorUI->deleteObject(m_fields->m_obj, false);
                m_fields->m_obj = nullptr;
            } else {
                auto* obj = static_cast<TextGameObject*>(editor->m_editorUI->createObject(914, {0, 0}));
                obj->m_isHide = true;
                obj->updateTextObject("--enable-joystick", false);
                m_fields->m_obj = obj;
            }
        });
        toggler->toggle(m_fields->m_obj != nullptr);
        toggler->setCascadeOpacityEnabled(true);
        toggler->setEnabled(m_settingsObject->m_platformerMode);
        toggler->setOpacity(m_settingsObject->m_platformerMode ? 255 : 100);
        m_buttonMenu->addChildAtPosition(toggler, Anchor::BottomLeft, {-185, -120}, false);
        m_fields->m_toggle = toggler;
        CCLabelBMFont* lab = CCLabelBMFont::create("Joystick:", "goldFont.fnt");
        lab->setScale(.6f);
        m_mainLayer->addChildAtPosition(lab, Anchor::Center, {-185, -95}, false);
        return true;
    }

    void updateGameplayModeButtons() {
        LevelSettingsLayer::updateGameplayModeButtons();
        auto toggler = m_fields->m_toggle;
        if (!toggler) return;
        toggler->setEnabled(m_settingsObject->m_platformerMode);
        toggler->setOpacity(m_settingsObject->m_platformerMode ? 255 : 100);
    }
    
};

class $modify(JSPO, PlayerObject) {
    void disablePlayerControls() {
        PlayerObject::disablePlayerControls();
        if (m_gameLayer->m_player1 != this) return;
        if (!enableJoystick || !UILayer::get()) return;
        updateVal(m_gameLayer, 3741, 0);
        updateVal(m_gameLayer, 3742, 0);
    }

    void enablePlayerControls() {
        PlayerObject::enablePlayerControls();
        if (m_gameLayer->m_player1 != this) return;
        if (!enableJoystick || !UILayer::get()) return;
        auto js = UILayer::get()->getChildByType<JoystickNode>(0);
        if (!js) return;
        updateVal(m_gameLayer, 3741, js->m_currentInput.x);
        updateVal(m_gameLayer, 3742, js->m_currentInput.y);
    }
};