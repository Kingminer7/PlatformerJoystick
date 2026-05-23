#include "../ui/JoystickNode.hpp"
#include "../ui/JoystickSettingsPopup.hpp"
#include "../Utils.hpp"
#include <Geode/Geode.hpp>
#include <Geode/ui/Button.hpp>
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace geode::prelude;

// Idea from Better Touch Prio
class JoystickTouchListener : public CCLayer {
public:
    JoystickNode *m_node;

    static JoystickTouchListener *create(JoystickNode *node) {
        auto ret = new JoystickTouchListener;
        if (ret->init(node)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool init(JoystickNode *node) {
        if (!CCLayer::init()) return false;

        m_node = node;

        this->setTouchEnabled(true);
        this->setID("joystick-touch-listener"_spr);

        return true;
    }

    void registerWithTouchDispatcher() override {
        CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
    }

    bool ccTouchBegan(CCTouch *touch, CCEvent *event) override {
        return m_node->ccTouchBegan(touch, event);
    }

    void ccTouchMoved(CCTouch *touch, CCEvent *event) override {
        m_node->ccTouchMoved(touch, event);
    }

    void ccTouchEnded(CCTouch *touch, CCEvent *event) override {
        m_node->ccTouchEnded(touch, event);
    }

    void ccTouchCancelled(CCTouch *touch, CCEvent *event) override {
        m_node->ccTouchCancelled(touch, event);
    }
};

#include <Geode/modify/EditorUI.hpp>
class $modify(PJEditorUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        auto node = lel->m_uiLayer->getChildByType<JoystickNode>(0);
        auto listener = JoystickTouchListener::create(node);
        this->addChild(JoystickTouchListener::create(node), 1000);

        if (auto ret = legacy::runChecks(lel->m_objects)) {
            deselectObject(ret);
            deleteObject(ret, false);   
			alpha::level_storage::setSavedValue(lel, "joystick-counters", true);

            // Lore Accurate FLAlertLayer
            auto alert = FLAlertLayer::create(
                                                      "Notice", 
        "Platformer Joystick has changed how it stores if the joystick is enabled in levels. <cr>Future versions of the mod may not support older levels.</c>\n\n"
            "<cg>Your level has automatically been migrated.</c> <co>You should update the level if it's already uploaded on the GD servers.</c>",
                                                        "Ok"
            );
            alert->m_scene = this;
            alert->m_noElasticity = true;
            alert->show();
        }

        return true;
    }
};

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(PJLevelEditorLayer, LevelEditorLayer) {
    void onPlaytest() {
        LevelEditorLayer::onPlaytest();
        auto js = m_uiLayer->getChildByType<JoystickNode>(0);
		js->m_initialized = true;
        if (!m_levelSettings->m_platformerMode) {
			js->setEnabled(false);
			js->updateVis();
			js->setCountersEnabled(false);
            return;
        }
        js->setEnabled(true);
		js->updateVis();
        js->setCountersEnabled(alpha::level_storage::getSavedValue<bool>(this, "joystick-counters", Mod::get()));
		js->setAdvancedCounters(alpha::level_storage::getSavedValue<bool>(this, "joystick-advanced", Mod::get()));
		if (js->isCountersEnabled()) {
            setCounter(this, 3740, 1);
        } else if (!Mod::get()->getSettingValue<bool>("global")) {
			js->setEnabled(false);
			js->updateVis();
		}
    }
};

#include <Geode/modify/LevelSettingsLayer.hpp>
class $modify(PJLevelSettingsLayer, LevelSettingsLayer) {
	bool init(LevelSettingsObject* object, LevelEditorLayer* layer) {
		if (!LevelSettingsLayer::init(object, layer)) return false;

        auto btn = Button::createWithNode(CircleButtonSprite::createWithSprite("joystick.png"_spr), [layer](auto){
			JoystickSettingsPopup::create(layer)->show();
		});
		m_mainLayer->addChildAtPosition(btn, Anchor::Center, CCPoint{185, -115}, false);

		return true;
	}
};