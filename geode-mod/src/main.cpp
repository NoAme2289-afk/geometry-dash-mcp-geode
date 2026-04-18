#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

// Named pipe for IPC with MCP server
#define PIPE_NAME "\\\\.\\pipe\\gd_mcp_geode_pipe"

// Global state
std::string g_currentLevelData;
bool g_levelUpdated = false;

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
                    g_currentLevelData = command.substr(11);
                    g_levelUpdated = true;

                    log::info("Received level data: {}", g_currentLevelData.substr(0, 50));

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
void ProcessLevelData(const std::string& levelData) {
    auto editor = LevelEditorLayer::get();
    if (!editor) {
        log::error("LevelEditorLayer not found!");
        return;
    }

    log::info("Processing level data...");

    // Parse format: id,x,y;id,x,y;id,x,y
    std::stringstream ss(levelData);
    std::string token;
    int objectCount = 0;

    while (std::getline(ss, token, ';')) {
        std::stringstream objStream(token);
        std::string idStr, xStr, yStr;

        if (std::getline(objStream, idStr, ',') &&
            std::getline(objStream, xStr, ',') &&
            std::getline(objStream, yStr, ',')) {

            try {
                int objID = std::stoi(idStr);
                float x = std::stof(xStr);
                float y = std::stof(yStr);

                // Create object using Geode API
                std::string objectString = fmt::format("1,{},2,{},3,{}", objID, x, y);
                auto obj = GameObject::objectFromString(objectString);

                if (obj) {
                    // Add object to editor
                    editor->addObject(obj);
                    objectCount++;
                    log::info("Created object: ID={} X={} Y={}", objID, x, y);
                } else {
                    log::error("Failed to create object from string: {}", objectString);
                }
            } catch (const std::exception& e) {
                log::error("Error parsing object: {}", e.what());
            }
        }
    }

    log::info("Successfully created {} objects", objectCount);

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

    void update(float dt) {
        LevelEditorLayer::update(dt);

        // Check if we have new level data to process
        if (g_levelUpdated) {
            ProcessLevelData(g_currentLevelData);
            g_levelUpdated = false;
        }
    }
};

$execute {
    log::info("GD-MCP Geode Mod loaded!");
}
