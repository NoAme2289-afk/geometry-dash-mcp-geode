#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Global log storage
inline std::vector<std::string> g_mcpLogs;
inline std::mutex g_logMutex;
inline bool g_serverRunning = false;

// Add log entry
inline void AddMCPLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_mcpLogs.push_back(message);
    if (g_mcpLogs.size() > 100) {
        g_mcpLogs.erase(g_mcpLogs.begin());
    }
}

class MCPPanel : public FLAlertLayer {
protected:
    CCLabelBMFont* m_statusLabel;
    CCLabelBMFont* m_logLabel;
    CCMenu* m_buttonMenu;
    
    bool init() {
        if (!FLAlertLayer::init(nullptr, "MCP Control", "OK", nullptr, 320.0f, false, 240.0f, 1.0f)) {
            return false;
        }
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto layer = this->m_mainLayer;
        
        // Status label
        m_statusLabel = CCLabelBMFont::create(
            g_serverRunning ? "Server: Running" : "Server: Stopped",
            "bigFont.fnt"
        );
        m_statusLabel->setScale(0.5f);
        m_statusLabel->setPosition(160, 200);
        layer->addChild(m_statusLabel);
        
        // Start/Stop button
        auto toggleBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                g_serverRunning ? "Stop" : "Start",
                "goldFont.fnt",
                "GJ_button_01.png",
                0.8f
            ),
            this,
            menu_selector(MCPPanel::onToggleServer)
        );
        toggleBtn->setPosition(-80, 160);
        
        // Copy logs button
        auto copyBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Copy", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(MCPPanel::onCopyLogs)
        );
        copyBtn->setPosition(0, 160);
        
        // Clear logs button
        auto clearBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Clear", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(MCPPanel::onClearLogs)
        );
        clearBtn->setPosition(80, 160);
        
        m_buttonMenu = CCMenu::create();
        m_buttonMenu->addChild(toggleBtn);
        m_buttonMenu->addChild(copyBtn);
        m_buttonMenu->addChild(clearBtn);
        m_buttonMenu->setPosition(160, 0);
        layer->addChild(m_buttonMenu);
        
        // Log display
        std::string logText = getLogsText();
        m_logLabel = CCLabelBMFont::create(logText.c_str(), "chatFont.fnt");
        m_logLabel->setScale(0.4f);
        m_logLabel->setPosition(160, 80);
        m_logLabel->setAnchorPoint({0.5f, 0.5f});
        m_logLabel->setAlignment(kCCTextAlignmentLeft);
        layer->addChild(m_logLabel);
        
        return true;
    }
    
    std::string getLogsText() {
        std::lock_guard<std::mutex> lock(g_logMutex);
        std::string result;
        int count = 0;
        for (auto it = g_mcpLogs.rbegin(); it != g_mcpLogs.rend() && count < 10; ++it, ++count) {
            result += *it + "\n";
        }
        if (result.empty()) {
            result = "No logs yet...";
        }
        return result;
    }
    
    void onToggleServer(CCObject*) {
        g_serverRunning = !g_serverRunning;
        
        if (g_serverRunning) {
            AddMCPLog("[INFO] Server started");
            m_statusLabel->setString("Server: Running");
        } else {
            AddMCPLog("[INFO] Server stopped");
            m_statusLabel->setString("Server: Stopped");
        }
        
        refreshLogs();
    }
    
    void onCopyLogs(CCObject*) {
        std::string logs = getLogsText();
        clipboard::write(logs);
        
        FLAlertLayer::create(
            "Success",
            "Logs copied!",
            "OK"
        )->show();
    }
    
    void onClearLogs(CCObject*) {
        std::lock_guard<std::mutex> lock(g_logMutex);
        g_mcpLogs.clear();
        AddMCPLog("[INFO] Logs cleared");
        refreshLogs();
    }
    
    void refreshLogs() {
        std::string logText = getLogsText();
        m_logLabel->setString(logText.c_str());
    }
    
public:
    static MCPPanel* create() {
        auto ret = new MCPPanel();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
