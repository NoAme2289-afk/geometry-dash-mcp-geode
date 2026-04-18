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
            if (obj && obj->m_groups) {
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
    
    // Find object by position (within tolerance)
    static GameObject* findObjectAtPosition(LevelEditorLayer* editor, float x, float y, float tolerance = 10.0f) {
        if (!editor) return nullptr;
        
        auto objects = editor->m_objects;
        if (!objects) return nullptr;
        
        for (int i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (obj) {
                auto pos = obj->getPosition();
                float dx = pos.x - x;
                float dy = pos.y - y;
                float dist = sqrt(dx*dx + dy*dy);
                
                if (dist <= tolerance) {
                    return obj;
                }
            }
        }
        
        return nullptr;
    }
    
    // Copy objects from a group
    static std::vector<GameObject*> copyObjectsFromGroup(LevelEditorLayer* editor, int groupID) {
        return findObjectsByGroup(editor, groupID);
    }
    
    // Paste objects with offset
    static int pasteObjects(EditorUI* editorUI, const std::vector<GameObject*>& objects, float offsetX, float offsetY) {
        if (!editorUI) return 0;
        
        int pastedCount = 0;
        for (auto obj : objects) {
            if (!obj) continue;
            
            auto pos = obj->getPosition();
            auto newObj = editorUI->createObject(obj->m_objectID, {pos.x + offsetX, pos.y + offsetY});
            
            if (newObj) {
                // Copy groups
                if (obj->m_groups) {
                    for (int i = 0; i < obj->m_groups->size(); i++) {
                        newObj->addToGroup(obj->m_groups->at(i));
                    }
                }
                pastedCount++;
            }
        }
        
        return pastedCount;
    }
    
    // Batch set scale for group
    static int batchSetScale(LevelEditorLayer* editor, int groupID, float scaleX, float scaleY) {
        auto objects = findObjectsByGroup(editor, groupID);
        int count = 0;
        
        for (auto obj : objects) {
            if (obj) {
                obj->setScaleX(scaleX);
                obj->setScaleY(scaleY);
                count++;
            }
        }
        
        return count;
    }
    
    // Batch set rotation for group
    static int batchSetRotation(LevelEditorLayer* editor, int groupID, float rotation) {
        auto objects = findObjectsByGroup(editor, groupID);
        int count = 0;
        
        for (auto obj : objects) {
            if (obj) {
                obj->setRotation(rotation);
                count++;
            }
        }
        
        return count;
    }
    
    // Batch add to group
    static int batchAddToGroup(LevelEditorLayer* editor, int sourceGroup, int targetGroup) {
        auto objects = findObjectsByGroup(editor, sourceGroup);
        int count = 0;
        
        for (auto obj : objects) {
            if (obj) {
                obj->addToGroup(targetGroup);
                count++;
            }
        }
        
        return count;
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
        
        // Set properties using correct field names from bindings
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
            effectObj->m_moveOffset = cocos2d::CCPoint(moveX, moveY);  // property 28, 29
            effectObj->m_duration = duration;  // property 10
            effectObj->m_easingType = static_cast<EasingType>(easing);  // property 30
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
            effectObj->m_opacity = opacity;  // property 35
            effectObj->m_duration = duration;  // property 10
            effectObj->m_easingType = static_cast<EasingType>(easing);  // property 30
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
            effectObj->m_rotationDegrees = degrees;  // property 68
            effectObj->m_duration = duration;  // property 10
            effectObj->m_easingType = static_cast<EasingType>(easing);  // property 30
            effectObj->m_times360 = times;  // property 69
            effectObj->m_lockObjectRotation = lockRotation;  // property 70
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
            effectObj->m_duration = duration;  // property 10
            effectObj->m_easingType = static_cast<EasingType>(easing);  // property 30
        }
        
        return trigger;
    }
    
    // Create Color Trigger
    static GameObject* createColorTrigger(EditorUI* editorUI, float x, float y, int targetChannel,
                                          int r, int g, int b, float duration, int easing, float opacity) {
        if (!editorUI) return nullptr;
        
        // Color trigger ID = 899
        auto trigger = editorUI->createObject(899, {x, y});
        return trigger;
    }
    
    // Create Pulse Trigger
    static GameObject* createPulseTrigger(EditorUI* editorUI, float x, float y, int targetGroup,
                                          int r, int g, int b, float fadeIn, float hold, float fadeOut) {
        if (!editorUI) return nullptr;
        
        // Pulse trigger ID = 1006
        auto trigger = editorUI->createObject(1006, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
        }
        
        return trigger;
    }
    
    // Create Spawn Trigger
    static GameObject* createSpawnTrigger(EditorUI* editorUI, float x, float y, int targetGroup, float delay) {
        if (!editorUI) return nullptr;
        
        // Spawn trigger ID = 1268
        auto trigger = editorUI->createObject(1268, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
        }
        
        return trigger;
    }
    
    // Create Toggle Trigger
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
    
    // Create Stop Trigger
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
    
    // Create Follow Trigger
    static GameObject* createFollowTrigger(EditorUI* editorUI, float x, float y, int targetGroup, 
                                           int followGroup, float xMod, float yMod, float duration) {
        if (!editorUI) return nullptr;
        
        // Follow trigger ID = 1347
        auto trigger = editorUI->createObject(1347, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_targetGroupID = targetGroup;
        }
        
        return trigger;
    }
    
    // Create Shake Trigger
    static GameObject* createShakeTrigger(EditorUI* editorUI, float x, float y, float strength, float interval, float duration) {
        if (!editorUI) return nullptr;
        
        // Shake trigger ID = 1520
        auto trigger = editorUI->createObject(1520, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_shakeStrength = strength;  // property 75
            effectObj->m_shakeInterval = interval;  // property 84
            effectObj->m_duration = duration;
        }
        
        return trigger;
    }
    
    // Create Camera Offset Trigger
    static GameObject* createCameraOffsetTrigger(EditorUI* editorUI, float x, float y, float offsetX, float offsetY, float duration, int easing) {
        if (!editorUI) return nullptr;
        
        // Camera offset trigger ID = 1914
        auto trigger = editorUI->createObject(1914, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_moveOffset = cocos2d::CCPoint(offsetX, offsetY);
            effectObj->m_duration = duration;
            effectObj->m_easingType = static_cast<EasingType>(easing);
        }
        
        return trigger;
    }
    
    // Create Static Camera Trigger
    static GameObject* createStaticCameraTrigger(EditorUI* editorUI, float x, float y, bool enable, float duration) {
        if (!editorUI) return nullptr;
        
        // Static camera trigger ID = 1917
        auto trigger = editorUI->createObject(1917, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_duration = duration;
        }
        
        return trigger;
    }
    
    // Create Zoom Trigger (Camera Zoom)
    static GameObject* createZoomTrigger(EditorUI* editorUI, float x, float y, float zoom, float duration, int easing) {
        if (!editorUI) return nullptr;
        
        // Camera zoom trigger ID = 1916
        auto trigger = editorUI->createObject(1916, {x, y});
        if (!trigger) return nullptr;
        
        if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
            effectObj->m_duration = duration;
            effectObj->m_easingType = static_cast<EasingType>(easing);
        }
        
        return trigger;
    }
    
    // Get level info (object count, groups used, etc.)
    static std::string getLevelInfo(EditorUI* editorUI) {
        if (!editorUI) return "ERROR: No editor";
        
        auto editor = editorUI->m_editorLayer;
        if (!editor) return "ERROR: No editor layer";
        
        auto objects = editor->m_objects;
        if (!objects) return "ERROR: No objects array";
        
        int totalObjects = objects->count();
        
        // Count objects by type
        int blocks = 0;
        int spikes = 0;
        int portals = 0;
        int orbs = 0;
        int triggers = 0;
        int decorations = 0;
        
        for (int i = 0; i < totalObjects; i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (!obj) continue;
            
            int objID = obj->m_objectID;
            
            // Categorize objects
            if (objID == 1 || objID == 40) blocks++;
            else if (objID >= 8 && objID <= 39) spikes++;
            else if (objID >= 10 && objID <= 1933) portals++;
            else if (objID >= 36 && objID <= 1751) orbs++;
            else if (objID >= 899 && objID <= 3602) triggers++;
            else decorations++;
        }
        
        std::string result = "LEVEL_INFO:";
        result += "total=" + std::to_string(totalObjects);
        result += ",blocks=" + std::to_string(blocks);
        result += ",spikes=" + std::to_string(spikes);
        result += ",portals=" + std::to_string(portals);
        result += ",orbs=" + std::to_string(orbs);
        result += ",triggers=" + std::to_string(triggers);
        result += ",decorations=" + std::to_string(decorations);
        
        return result;
    }
    
    // List all objects in level
    static std::string listObjects(EditorUI* editorUI, int limit = 50) {
        if (!editorUI) return "ERROR: No editor";
        
        auto editor = editorUI->m_editorLayer;
        if (!editor) return "ERROR: No editor layer";
        
        auto objects = editor->m_objects;
        if (!objects) return "ERROR: No objects array";
        
        std::string result = "OBJECTS_LIST:";
        int count = limit < objects->count() ? limit : objects->count();
        
        for (int i = 0; i < count; i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (!obj) continue;
            
            auto pos = obj->getPosition();
            result += std::to_string(i) + ":{";
            result += "id=" + std::to_string(obj->m_objectID);
            result += ",x=" + std::to_string(pos.x);
            result += ",y=" + std::to_string(pos.y);
            result += "}";
            
            if (i < count - 1) result += ",";
        }
        
        return result;
    }
    
    // Undo last action
    static bool undoAction(EditorUI* editorUI) {
        if (!editorUI) return false;
        
        auto editor = editorUI->m_editorLayer;
        if (!editor) return false;
        
        editor->undoLastAction();
        return true;
    }
    
    // Redo last action
    static bool redoAction(EditorUI* editorUI) {
        if (!editorUI) return false;
        
        auto editor = editorUI->m_editorLayer;
        if (!editor) return false;
        
        editor->redoLastAction();
        return true;
    }
};
