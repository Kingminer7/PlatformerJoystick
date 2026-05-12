#include <Geode/Geode.hpp>
#include "../ui/JoystickNode.hpp"

using namespace geode::prelude;

#include <Geode/modify/UILayer.hpp>
class $modify(PJUILayer, UILayer) {
	bool init(GJBaseGameLayer* bgl) {
		if (!UILayer::init(bgl)) return false;

        auto js = JoystickNode::create();
        js->setZOrder(100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "up-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled()) return ListenerResult::Propagate;
			if (js->m_kbUp == down) return ListenerResult::Propagate;
			js->m_kbUp = down;
			if (down) js->m_input.y += 1;
			else js->m_input.y -= 1;
			return ListenerResult::Stop;
		}, -100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "down-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled()) return ListenerResult::Propagate;
			if (js->m_kbDown == down) return ListenerResult::Propagate;
			js->m_kbDown = down;
			if (down) js->m_input.y -= 1;
			else js->m_input.y += 1;
			return ListenerResult::Stop;
		}, -100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "left-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled()) return ListenerResult::Propagate;
			if (js->m_kbLeft == down) return ListenerResult::Propagate;
			js->m_kbLeft = down;
			if (down) js->m_input.x -= 1;
			else js->m_input.x += 1;
			return ListenerResult::Stop;
		}, -100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "right-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled()) return ListenerResult::Propagate;
			if (js->m_kbRight == down) return ListenerResult::Propagate;
			js->m_kbRight = down;
			if (down) js->m_input.x += 1;
			else js->m_input.x -= 1;
			return ListenerResult::Stop;
		}, -100);
		addChildAtPosition(js, Anchor::BottomLeft, {100, 80}, false);

		return true;
	}
};