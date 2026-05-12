#include "JoystickNode.hpp"
#include <Geode/Geode.hpp>
#include <numbers>
#include <geode.devtools/include/API.hpp>

using namespace geode::prelude;

bool JoystickNode::init(GJBGL* bgl) {
    if (!CCLayer::init()) return false;

    m_bgl = bgl;

    setTouchEnabled(true);
    setTouchMode(ccTouchesMode::kCCTouchesOneByOne);
    setContentSize({100, 100});
    setAnchorPoint({.5, .5});
    ignoreAnchorPointForPosition(false);
    
    return true;
}

void drawFilledSegment(cocos2d::CCPoint const& center, float radius, float startAngle, float endAngle, unsigned int segments, ccColor4B const& color) {
    if (radius <= 0.0f) return;
    if (segments < 2) segments = 2;

    float sweep = endAngle - startAngle;

    unsigned int usedSegs = std::max<unsigned int>(2, ceil(segments * abs(sweep)));

    std::vector<CCPoint> verts;
    verts.push_back(center);

    for (unsigned int i = 0; i <= usedSegs; ++i) {
        float t = (float)i / usedSegs;
        float a = (startAngle + t * sweep);
        verts.push_back(ccp(center.x + cosf(a) * radius, center.y + sinf(a) * radius));
    }

    ccDrawSolidPoly(&verts[0], (int)verts.size(), to4F(color));
}

void JoystickNode::draw() {
    CCLayerRGBA::draw();
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ccDrawColor4B(ccColor4B{_displayedColor.r, _displayedColor.g, _displayedColor.b, (GLubyte) (105 * _displayedOpacity / 255)});
    ccDrawFilledCircle(getContentSize() / 2, getContentWidth() / 2, 0, 64);
    #ifdef DEBUG_BUILD
    // R
    drawFilledSegment(getContentSize() / 2, getContentWidth() / 2, -std::numbers::pi / 3, std::numbers::pi / 3, 16, ccColor4B(255, 0, 0, 120 * _displayedOpacity / 255.f));
    // L
    drawFilledSegment(getContentSize() / 2, getContentWidth() / 2, 2 * std::numbers::pi / 3, 4 * std::numbers::pi / 3, 16, ccColor4B(255, 0, 0, 120 * _displayedOpacity / 255.f));
    // T
    drawFilledSegment(getContentSize() / 2, getContentWidth() / 2, std::numbers::pi / 6, 5 * std::numbers::pi / 6, 16, ccColor4B(0, 255, 0, 120 * _displayedOpacity / 255.f));
    // B
    drawFilledSegment(getContentSize() / 2, getContentWidth() / 2, 7 * std::numbers::pi / 6, 11 * std::numbers::pi / 6, 16, ccColor4B(0, 255, 0, 120 * _displayedOpacity / 255.f));
    #endif

    ccDrawColor4B(ccColor4B{0, 0, 0, (GLubyte) (226 * _displayedOpacity / 255)});
    auto pos = (m_input.getLength() > 1 ? m_input.normalize() : m_input);
    ccDrawFilledCircle(getContentSize() / 2 * (pos * 2 / 3 + CCPoint{1.f, 1.f}), getContentWidth() / 6, 0, 64);
}

bool JoystickNode::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto local = convertTouchToNodeSpace(touch) - getContentSize() / 2;
    if (local.getLength() < getContentWidth() / 2) {
        setInput(local / getContentWidth() * 3, touch->getTimestamp());
        return true;
    }
    return false;
}

void JoystickNode::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto local = convertTouchToNodeSpace(touch) - getContentSize() / 2;
    setInput(local / getContentWidth() * 3, touch->getTimestamp());
}

void JoystickNode::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    setInput(CCPoint{0, 0}, touch->getTimestamp());
}

void JoystickNode::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

void JoystickNode::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool JoystickNode::isEnabled() {
    return m_enabled;
}

void JoystickNode::setCountersEnabled(bool enabled) {
    m_counters = enabled;
}

bool JoystickNode::isCountersEnabled() {
    return m_counters;
}

void JoystickNode::setAdvancedCounters(bool advanced) {
    m_advCounters = advanced;
}

bool JoystickNode::isAdvancedCounters() {
    return m_advCounters;
}

void JoystickNode::setInput(const CCPoint& where, double timestamp) {
    auto old = m_input;
    if (where.getLength() > 1) m_input = where.normalize();
    else m_input = where;

    if (m_input.x > .3 && old.x <= 0.3) { // r
        if (old.x < -0.3) m_bgl->queueButton(2, false, false, timestamp);
        m_bgl->queueButton(3, true, false, timestamp);
    } else if (m_input.x < -.3 && old.x >= -0.3) { // l
        if (old.x > 0.3) m_bgl->queueButton(3, false, false, timestamp);
        m_bgl->queueButton(2, true, false, timestamp);
    } else if (m_input.x > -0.3 && m_input.x < 0.3) {
        if (old.x > 0.3) m_bgl->queueButton(3, false, false, timestamp);
        else if (old.x < -0.3) m_bgl->queueButton(2, false, false, timestamp);
    }

    updateCounters();
}

void JoystickNode::updateKeyboard(double timestamp) {
    CCPoint inp{};
    if (m_kbRight) inp.x += 1;
    if (m_kbLeft) inp.x -= 1;
    if (m_kbUp) inp.y += 1;
    if (m_kbDown) inp.y -= 1;
    setInput(inp, timestamp);
}

inline void setCounter(GJBaseGameLayer* bgl, int id, float val) {
    bgl->m_effectManager->updateCountForItem(id, val);
    bgl->updateCounters(id, val);
}

void JoystickNode::updateCounters() {
    if (!m_counters) return;
    if (m_advCounters) {
        if (m_bgl->m_effectManager->countForItem(3741) != round(m_input.x * 100)) setCounter(m_bgl, 3741, round(m_input.x * 100));
        if (m_bgl->m_effectManager->countForItem(3742) != round(m_input.y * 100)) setCounter(m_bgl, 3742, round(m_input.y * 100));
    } else {
        if (m_input.x > 0.3 && m_bgl->m_effectManager->countForItem(3741) != 1) setCounter(m_bgl, 3741, 1);
        else if (m_input.x < -0.3 && m_bgl->m_effectManager->countForItem(3741) != -1) setCounter(m_bgl, 3741, -1);
        else if (m_input.x > -0.3 && m_input.x < 0.3 && m_bgl->m_effectManager->countForItem(3741) != 0) setCounter(m_bgl, 3741, 0);
        
        if (m_input.y > 0.3 && m_bgl->m_effectManager->countForItem(3742) != 1) setCounter(m_bgl, 3742, 1);
        else if (m_input.y < -0.3 && m_bgl->m_effectManager->countForItem(3742) != -1) setCounter(m_bgl, 3742, -1);
        else if (m_input.y > -0.3 && m_input.y < 0.3 && m_bgl->m_effectManager->countForItem(3742) != 0) setCounter(m_bgl, 3742, 0);
    }
}

JoystickNode* JoystickNode::create(GJBGL* bgl) {
    auto ret = new JoystickNode();
    if (!ret->init(bgl)) return delete ret, ret;
    return ret->autorelease(), ret;
}

$on_mod(Loaded) {
    devtools::waitForDevTools([](){
        devtools::registerNode<JoystickNode>([](JoystickNode* node){
            devtools::property("Input", node->m_input);
        });
    });
}