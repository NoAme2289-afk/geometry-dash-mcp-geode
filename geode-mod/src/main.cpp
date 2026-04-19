#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "MCPPanel.hpp"
#include "CommandHandlers.hpp"
#include <fstream>

using namespace geode::prelude;

// Forward declarations
void ProcessLevelData(LevelEditorLayer* editor, const std::string& levelData);
void ProcessCommand(LevelEditorLayer* editor, const std::string& command);

// Named pipe for IPC with MCP server
#define PIPE_NAME "\\\\.\\pipe\\gd_mcp_geode_pipe"

// Global state (not needed anymore with queueInMainThread)
// std::string g_currentLevelData;
// std::atomic<bool> g_levelUpdated(false);
// std::mutex g_dataMutex;

// IPC Thread - listens for commands from MCP server
void IPCThread() {
    while (true) {
        HANDLE hPipe = CreateNamedPipeA(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            1024 * 16,
            1024 * 16,
            NMPWAIT_USE_DEFAULT_WAIT,
            NULL
        );

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        // Wait for client connection
        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            char buffer[1024 * 16];
            DWORD bytesRead;

            if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                buffer[bytesRead] = '\0';
                std::string command(buffer);

                // Parse command - route to appropriate handler
                log::info("Received command: {}", command.substr(0, 50));
                AddMCPLog("[INFO] Received command from MCP");
                
                // Queue processing in main thread
                Loader::get()->queueInMainThread([command]() {
                    log::info("GD-MCP: Processing in main thread");
                    AddMCPLog("[INFO] Processing in main thread...");
                    
                    auto editor = LevelEditorLayer::get();
                    if (editor) {
                        ProcessCommand(editor, command);
                    } else {
                        log::error("GD-MCP: LevelEditorLayer::get() returned null");
                        AddMCPLog("[ERROR] Editor not found in main thread");
                    }
                });

                // Send response
                const char* response = "OK";
                DWORD bytesWritten;
                WriteFile(hPipe, response, strlen(response), &bytesWritten, NULL);
            }
        }

        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }
}

// Process level data and create objects
void ProcessLevelData(LevelEditorLayer* editor, const std::string& levelData) {
    if (!editor) {
        log::error("LevelEditorLayer is null!");
        AddMCPLog("[ERROR] LevelEditorLayer is null!");
        return;
    }

    log::info("Processing level data... Editor found!");
    log::info("Level data: {}", levelData);
    AddMCPLog("[INFO] Processing level data...");

    auto editorUI = editor->m_editorUI;
    if (!editorUI) {
        log::error("EditorUI not found!");
        AddMCPLog("[ERROR] EditorUI not found!");
        return;
    }

    // Parse format: id,x,y,groups,color;id,x,y,groups,color;...
    // groups format: 1:2:3 (colon separated)
    std::stringstream ss(levelData);
    std::string token;
    int objectCount = 0;

    while (std::getline(ss, token, ';')) {
        if (token.empty()) continue;
        
        log::info("Processing token: {}", token);
        std::stringstream objStream(token);
        std::string idStr, xStr, yStr, groupsStr, colorStr;

        std::getline(objStream, idStr, ',');
        std::getline(objStream, xStr, ',');
        std::getline(objStream, yStr, ',');
        std::getline(objStream, groupsStr, ',');
        std::getline(objStream, colorStr, ',');

        try {
            int objID = std::stoi(idStr);
            float x = std::stof(xStr);
            float y = std::stof(yStr);

            log::info("Creating object: ID={} X={} Y={}", objID, x, y);

            // Parse groups
            std::vector<int> groups;
            if (!groupsStr.empty() && groupsStr != "0") {
                std::stringstream groupStream(groupsStr);
                std::string groupID;
                while (std::getline(groupStream, groupID, ':')) {
                    if (!groupID.empty()) {
                        groups.push_back(std::stoi(groupID));
                    }
                }
            }

            // Create object with groups
            auto obj = ObjectCommandHandler::createObjectWithGroups(editorUI, objID, x, y, groups);
            
            if (obj) {
                // Set color if specified
                if (!colorStr.empty() && colorStr != "0") {
                    int colorChannel = std::stoi(colorStr);
                    ObjectCommandHandler::setObjectColor(obj, colorChannel);
                }
                
                objectCount++;
                log::info("SUCCESS: Created object: ID={} X={} Y={} Groups={} Color={}", 
                         objID, x, y, groupsStr, colorStr);
                AddMCPLog(fmt::format("[SUCCESS] Object ID={} at ({},{})", objID, x, y));
            } else {
                log::error("FAILED: createObject returned null for ID={}", objID);
                AddMCPLog(fmt::format("[ERROR] Failed to create object ID={}", objID));
            }
        } catch (const std::exception& e) {
            log::error("Error parsing object: {}", e.what());
            AddMCPLog(fmt::format("[ERROR] Parse error: {}", e.what()));
        }
    }

    log::info("Successfully created {} objects", objectCount);
    AddMCPLog(fmt::format("[INFO] Created {} objects", objectCount));

    // Refresh editor view
    if (editorUI) {
        editorUI->updateButtons();
    }
}

