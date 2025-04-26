#include <Geode/Geode.hpp>
#include "JoystickNode.hpp"

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
class $modify(CML, PlayLayer) {
    bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto node = JoystickNode::create();
        addChildAtPosition(node, Anchor::BottomLeft, {50, 50}, false);

        return true;
    }
};