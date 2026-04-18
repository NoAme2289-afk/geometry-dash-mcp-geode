#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

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

class MCPPanel : public Popup<> {
protected:
    CCLabelBMFont* m_statusLabel;
    TextArea* m_logArea;
    CCMenu* m_buttonMenu;
    
    bool setup() override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        this->setTitle("MCP Control Panel");
        
        // Status label
        m_statusLabel = CCLabelBMFont::create(
            g_serverRunning ? "Server: Running" : "Server: Stopped",
            "bigFont.fnt"
        );
        m_statusLabel->setScale(0.5f);
        m_statusLabel->setPosition(winSize.width / 2, winSize.height / 2 + 80);
        m_mainLayer->addChild(m_statusLabel);
        
        // Start/Stop button
        auto toggleBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                g_serverRunning ? "Stop Server" : "Start Server",
                "goldFont.fnt",
                "GJ_button_01.png",
                0.8f
            ),
            this,
            menu_selector(MCPPanel::onToggleServer)
        );
        
        // Copy logs button
        auto copyBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Copy Logs", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(MCPPanel::onCopyLogs)
        );
        
        // Clear logs button
        auto clearBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Clear Logs", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(MCPPanel::onClearLogs)
        );
        
        m_buttonMenu = CCMenu::create();
        m_buttonMenu->addChild(toggleBtn);
        m_buttonMenu->addChild(copyBtn);
        m_buttonMenu->addChild(clearBtn);
        m_buttonMenu->setLayout(
            RowLayout::create()
                ->setGap(10.0f)
                ->setAxisAlignment(AxisAlignment::Center)
        );
        m_buttonMenu->setPosition(winSize.width / 2, winSize.height / 2 + 40);
        m_buttonMenu->setContentSize({300, 40});
        m_buttonMenu->updateLayout();
        m_mainLayer->addChild(m_buttonMenu);
        
        // Log display area
        std::string logText = getLogsText();
        m_logArea = TextArea::create(logText, "chatFont.fnt", 0.5f, 280.0f, {0, 0}, 20.0f, false);
        
        auto scrollLayer = ScrollLayer::create({280, 100});
        scrollLayer->m_contentLayer->addChild(m_logArea);
        scrollLayer->m_contentLayer->setContentSize({280, m_logArea->getContentSize().height});
        scrollLayer->setPosition(winSize.width / 2 - 140, winSize.height / 2 - 60);
        m_mainLayer->addChild(scrollLayer);
        
        return true;
    }
    
    std::string getLogsText() {
        std::lock_guard<std::mutex> lock(g_logMutex);
        std::string result;
        for (const auto& log : g_mcpLogs) {
            result += log + "\n";
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
        
        // Refresh logs
        refreshLogs();
    }
    
    void onCopyLogs(CCObject*) {
        std::string logs = getLogsText();
        clipboard::write(logs);
        
        FLAlertLayer::create(
            "Success",
            "Logs copied to clipboard!",
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
        m_logArea->setString(logText);
    }
    
public:
    static MCPPanel* create() {
        auto ret = new MCPPanel();
        if (ret && ret->initAnchored(320.0f, 240.0f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