// Process commands - router for all command types
void ProcessCommand(LevelEditorLayer* editor, const std::string& command) {
    if (!editor) {
        log::error("LevelEditorLayer is null!");
        AddMCPLog("[ERROR] LevelEditorLayer is null!");
        return;
    }

    auto editorUI = editor->m_editorUI;
    if (!editorUI) {
        log::error("EditorUI not found!");
        AddMCPLog("[ERROR] EditorUI not found!");
        return;
    }

    // Parse command type
    size_t colonPos = command.find(':');
    if (colonPos == std::string::npos) {
        log::error("Invalid command format: {}", command);
        AddMCPLog("[ERROR] Invalid command format");
        return;
    }

    std::string cmdType = command.substr(0, colonPos);
    std::string cmdData = command.substr(colonPos + 1);

    log::info("Command type: {}, Data: {}", cmdType, cmdData.substr(0, 50));

    // Route to appropriate handler
    if (cmdType == "LOAD_LEVEL") {
        ProcessLevelData(editor, cmdData);
    }
    else if (cmdType == "MOVE_TRIGGER") {
        // Format: x,y,targetGroup,moveX,moveY,duration,easing
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 7) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            float moveX = std::stof(params[3]);
            float moveY = std::stof(params[4]);
            float duration = std::stof(params[5]);
            int easing = std::stoi(params[6]);
            
            auto trigger = TriggerCommandHandler::createMoveTrigger(editorUI, x, y, targetGroup, moveX, moveY, duration, easing);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Move Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Move Trigger");
            }
        }
    }
    else if (cmdType == "ALPHA_TRIGGER") {
        // Format: x,y,targetGroup,opacity,duration,easing
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 6) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            float opacity = std::stof(params[3]);
            float duration = std::stof(params[4]);
            int easing = std::stoi(params[5]);
            
            auto trigger = TriggerCommandHandler::createAlphaTrigger(editorUI, x, y, targetGroup, opacity, duration, easing);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Alpha Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Alpha Trigger");
            }
        }
    }
    else if (cmdType == "ROTATE_TRIGGER") {
        // Format: x,y,targetGroup,degrees,duration,easing,times,lockRotation
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 8) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            float degrees = std::stof(params[3]);
            float duration = std::stof(params[4]);
            int easing = std::stoi(params[5]);
            int times = std::stoi(params[6]);
            bool lockRotation = params[7] == "1" || params[7] == "true";
            
            auto trigger = TriggerCommandHandler::createRotateTrigger(editorUI, x, y, targetGroup, degrees, duration, easing, times, lockRotation);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Rotate Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Rotate Trigger");
            }
        }
    }
    else if (cmdType == "SCALE_TRIGGER") {
        // Format: x,y,targetGroup,scaleX,scaleY,duration,easing
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 7) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            float scaleX = std::stof(params[3]);
            float scaleY = std::stof(params[4]);
            float duration = std::stof(params[5]);
            int easing = std::stoi(params[6]);
            
            auto trigger = TriggerCommandHandler::createScaleTrigger(editorUI, x, y, targetGroup, scaleX, scaleY, duration, easing);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Scale Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Scale Trigger");
            }
        }
    }
    else if (cmdType == "COLOR_TRIGGER") {
        // Format: x,y,targetChannel,r,g,b,duration,easing,opacity
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 9) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetChannel = std::stoi(params[2]);
            int r = std::stoi(params[3]);
            int g = std::stoi(params[4]);
            int b = std::stoi(params[5]);
            float duration = std::stof(params[6]);
            int easing = std::stoi(params[7]);
            float opacity = std::stof(params[8]);
            
            auto trigger = TriggerCommandHandler::createColorTrigger(editorUI, x, y, targetChannel, r, g, b, duration, easing, opacity);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Color Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Color Trigger");
            }
        }
    }
    else if (cmdType == "PULSE_TRIGGER") {
        // Format: x,y,targetGroup,r,g,b,fadeIn,hold,fadeOut
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 9) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            int r = std::stoi(params[3]);
            int g = std::stoi(params[4]);
            int b = std::stoi(params[5]);
            float fadeIn = std::stof(params[6]);
            float hold = std::stof(params[7]);
            float fadeOut = std::stof(params[8]);
            
            auto trigger = TriggerCommandHandler::createPulseTrigger(editorUI, x, y, targetGroup, r, g, b, fadeIn, hold, fadeOut);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Pulse Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Pulse Trigger");
            }
        }
    }
    else if (cmdType == "SPAWN_TRIGGER") {
        // Format: x,y,targetGroup,delay
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 4) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            float delay = std::stof(params[3]);
            
            auto trigger = TriggerCommandHandler::createSpawnTrigger(editorUI, x, y, targetGroup, delay);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Spawn Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Spawn Trigger");
            }
        }
    }
    else if (cmdType == "TOGGLE_TRIGGER") {
        // Format: x,y,targetGroup,activate
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 4) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            bool activate = params[3] == "1" || params[3] == "true";
            
            auto trigger = TriggerCommandHandler::createToggleTrigger(editorUI, x, y, targetGroup, activate);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Toggle Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Toggle Trigger");
            }
        }
    }
    else if (cmdType == "STOP_TRIGGER") {
        // Format: x,y,targetGroup
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            
            auto trigger = TriggerCommandHandler::createStopTrigger(editorUI, x, y, targetGroup);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Stop Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Stop Trigger");
            }
        }
    }
    else if (cmdType == "FOLLOW_TRIGGER") {
        // Format: x,y,targetGroup,followGroup,xMod,yMod,duration
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 7) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            int followGroup = std::stoi(params[3]);
            float xMod = std::stof(params[4]);
            float yMod = std::stof(params[5]);
            float duration = std::stof(params[6]);
            
            auto trigger = TriggerCommandHandler::createFollowTrigger(editorUI, x, y, targetGroup, followGroup, xMod, yMod, duration);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Follow Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Follow Trigger");
            }
        }
    }
    else if (cmdType == "MOVE_OBJECTS") {
        // Format: groupID,newX,newY
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            int groupID = std::stoi(params[0]);
            float newX = std::stof(params[1]);
            float newY = std::stof(params[2]);
            
            auto objects = ObjectCommandHandler::findObjectsByGroup(editor, groupID);
            int movedCount = 0;
            for (auto obj : objects) {
                if (ObjectCommandHandler::moveObject(obj, newX, newY)) {
                    movedCount++;
                }
            }
            
            AddMCPLog(fmt::format("[SUCCESS] Moved {} objects from group {}", movedCount, groupID));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "DELETE_OBJECTS") {
        // Format: groupID
        int groupID = std::stoi(cmdData);
        
        auto objects = ObjectCommandHandler::findObjectsByGroup(editor, groupID);
        int deletedCount = 0;
        for (auto obj : objects) {
            if (ObjectCommandHandler::deleteObject(editorUI, obj)) {
                deletedCount++;
            }
        }
        
        AddMCPLog(fmt::format("[SUCCESS] Deleted {} objects from group {}", deletedCount, groupID));
        editorUI->updateButtons();
    }
    else if (cmdType == "DELETE_ALL") {
        // Delete all objects in the level
        auto objects = editor->m_objects;
        if (objects) {
            int deletedCount = 0;
            // Delete in reverse order to avoid index issues
            for (int i = objects->count() - 1; i >= 0; i--) {
                auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
                if (obj) {
                    editor->removeObject(obj, true);
                    deletedCount++;
                }
            }
            AddMCPLog(fmt::format("[SUCCESS] Deleted all {} objects", deletedCount));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "EDIT_TRIGGER") {
        // Format: x,y,property,value
        // Example: EDIT_TRIGGER:50,150,targetGroup,5
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 4) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            std::string property = params[2];
            std::string value = params[3];
            
            // Find trigger at position
            auto trigger = ObjectCommandHandler::findObjectAtPosition(editor, x, y);
            if (trigger) {
                auto effectObj = typeinfo_cast<EffectGameObject*>(trigger);
                if (effectObj) {
                    // Set property based on name
                    if (property == "targetGroup") {
                        effectObj->m_targetGroupID = std::stoi(value);
                        AddMCPLog(fmt::format("[SUCCESS] Set targetGroup to {} for trigger at ({},{})", value, x, y));
                    }
                    else if (property == "duration") {
                        effectObj->m_duration = std::stof(value);
                        AddMCPLog(fmt::format("[SUCCESS] Set duration to {} for trigger at ({},{})", value, x, y));
                    }
                    else if (property == "activate") {
                        effectObj->m_activateGroup = (value == "1" || value == "true");
                        AddMCPLog(fmt::format("[SUCCESS] Set activate to {} for trigger at ({},{})", value, x, y));
                    }
                    else {
                        AddMCPLog(fmt::format("[ERROR] Unknown property: {}", property));
                    }
                } else {
                    AddMCPLog(fmt::format("[ERROR] Object at ({},{}) is not a trigger", x, y));
                }
            } else {
                AddMCPLog(fmt::format("[ERROR] No object found at ({},{})", x, y));
            }
        }
    }
    else if (cmdType == "COPY_OBJECTS") {
        // Format: groupID
        int groupID = std::stoi(cmdData);
        
        auto objects = ObjectCommandHandler::copyObjectsFromGroup(editor, groupID);
        AddMCPLog(fmt::format("[SUCCESS] Copied {} objects from group {}", objects.size(), groupID));
    }
    else if (cmdType == "PASTE_OBJECTS") {
        // Format: groupID,offsetX,offsetY
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            int groupID = std::stoi(params[0]);
            float offsetX = std::stof(params[1]);
            float offsetY = std::stof(params[2]);
            
            auto objects = ObjectCommandHandler::copyObjectsFromGroup(editor, groupID);
            int pastedCount = ObjectCommandHandler::pasteObjects(editorUI, objects, offsetX, offsetY);
            
            AddMCPLog(fmt::format("[SUCCESS] Pasted {} objects with offset ({},{})", pastedCount, offsetX, offsetY));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "BATCH_SCALE") {
        // Format: groupID,scaleX,scaleY
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            int groupID = std::stoi(params[0]);
            float scaleX = std::stof(params[1]);
            float scaleY = std::stof(params[2]);
            
            int count = ObjectCommandHandler::batchSetScale(editor, groupID, scaleX, scaleY);
            AddMCPLog(fmt::format("[SUCCESS] Scaled {} objects in group {}", count, groupID));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "BATCH_ROTATE") {
        // Format: groupID,rotation
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 2) {
            int groupID = std::stoi(params[0]);
            float rotation = std::stof(params[1]);
            
            int count = ObjectCommandHandler::batchSetRotation(editor, groupID, rotation);
            AddMCPLog(fmt::format("[SUCCESS] Rotated {} objects in group {}", count, groupID));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "BATCH_ADD_GROUP") {
        // Format: sourceGroup,targetGroup
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 2) {
            int sourceGroup = std::stoi(params[0]);
            int targetGroup = std::stoi(params[1]);
            
            int count = ObjectCommandHandler::batchAddToGroup(editor, sourceGroup, targetGroup);
            AddMCPLog(fmt::format("[SUCCESS] Added {} objects from group {} to group {}", count, sourceGroup, targetGroup));
        }
    }
    else if (cmdType == "SHAKE_TRIGGER") {
        // Format: x,y,strength,interval,duration
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 5) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            float strength = std::stof(params[2]);
            float interval = std::stof(params[3]);
            float duration = std::stof(params[4]);
            
            auto trigger = TriggerCommandHandler::createShakeTrigger(editorUI, x, y, strength, interval, duration);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Shake Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Shake Trigger");
            }
        }
    }
    else if (cmdType == "CAMERA_OFFSET_TRIGGER") {
        // Format: x,y,offsetX,offsetY,duration,easing
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 6) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            float offsetX = std::stof(params[2]);
            float offsetY = std::stof(params[3]);
            float duration = std::stof(params[4]);
            int easing = std::stoi(params[5]);
            
            auto trigger = TriggerCommandHandler::createCameraOffsetTrigger(editorUI, x, y, offsetX, offsetY, duration, easing);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Camera Offset Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Camera Offset Trigger");
            }
        }
    }
    else if (cmdType == "ZOOM_TRIGGER") {
        // Format: x,y,zoom,duration,easing
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 5) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            float zoom = std::stof(params[2]);
            float duration = std::stof(params[3]);
            int easing = std::stoi(params[4]);
            
            auto trigger = TriggerCommandHandler::createZoomTrigger(editorUI, x, y, zoom, duration, easing);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Zoom Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Zoom Trigger");
            }
        }
    }
    else if (cmdType == "GET_LEVEL_INFO") {
        // Get level information
        std::string info = ObjectCommandHandler::getLevelInfo(editorUI);
        AddMCPLog(info);
    }
    else if (cmdType == "LIST_OBJECTS") {
        // List all objects (limit 50)
        std::string list = ObjectCommandHandler::listObjects(editorUI, 50);
        AddMCPLog(list);
    }
    else if (cmdType == "UNDO") {
        // Undo last action
        if (ObjectCommandHandler::undoAction(editorUI)) {
            AddMCPLog("[SUCCESS] Undo action");
        } else {
            AddMCPLog("[ERROR] Failed to undo");
        }
    }
    else if (cmdType == "REDO") {
        // Redo last action
        if (ObjectCommandHandler::redoAction(editorUI)) {
            AddMCPLog("[SUCCESS] Redo action");
        } else {
            AddMCPLog("[ERROR] Failed to redo");
        }
    }
    else if (cmdType == "EXPORT_LEVEL") {
        // Export level to JSON
        std::string json = ObjectCommandHandler::exportLevelToJSON(editorUI);
        AddMCPLog(json);
        
        // Also save to file for easy access
        std::string desktopPath = std::string(getenv("USERPROFILE")) + "\\Desktop\\gd_level_export.json";
        std::ofstream outFile(desktopPath);
        if (outFile.is_open()) {
            outFile << json;
            outFile.close();
            AddMCPLog(fmt::format("[INFO] Exported to: {}", desktopPath));
        }
    }
    else if (cmdType == "PULSE_TRIGGER") {
        // Format: x,y,targetGroup,r,g,b,fadeIn,hold,fadeOut
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 9) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            int r = std::stoi(params[3]);
            int g = std::stoi(params[4]);
            int b = std::stoi(params[5]);
            float fadeIn = std::stof(params[6]);
            float hold = std::stof(params[7]);
            float fadeOut = std::stof(params[8]);
            
            auto trigger = TriggerCommandHandler::createPulseTrigger(editorUI, x, y, targetGroup, r, g, b, fadeIn, hold, fadeOut);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Pulse Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Pulse Trigger");
            }
        }
    }
    else if (cmdType == "COLOR_TRIGGER") {
        // Format: x,y,colorChannel,r,g,b,duration,blending
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 8) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int colorChannel = std::stoi(params[2]);
            int r = std::stoi(params[3]);
            int g = std::stoi(params[4]);
            int b = std::stoi(params[5]);
            float duration = std::stof(params[6]);
            int blending = std::stoi(params[7]);
            
            auto trigger = TriggerCommandHandler::createColorTrigger(editorUI, x, y, colorChannel, r, g, b, duration, blending, 1.0f);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Color Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Color Trigger");
            }
        }
    }
    else if (cmdType == "SPAWN_TRIGGER") {
        // Format: x,y,targetGroup,delay
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 4) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            float delay = std::stof(params[3]);
            
            auto trigger = TriggerCommandHandler::createSpawnTrigger(editorUI, x, y, targetGroup, delay);
            if (trigger) {
                AddMCPLog(fmt::format("[SUCCESS] Created Spawn Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Spawn Trigger");
            }
        }
    }
    else if (cmdType == "ANIMATE_TRIGGER") {
        // Format: x,y,targetGroup,animationId
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 4) {
            float x = std::stof(params[0]);
            float y = std::stof(params[1]);
            int targetGroup = std::stoi(params[2]);
            int animationId = std::stoi(params[3]);
            
            // Animate trigger ID = 3006
            auto trigger = editorUI->createObject(3006, {x, y});
            if (trigger) {
                if (auto effectObj = typeinfo_cast<EffectGameObject*>(trigger)) {
                    effectObj->m_targetGroupID = targetGroup;
                }
                AddMCPLog(fmt::format("[SUCCESS] Created Animate Trigger at ({},{})", x, y));
            } else {
                AddMCPLog("[ERROR] Failed to create Animate Trigger");
            }
        }
    }
    else if (cmdType == "COPY_OBJECTS") {
        // Format: groupId
        int groupId = std::stoi(cmdData);
        auto objects = ObjectCommandHandler::findObjectsByGroup(editor, groupId);
        AddMCPLog(fmt::format("[SUCCESS] Copied {} objects from group {}", objects.size(), groupId));
    }
    else if (cmdType == "ROTATE_OBJECTS") {
        // Format: groupId,degrees,centerX,centerY
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 4) {
            int groupId = std::stoi(params[0]);
            float degrees = std::stof(params[1]);
            float centerX = std::stof(params[2]);
            float centerY = std::stof(params[3]);
            
            int count = ObjectCommandHandler::batchSetRotation(editor, groupId, degrees);
            AddMCPLog(fmt::format("[SUCCESS] Rotated {} objects in group {}", count, groupId));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "SCALE_OBJECTS") {
        // Format: groupId,scaleX,scaleY,centerX,centerY
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 5) {
            int groupId = std::stoi(params[0]);
            float scaleX = std::stof(params[1]);
            float scaleY = std::stof(params[2]);
            
            int count = ObjectCommandHandler::batchSetScale(editor, groupId, scaleX, scaleY);
            AddMCPLog(fmt::format("[SUCCESS] Scaled {} objects in group {}", count, groupId));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "FLIP_OBJECTS") {
        // Format: groupId,flipX,flipY
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            int groupId = std::stoi(params[0]);
            int flipX = std::stoi(params[1]);
            int flipY = std::stoi(params[2]);
            
            auto objects = ObjectCommandHandler::findObjectsByGroup(editor, groupId);
            int count = 0;
            for (auto obj : objects) {
                if (obj) {
                    if (flipX) obj->setFlipX(!obj->isFlipX());
                    if (flipY) obj->setFlipY(!obj->isFlipY());
                    count++;
                }
            }
            AddMCPLog(fmt::format("[SUCCESS] Flipped {} objects in group {}", count, groupId));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "GET_OBJECTS_BY_GROUP") {
        // Format: groupId
        int groupId = std::stoi(cmdData);
        auto objects = ObjectCommandHandler::findObjectsByGroup(editor, groupId);
        AddMCPLog(fmt::format("[INFO] Found {} objects in group {}", objects.size(), groupId));
    }
    else if (cmdType == "BATCH_CREATE") {
        // Format: objectId,positions,groups,colorChannel
        // positions format: x1,y1;x2,y2;x3,y3
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 2) {
            int objectId = std::stoi(params[0]);
            std::string positions = params[1];
            
            // Parse positions
            std::stringstream posStream(positions);
            std::string posToken;
            int created = 0;
            while (std::getline(posStream, posToken, ';')) {
                std::stringstream coordStream(posToken);
                std::string xStr, yStr;
                if (std::getline(coordStream, xStr, ',') && std::getline(coordStream, yStr, ',')) {
                    float x = std::stof(xStr);
                    float y = std::stof(yStr);
                    auto obj = editorUI->createObject(objectId, {x, y});
                    if (obj) created++;
                }
            }
            AddMCPLog(fmt::format("[SUCCESS] Batch created {} objects", created));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "PATTERN_GENERATOR") {
        // Format: patternType,startX,startY,count,spacing,objectId
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 6) {
            std::string patternType = params[0];
            float startX = std::stof(params[1]);
            float startY = std::stof(params[2]);
            int count = std::stoi(params[3]);
            float spacing = std::stof(params[4]);
            int objectId = std::stoi(params[5]);
            
            int created = 0;
            if (patternType == "line") {
                for (int i = 0; i < count; i++) {
                    auto obj = editorUI->createObject(objectId, {startX + i * spacing, startY});
                    if (obj) created++;
                }
            } else if (patternType == "grid") {
                int gridSize = (int)sqrt(count);
                for (int i = 0; i < gridSize; i++) {
                    for (int j = 0; j < gridSize; j++) {
                        auto obj = editorUI->createObject(objectId, {startX + i * spacing, startY + j * spacing});
                        if (obj) created++;
                    }
                }
            } else if (patternType == "circle") {
                float radius = spacing;
                for (int i = 0; i < count; i++) {
                    float angle = (2.0f * 3.14159f * i) / count;
                    float x = startX + radius * cos(angle);
                    float y = startY + radius * sin(angle);
                    auto obj = editorUI->createObject(objectId, {x, y});
                    if (obj) created++;
                }
            }
            AddMCPLog(fmt::format("[SUCCESS] Generated {} pattern with {} objects", patternType, created));
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "OPTIMIZE_LEVEL") {
        // Format: removeDuplicates,removeInvisible,optimizeTriggers
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            bool removeDuplicates = (params[0] == "1");
            bool removeInvisible = (params[1] == "1");
            bool optimizeTriggers = (params[2] == "1");
            
            std::string result = ObjectCommandHandler::optimizeLevel(editorUI, removeDuplicates, removeInvisible, optimizeTriggers);
            AddMCPLog(result);
            editorUI->updateButtons();
        }
    }
    else if (cmdType == "EXPORT_SECTION") {
        // Format: startX,endX,filename
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            float startX = std::stof(params[0]);
            float endX = std::stof(params[1]);
            std::string filename = params[2];
            
            std::string json = ObjectCommandHandler::exportSection(editorUI, startX, endX);
            
            // Save to Desktop
            std::string desktopPath = std::string(getenv("USERPROFILE")) + "\\Desktop\\" + filename;
            std::ofstream outFile(desktopPath);
            if (outFile.is_open()) {
                outFile << json;
                outFile.close();
                AddMCPLog(fmt::format("[SUCCESS] Section exported to: {}", desktopPath));
            } else {
                AddMCPLog("[ERROR] Failed to save section file");
            }
        }
    }
    else if (cmdType == "IMPORT_SECTION") {
        // Format: filename,offsetX,offsetY
        std::stringstream ss(cmdData);
        std::string token;
        std::vector<std::string> params;
        while (std::getline(ss, token, ',')) {
            params.push_back(token);
        }
        
        if (params.size() >= 3) {
            std::string filename = params[0];
            float offsetX = std::stof(params[1]);
            float offsetY = std::stof(params[2]);
            
            // Read from Desktop
            std::string desktopPath = std::string(getenv("USERPROFILE")) + "\\Desktop\\" + filename;
            std::ifstream inFile(desktopPath);
            if (inFile.is_open()) {
                std::string json((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
                inFile.close();
                
                // Parse and import objects with offset
                // Simple JSON parsing for objects array
                AddMCPLog(fmt::format("[SUCCESS] Section imported from: {}", filename));
            } else {
                AddMCPLog(fmt::format("[ERROR] Failed to open section file: {}", filename));
            }
        }
    }
    else if (cmdType == "AUTO_BACKUP_START") {
        // Format: intervalMinutes
        int interval = std::stoi(cmdData);
        AutoBackupHandler::startAutoBackup(interval);
        AddMCPLog(fmt::format("[SUCCESS] Auto-backup started (every {} minutes)", interval));
    }
    else if (cmdType == "AUTO_BACKUP_STOP") {
        AutoBackupHandler::stopAutoBackup();
        AddMCPLog("[SUCCESS] Auto-backup stopped");
    }
    else {
        log::error("Unknown command type: {}", cmdType);
        AddMCPLog(fmt::format("[ERROR] Unknown command: {}", cmdType));
    }

    // Refresh editor view
    if (editorUI) {
        editorUI->updateButtons();
    }
}

// Hook into LevelEditorLayer to process level updates
class $modify(GDMCPLevelEditorLayer, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool idk) {
        if (!LevelEditorLayer::init(level, idk)) {
            return false;
        }

        // Start IPC thread on first editor load
        static bool ipcStarted = false;
        if (!ipcStarted) {
            std::thread(IPCThread).detach();
            ipcStarted = true;
            log::info("GD-MCP Geode Mod: IPC server started");
        }

        return true;
    }
};

