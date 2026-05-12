#include <Geode/Geode.hpp>
#include <Geode/ui/Button.hpp>
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelSettingsLayer.hpp>
class $modify(PJLevelSettingsLayer, LevelSettingsLayer) {
	bool init(LevelSettingsObject* object, LevelEditorLayer* layer) {
		if (!LevelSettingsLayer::init(object, layer)) return false;

        auto btn = Button::createWithNode(CircleButtonSprite::createWithSpriteFrameName("joystick.png"_spr), [](auto){
			FLAlertLayer::create("H", "i", "!")->show();
		});
		addChildAtPosition(btn, Anchor::Center, CCPoint{185, -115}, false);

		return true;
	}
};