#include "../ui/JoystickNode.hpp"
#include <Geode/Geode.hpp>
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace geode::prelude;

// Idea from Better Touch Prio
class JoystickTouchListener : public CCLayer {
public:
  JoystickNode *m_node;

  static JoystickTouchListener* create(JoystickNode*  node) {
    auto ret = new JoystickTouchListener;
    if (ret->init(node)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }

  bool init(JoystickNode *node) {
    if (!CCLayer::init())
      return false;

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
  bool init(LevelEditorLayer *lel) {
    if (!EditorUI::init(lel))
      return false;

    auto node = lel->m_uiLayer->getChildByType<JoystickNode>(0);
    auto listener = JoystickTouchListener::create(node);
	this->addChild(JoystickTouchListener::create(node), -1000); 

    return true;
  }
};