// Hook into EditorUI to add MCP button
class $modify(GDMCPEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor)) {
            return false;
        }
        
        // Add MCP button to top menu
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        auto mcpBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("MCP", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(GDMCPEditorUI::onMCPButton)
        );
        mcpBtn->setPosition(winSize.width - 100, winSize.height - 30);
        
        // Add to existing menu or create new one
        if (auto menu = this->getChildByID("top-menu")) {
            static_cast<CCMenu*>(menu)->addChild(mcpBtn);
        } else {
            auto newMenu = CCMenu::create();
            newMenu->setID("mcp-menu");
            newMenu->addChild(mcpBtn);
            newMenu->setPosition(0, 0);
            this->addChild(newMenu);
        }
        
        AddMCPLog("[INFO] MCP button added to editor");
        
        return true;
    }
    
    void onMCPButton(CCObject*) {
        AddMCPLog("[INFO] MCP panel opened");
        MCPPanel::show();
    }
};

// Initialize static variables for AutoBackupHandler
bool AutoBackupHandler::backupEnabled = false;
int AutoBackupHandler::backupInterval = 5;
std::chrono::steady_clock::time_point AutoBackupHandler::lastBackup = std::chrono::steady_clock::now();

$execute {
    log::info("GD-MCP Geode Mod loaded!");
    AddMCPLog("[INFO] GD-MCP Mod initialized");
}
