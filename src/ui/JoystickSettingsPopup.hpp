#pragma once

#include <Geode/ui/Popup.hpp>

class JoystickSettingsPopup : public geode::Popup {
protected:
    bool init(LevelEditorLayer* editor);
public:
    static JoystickSettingsPopup* create(LevelEditorLayer* editorlayer);
};