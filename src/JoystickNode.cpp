#include "JoystickNode.hpp"

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
    if (getScaledContentSize().width / 2 < ccpDistance(getPosition(), touch->getLocation())) {
        return false;
    }

    // m_offset = m_center->getPosition() - convertToNodeSpace(touch->getLocation());

    return true;
}

void handleInput(GJBaseGameLayer *layer, CCPoint input, CCPoint old) {
    if (old.x == 1) {
        layer->handleButton(false, 3, true);
    } else if (old.x == -1) {
        layer->handleButton(false, 2, true);
    }
    if (old.y == 1) {
        layer->handleButton(false, 3, false);
    } else if (old.y == -1) {
        layer->handleButton(false, 2, false);
    }

    if (input.x == 1) {
        layer->handleButton(true, 3, true);
    } else if (input.x == -1) {
        layer->handleButton(true, 2, true);
    }
    if (input.y == 1) {
        layer->handleButton(true, 3, false);
    } else if (input.y == -1) {
        layer->handleButton(true, 2, false);
    }
}

void JoystickNode::ccTouchEnded(CCTouch *touch, CCEvent *event) {
    if (auto pl = PlayLayer::get()) {
        handleInput(pl, {0, 0}, m_currentInput);
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
        if (angle > -M_PI / 3 && angle < M_PI / 3) {
            inp.x = 1;
        } else if (angle > 2 * M_PI / 3 || angle < 2 * -M_PI / 3) {
            inp.x = -1;
        }
    }

    if (std::abs(fromCenter.y) > fromCenter.normalize().y * 15) {
        if (angle > M_PI / 6 && angle < 5 * M_PI / 6) {
            inp.y = 1;
        } else if (angle < -M_PI / 6 && angle > -5 * M_PI / 6) {
            inp.y = -1;
        }
    }

    if (inp != m_currentInput) {
        if (auto pl = PlayLayer::get()) {
            handleInput(pl, inp, m_currentInput);
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