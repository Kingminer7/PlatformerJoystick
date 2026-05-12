#include <Geode/Geode.hpp>
#include "../ui/JoystickNode.hpp"
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace geode::prelude;

#include <Geode/modify/UILayer.hpp>
class $modify(PJUILayer, UILayer) {
	bool init(GJBaseGameLayer* bgl) {
		if (!UILayer::init(bgl)) return false;

        auto js = JoystickNode::create(bgl);
		js->setCountersEnabled(alpha::level_storage::getSavedValue<bool>(bgl, "joystick-counters", Mod::get()));
		js->setAdvancedCounters(alpha::level_storage::getSavedValue<bool>(bgl, "joystick-advanced", Mod::get()));
        js->setZOrder(100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "up-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled() || !nodeIsVisible(js) || m_gameLayer->m_playbackMode == PlaybackMode::Paused) return ListenerResult::Propagate;
			js->setInput(CCPoint{js->m_input.x, down ? 1.f : 0.f}, stamp);
			js->m_kbUp = down;
			js->updateKeyboard(stamp);
			return ListenerResult::Stop;
		}, -100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "down-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled() || !nodeIsVisible(js) || m_gameLayer->m_playbackMode == PlaybackMode::Paused) return ListenerResult::Propagate;
			js->setInput(CCPoint{js->m_input.x, down ? -1.f : 0.f}, stamp);
			js->m_kbDown = down;
			js->updateKeyboard(stamp);
			return ListenerResult::Stop;
		}, -100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "left-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled() || !nodeIsVisible(js) || m_gameLayer->m_playbackMode == PlaybackMode::Paused) return ListenerResult::Propagate;
			js->setInput(CCPoint{down ? -1.f : 0.f, js->m_input.y}, stamp);
			js->m_kbLeft = down;
			js->updateKeyboard(stamp);
			return ListenerResult::Stop;
		}, -100);
		js->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "right-bind"), [this, js](Keybind const& keybind, const bool down, const bool repeat, const double stamp) {
			if (repeat || !js->isEnabled() || !nodeIsVisible(js) || m_gameLayer->m_playbackMode == PlaybackMode::Paused) return ListenerResult::Propagate;
			js->m_kbRight = down;
			js->updateKeyboard(stamp);
			return ListenerResult::Stop;
		}, -100);
		addChildAtPosition(js, Anchor::BottomLeft, {100, 80}, false);

		return true;
	}
};