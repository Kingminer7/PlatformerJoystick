#include <Geode/Geode.hpp>
#include "../ui/JoystickNode.hpp"
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>
#include "../Utils.hpp"

using namespace geode::prelude;

#include <Geode/modify/UILayer.hpp>
class $modify(PJUILayer, UILayer) {
	bool init(GJBaseGameLayer* bgl) {
		if (!UILayer::init(bgl)) return false;

        auto js = JoystickNode::create(bgl);

		js->runAction(CallFuncExt::create([bgl, js]{
			js->m_initialized = true;
			if (bgl->m_isEditor) {
				js->updateVis();
				return;
			}
			if (!bgl->m_isPlatformer) {
				js->setCountersEnabled(false);
				js->setEnabled(false);
				js->updateVis();
				return;
			}
			if (legacy::runChecks(bgl->m_objects)) {
				js->setCountersEnabled(true);
			} else {
				js->setCountersEnabled(alpha::level_storage::getSavedValue<bool>(bgl, "joystick-counters", Mod::get()));
				js->setAdvancedCounters(alpha::level_storage::getSavedValue<bool>(bgl, "joystick-advanced", Mod::get()));
			}
			if (!js->isCountersEnabled() && !Mod::get()->getSettingValue<bool>("global")) {
				js->setEnabled(false);
			}
			js->updateVis();
		}));

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

	#if !defined(GEODE_IS_WINDOWS) && !defined(GEODE_IS_ARM_MAC)
	void refreshDpad() {
        UILayer::refreshDpad();
		if (auto node = getChildByType<JoystickNode>(0)) node->updateVis();
    }
	#endif
};

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(JSGJBGL, GJBaseGameLayer) {
    void processQueuedButtons(float dt, bool clear) {
        m_uiLayer->getChildByType<JoystickNode>(0)->updateCounters();
	    GJBaseGameLayer::processQueuedButtons(dt, clear);
    }
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(JSPlayLayer, PlayLayer) {
	void resetLevel() {
        PlayLayer::resetLevel();
		auto js = m_uiLayer->getChildByType<JoystickNode>(0);
		js->setCountersEnabled(alpha::level_storage::getSavedValue<bool>(this, "joystick-counters", Mod::get()));
		js->setAdvancedCounters(alpha::level_storage::getSavedValue<bool>(this, "joystick-advanced", Mod::get()));
		if (!js->isCountersEnabled() && !Mod::get()->getSettingValue<bool>("global")) {
			js->setEnabled(false);
			js->updateVis();
		}
    }

	#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ARM_MAC)
    void resume() {
        PlayLayer::resume();
        if (auto node = m_uiLayer->getChildByType<JoystickNode>(0)) node->updateVis();
    }
    #endif
};
