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

class MCPPanel : public CCLayer {
protected:
    CCLabelBMFont* m_statusLabel;
    CCLabelBMFont* m_logLabel;
    CCMenu* m_buttonMenu;
    
    bool init() {
        if (!CCLayer::init()) {
            return false;
        }
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        // Background
        auto bg = CCLayerColor::create({0, 0, 0, 180});
        this->addChild(bg, -1);
        
        // Panel background
        auto panel = CCScale9Sprite::create("GJ_square01.png");
        panel->setContentSize({300, 200});
        panel->setPosition(winSize / 2);
        this->addChild(panel);
        
        // Title
        auto title = CCLabelBMFont::create("MCP Control Panel", "goldFont.fnt");
        title->setScale(0.7f);
        title->setPosition(winSize.width / 2, winSize.height / 2 + 80);
        this->addChild(title);
        
        // Status label
        m_statusLabel = CCLabelBMFont::create(
            g_serverRunning ? "Server: Running" : "Server: Stopped",
            "bigFont.fnt"
        );
        m_statusLabel->setScale(0.4f);
        m_statusLabel->setPosition(winSize.width / 2, winSize.height / 2 + 50);
        this->addChild(m_statusLabel);
        
        // Buttons
        auto toggleBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                g_serverRunning ? "Stop" : "Start",
                "goldFont.fnt",
                "GJ_button_01.png",
                0.7f
            ),
            this,
            menu_selector(MCPPanel::onToggleServer)
        );
        
        auto copyBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Copy Logs", "goldFont.fnt", "GJ_button_01.png", 0.7f),
            this,
            menu_selector(MCPPanel::onCopyLogs)
        );
        
        auto clearBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Clear", "goldFont.fnt", "GJ_button_01.png", 0.7f),
            this,
            menu_selector(MCPPanel::onClearLogs)
        );
        
        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this,
            menu_selector(MCPPanel::onClose)
        );
        closeBtn->setPosition(winSize.width / 2 + 140, winSize.height / 2 + 90);
        
        m_buttonMenu = CCMenu::create();
        m_buttonMenu->addChild(toggleBtn);
        m_buttonMenu->addChild(copyBtn);
        m_buttonMenu->addChild(clearBtn);
        m_buttonMenu->addChild(closeBtn);
        
        toggleBtn->setPosition(-80, 10);
        copyBtn->setPosition(0, 10);
        clearBtn->setPosition(80, 10);
        
        m_buttonMenu->setPosition(winSize.width / 2, winSize.height / 2);
        this->addChild(m_buttonMenu);
        
        // Log display
        std::string logText = getLogsText();
        m_logLabel = CCLabelBMFont::create(logText.c_str(), "chatFont.fnt");
        m_logLabel->setScale(0.35f);
        m_logLabel->setPosition(winSize.width / 2, winSize.height / 2 - 30);
        m_logLabel->setAlignment(kCCTextAlignmentLeft);
        m_logLabel->setWidth(250);
        this->addChild(m_logLabel);
        
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        
        return true;
    }
    
    std::string getLogsText() {
        std::lock_guard<std::mutex> lock(g_logMutex);
        std::string result;
        int count = 0;
        for (auto it = g_mcpLogs.rbegin(); it != g_mcpLogs.rend() && count < 8; ++it, ++count) {
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
        {
            std::lock_guard<std::mutex> lock(g_logMutex);
            g_mcpLogs.clear();
        }
        AddMCPLog("[INFO] Logs cleared");
        refreshLogs();
    }
    
    void onClose(CCObject*) {
        this->removeFromParent();
    }
    
    void refreshLogs() {
        std::string logText = getLogsText();
        m_logLabel->setString(logText.c_str());
    }
    
    void keyBackClicked() override {
        onClose(nullptr);
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
    
    static void show() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene) {
            auto panel = MCPPanel::create();
            scene->addChild(panel, 1000);
        }
    }
};
