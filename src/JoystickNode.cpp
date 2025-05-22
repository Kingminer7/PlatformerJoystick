#include "JoystickNode.hpp"
#include "utils.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

bool enableCounters = false;

class $modify(JSPL, PlayLayer) {
    void setupHasCompleted() {
        enableCounters = false;
        for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
            if (obj->m_objectID == 914) {
                if (auto txt = static_cast<TextGameObject*>(obj); txt && txt->m_text == "--joystick-counters") {
                    //removeObject(obj, true);
                    enableCounters = true;
                }
            }
        }
        PlayLayer::setupHasCompleted();
    }
};

class $modify(JSLEL, LevelEditorLayer) {
    void onPlaytest() {
        enableCounters = false;
        for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
            if (obj->m_objectID == 914) {
                if (auto txt = static_cast<TextGameObject*>(obj); txt && txt->m_text == "--joystick-counters") {
                    //removeObject(obj, true);
                    enableCounters = true;
                }
            }
        }
        LevelEditorLayer::onPlaytest();
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
    return ccpDistance(getPosition(), touch->getLocation()) <= getScaledContentSize().width / 2;
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
    if (enableCounters) layer->updateCounters(3740, input.x);
    if (!fastGetSetting<"disable-updown", bool>() || m_twoPlayer) {
        if (input.y == 1) {
            layer->queueButton(3, true, true);
        } else if (input.y == -1) {
            layer->queueButton(2, true, true);
        }
        if (enableCounters) layer->updateCounters(3741, input.y);
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

    if (std::abs(fromCenter.x) > fromCenter.normalize().x * 15) {
        if (angle > 5 * -M_PI / 12 && angle < 5 * M_PI / 12) {
            inp.x = 1;
        } else if (angle > 7 * M_PI / 12 || angle < 7 * -M_PI / 12) {
            inp.x = -1;
        }
    }

    if (std::abs(fromCenter.y) > fromCenter.normalize().y * 15) {
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
