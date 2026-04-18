#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "MCPPanel.hpp"

using namespace geode::prelude;

// Forward declaration
void ProcessLevelData(LevelEditorLayer* editor, const std::string& levelData);

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

                // Parse command
                if (command.find("LOAD_LEVEL:") == 0) {
                    std::string levelData = command.substr(11);
                    
                    log::info("Received level data: {}", levelData.substr(0, 50));
                    AddMCPLog("[INFO] Received level data from MCP");
                    
                    // Queue processing in main thread
                    Loader::get()->queueInMainThread([levelData]() {
                        log::info("GD-MCP: Processing in main thread");
                        AddMCPLog("[INFO] Processing in main thread...");
                        
                        auto editor = LevelEditorLayer::get();
                        if (editor) {
                            ProcessLevelData(editor, levelData);
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

    // Parse format: id,x,y;id,x,y;id,x,y
    std::stringstream ss(levelData);
    std::string token;
    int objectCount = 0;

    while (std::getline(ss, token, ';')) {
        log::info("Processing token: {}", token);
        std::stringstream objStream(token);
        std::string idStr, xStr, yStr;

        if (std::getline(objStream, idStr, ',') &&
            std::getline(objStream, xStr, ',') &&
            std::getline(objStream, yStr, ',')) {

            try {
                int objID = std::stoi(idStr);
                float x = std::stof(xStr);
                float y = std::stof(yStr);

                log::info("Attempting to create object: ID={} X={} Y={}", objID, x, y);

                // Use EditorUI to create object properly
                if (auto editorUI = editor->m_editorUI) {
                    auto obj = editorUI->createObject(objID, {x, y});
                    
                    if (obj) {
                        objectCount++;
                        log::info("SUCCESS: Created object: ID={} X={} Y={}", objID, x, y);
                        AddMCPLog(fmt::format("[SUCCESS] Created object: ID={} X={} Y={}", objID, x, y));
                    } else {
                        log::error("FAILED: createObject returned null for ID={}", objID);
                        AddMCPLog(fmt::format("[ERROR] Failed to create object ID={}", objID));
                    }
                } else {
                    log::error("EditorUI not found!");
                    AddMCPLog("[ERROR] EditorUI not found!");
                }
            } catch (const std::exception& e) {
                log::error("Error parsing object: {}", e.what());
                AddMCPLog(fmt::format("[ERROR] Parse error: {}", e.what()));
            }
        } else {
            log::error("Failed to parse token: {}", token);
            AddMCPLog(fmt::format("[ERROR] Failed to parse: {}", token));
        }
    }

    log::info("Successfully created {} objects", objectCount);
    AddMCPLog(fmt::format("[INFO] Created {} objects", objectCount));

    // Refresh editor view
    if (auto editorUI = editor->m_editorUI) {
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
        mcpBtn->setPosition(winSize.width - 30, winSize.height - 30);
        
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

$execute {
    log::info("GD-MCP Geode Mod loaded!");
    AddMCPLog("[INFO] GD-MCP Mod initialized");
}
