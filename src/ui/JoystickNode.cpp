#include "JoystickNode.hpp"
#include <Geode/Geode.hpp>
#include <numbers>
#include <geode.devtools/include/API.hpp>

using namespace geode::prelude;

bool JoystickNode::init() {
    if (!CCLayer::init()) return false;

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
    auto pos = (m_input.getLength() > 1 ? m_input.normalize() : m_input) * CCPoint{2.f/3, 2.f/3};
    ccDrawFilledCircle(getContentSize() / 2 * (pos + CCPoint{1.f, 1.f}), getContentWidth() / 6, 0, 64);
}

bool JoystickNode::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto local = convertTouchToNodeSpace(touch) - getContentSize() / 2;
    if (local.getLength() < getContentWidth() / 2) {
        m_input = local / getContentWidth() * 2;
        return true;
    }
    return false;
}

void JoystickNode::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto local = convertTouchToNodeSpace(touch) - getContentSize() / 2;
    m_input = CCPoint{std::clamp(local.x / getContentWidth() * 2, -1.f, 1.f), std::clamp(local.y / getContentWidth() * 2, -1.f, 1.f)};
}

void JoystickNode::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_input = CCPoint{0, 0};
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

JoystickNode* JoystickNode::create() {
    auto ret = new JoystickNode();
    if (!ret->init()) return delete ret, ret;
    return ret->autorelease(), ret;
}

$on_mod(Loaded) {
    devtools::waitForDevTools([](){
        devtools::registerNode<JoystickNode>([](JoystickNode* node){
            devtools::property("Input", node->m_input);
        });
    });
}