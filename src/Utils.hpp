#pragma once

inline void setCounter(GJBaseGameLayer* bgl, int id, float val) {
    bgl->m_effectManager->updateCountForItem(id, val);
    bgl->updateCounters(id, val);
}

namespace legacy {
    inline GameObject* runChecks(cocos2d::CCArray* objects) {
        for (auto obj : geode::cocos::CCArrayExt<GameObject*>(objects)) {
            if (obj->m_objectID == 914) {
                if (static_cast<TextGameObject*>(obj)->m_text == "--enable-joystick") {
                    return obj;
                }
            }
        }
        return nullptr;
    }
}