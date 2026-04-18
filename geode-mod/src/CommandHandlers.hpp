#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

// Command handlers for object manipulation
class ObjectCommandHandler {
public:
    // Create object with groups
    static GameObject* createObjectWithGroups(EditorUI* editorUI, int objID, float x, float y, const std::vector<int>& groups) {
        if (!editorUI) return nullptr;
        
        auto obj = editorUI->createObject(objID, {x, y});
        if (!obj) return nullptr;
        
        // Set groups
        for (int groupID : groups) {
            obj->addToGroup(groupID);
        }
        
        return obj;
    }
    
    // Set object color channel
    static void setObjectColor(GameObject* obj, int colorChannel) {
        if (!obj) return;
        
        // Set color using setObjectColor method
        obj->setObjectColor(GameManager::sharedState()->colorForIdx(colorChannel));
    }
    
    // Move object to new position
    static bool moveObject(GameObject* obj, float newX, float newY) {
        if (!obj) return false;
        
        obj->setPosition({newX, newY});
        
        return true;
    }
    
    // Delete object from editor
    static bool deleteObject(EditorUI* editorUI, GameObject* obj) {
        if (!editorUI || !obj) return false;
        
        auto editor = editorUI->m_editorLayer;
        if (!editor) return false;
        
        // Remove from all arrays
        editor->removeObject(obj, true);
        
        return true;
    }
    
    // Find objects by group
    static std::vector<GameObject*> findObjectsByGroup(LevelEditorLayer* editor, int groupID) {
        std::vector<GameObject*> result;
        if (!editor) return result;
        
        auto objects = editor->m_objects;
        if (!objects) return result;
        
        for (int i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (obj) {
                // Check if object is in group
                for (int j = 0; j < obj->m_groups->size(); j++) {
                    if (obj->m_groups->at(j) == groupID) {
                        result.push_back(obj);
                        break;
                    }
                }
            }
        }
        
        return result;
    }
};

// Trigger command handlers
class TriggerCommandHandler {
public:
    // Create Move Trigger with properties
    static GameObject* createMoveTrigger(EditorUI* editorUI, float x, float y, int targetGroup, 
                                         float moveX, float moveY, float duration, int easing) {
        if (!editorUI) return nullptr;
        
        // Move trigger ID = 901
        auto trigger = editorUI->createObject(901, {x, y});
        if (!trigger) return nullptr;
        
        // Try to cast to EffectGameObject and set properties
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_moveX = moveX;
            effectObj->m_moveY = moveY;
            effectObj->m_duration = duration;
            effectObj->m_easing = easing;
        }
        
        return trigger;
    }
    
    // Create Alpha Trigger with properties
    static GameObject* createAlphaTrigger(EditorUI* editorUI, float x, float y, int targetGroup,
                                          float opacity, float duration, int easing) {
        if (!editorUI) return nullptr;
        
        // Alpha trigger ID = 1007
        auto trigger = editorUI->createObject(1007, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_opacity = opacity;
            effectObj->m_duration = duration;
            effectObj->m_easing = easing;
        }
        
        return trigger;
    }
    
    // Create Rotate Trigger with properties
    static GameObject* createRotateTrigger(EditorUI* editorUI, float x, float y, int targetGroup,
                                           float degrees, float duration, int easing, int times, bool lockRotation) {
        if (!editorUI) return nullptr;
        
        // Rotate trigger ID = 1346
        auto trigger = editorUI->createObject(1346, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_rotationDegrees = degrees;
            effectObj->m_duration = duration;
            effectObj->m_easing = easing;
            effectObj->m_times360 = times;
            effectObj->m_lockObjectRotation = lockRotation;
        }
        
        return trigger;
    }
    
    // Create Scale Trigger with properties
    static GameObject* createScaleTrigger(EditorUI* editorUI, float x, float y, int targetGroup,
                                          float scaleX, float scaleY, float duration, int easing) {
        if (!editorUI) return nullptr;
        
        // Scale trigger ID = 2067
        auto trigger = editorUI->createObject(2067, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_scaleX = scaleX;
            effectObj->m_scaleY = scaleY;
            effectObj->m_duration = duration;
            effectObj->m_easing = easing;
        }
        
        return trigger;
    }
    
    // Create Color Trigger with properties
    static GameObject* createColorTrigger(EditorUI* editorUI, float x, float y, int targetChannel,
                                          int r, int g, int b, float duration, int easing, float opacity) {
        if (!editorUI) return nullptr;
        
        // Color trigger ID = 899
        auto trigger = editorUI->createObject(899, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetColorID = targetChannel;
            effectObj->m_color.r = r;
            effectObj->m_color.g = g;
            effectObj->m_color.b = b;
            effectObj->m_duration = duration;
            effectObj->m_easing = easing;
            effectObj->m_opacity = opacity;
        }
        
        return trigger;
    }
    
    // Create Pulse Trigger with properties
    static GameObject* createPulseTrigger(EditorUI* editorUI, float x, float y, int targetGroup,
                                          int r, int g, int b, float fadeIn, float hold, float fadeOut) {
        if (!editorUI) return nullptr;
        
        // Pulse trigger ID = 1006
        auto trigger = editorUI->createObject(1006, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_color.r = r;
            effectObj->m_color.g = g;
            effectObj->m_color.b = b;
            effectObj->m_fadeInTime = fadeIn;
            effectObj->m_holdTime = hold;
            effectObj->m_fadeOutTime = fadeOut;
        }
        
        return trigger;
    }
    
    // Create Spawn Trigger with properties
    static GameObject* createSpawnTrigger(EditorUI* editorUI, float x, float y, int targetGroup, float delay) {
        if (!editorUI) return nullptr;
        
        // Spawn trigger ID = 1268
        auto trigger = editorUI->createObject(1268, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_spawnDelay = delay;
        }
        
        return trigger;
    }
    
    // Create Toggle Trigger with properties
    static GameObject* createToggleTrigger(EditorUI* editorUI, float x, float y, int targetGroup, bool activate) {
        if (!editorUI) return nullptr;
        
        // Toggle trigger ID = 1049
        auto trigger = editorUI->createObject(1049, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_activateGroup = activate;
        }
        
        return trigger;
    }
    
    // Create Stop Trigger with properties
    static GameObject* createStopTrigger(EditorUI* editorUI, float x, float y, int targetGroup) {
        if (!editorUI) return nullptr;
        
        // Stop trigger ID = 1616
        auto trigger = editorUI->createObject(1616, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
        }
        
        return trigger;
    }
    
    // Create Follow Trigger with properties
    static GameObject* createFollowTrigger(EditorUI* editorUI, float x, float y, int targetGroup, 
                                           int followGroup, float xMod, float yMod, float duration) {
        if (!editorUI) return nullptr;
        
        // Follow trigger ID = 1347
        auto trigger = editorUI->createObject(1347, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_followGroupID = followGroup;
            effectObj->m_followXMod = xMod;
            effectObj->m_followYMod = yMod;
            effectObj->m_duration = duration;
        }
        
        return trigger;
    }
};
