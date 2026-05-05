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
        
        // Dark background overlay
        auto bg = CCLayerColor::create({0, 0, 0, 150});
        this->addChild(bg, -1);
        
        // Main Panel Sprite
        auto panel = CCScale9Sprite::create("GJ_square05.png");
        panel->setContentSize({350, 250});
        panel->setPosition(winSize / 2);
        panel->setID("main-panel");
        this->addChild(panel);
        
        // Title with nice shadow
        auto title = CCLabelBMFont::create("MCP CONTROL PANEL", "goldFont.fnt");
        title->setScale(0.8f);
        title->setPosition(winSize.width / 2, winSize.height / 2 + 100);
        this->addChild(title);
        
        // Status with color based on state
        m_statusLabel = CCLabelBMFont::create(
            g_serverRunning ? "SERVER: RUNNING" : "SERVER: STOPPED",
            "bigFont.fnt"
        );
        m_statusLabel->setScale(0.45f);
        m_statusLabel->setPosition(winSize.width / 2, winSize.height / 2 + 70);
        m_statusLabel->setColor(g_serverRunning ? ccColor3B{0, 255, 0} : ccColor3B{255, 0, 0});
        this->addChild(m_statusLabel);
        
        // Log Container (Darker area for logs)
        auto logBg = CCLayerColor::create({0, 0, 0, 100});
        logBg->setContentSize({310, 100});
        logBg->setPosition(winSize.width / 2 - 155, winSize.height / 2 - 40);
        this->addChild(logBg);
        
        // Log display
        std::string logText = getLogsText();
        m_logLabel = CCLabelBMFont::create(logText.c_str(), "chatFont.fnt");
        m_logLabel->setScale(0.4f);
        m_logLabel->setAnchorPoint({0, 1});
        m_logLabel->setPosition(winSize.width / 2 - 145, winSize.height / 2 + 50);
        m_logLabel->setAlignment(kCCTextAlignmentLeft);
        m_logLabel->setWidth(290);
        this->addChild(m_logLabel);
        
        // Buttons with proper layout
        auto toggleBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(g_serverRunning ? "STOP" : "START", "goldFont.fnt", "GJ_button_01.png", 0.6f),
            this, menu_selector(MCPPanel::onToggleServer)
        );
        
        auto copyBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("COPY LOGS", "goldFont.fnt", "GJ_button_01.png", 0.6f),
            this, menu_selector(MCPPanel::onCopyLogs)
        );
        
        auto clearBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("CLEAR", "goldFont.fnt", "GJ_button_06.png", 0.6f),
            this, menu_selector(MCPPanel::onClearLogs)
        );
        
        m_buttonMenu = CCMenu::create();
        m_buttonMenu->addChild(toggleBtn);
        m_buttonMenu->addChild(copyBtn);
        m_buttonMenu->addChild(clearBtn);
        
        // Arrange buttons horizontally with spacing
        m_buttonMenu->alignItemsHorizontallyWithPadding(10.0f);
        m_buttonMenu->setPosition(winSize.width / 2, winSize.height / 2 - 80);
        this->addChild(m_buttonMenu);
        
        // Close button in corner
        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MCPPanel::onClose)
        );
        auto closeMenu = CCMenu::create();
        closeMenu->addChild(closeBtn);
        closeMenu->setPosition(winSize.width / 2 + 165, winSize.height / 2 + 115);
        this->addChild(closeMenu);
        
        // --- ANIMATIONS ---
        this->setScale(0.1f);
        this->setOpacity(0);
        this->runAction(CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)));
        this->runAction(CCFadeIn::create(0.2f));
        
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        
        return true;
    }
    
    void onToggleServer(CCObject*) {
        g_serverRunning = !g_serverRunning;
        
        m_statusLabel->setString(g_serverRunning ? "SERVER: RUNNING" : "SERVER: STOPPED");
        m_statusLabel->setColor(g_serverRunning ? ccColor3B{0, 255, 0} : ccColor3B{255, 0, 0});
        
        // Visual feedback on button
        auto btn = static_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(0));
        btn->setSprite(ButtonSprite::create(g_serverRunning ? "STOP" : "START", "goldFont.fnt", "GJ_button_01.png", 0.6f));
        
        AddMCPLog(g_serverRunning ? "[INFO] Server started" : "[INFO] Server stopped");
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

    std::string getLogsText() {
        std::lock_guard<std::mutex> lock(g_logMutex);
        std::string result;
        int count = 0;
        // Show last 7 logs to fit comfortably in the panel
        for (auto it = g_mcpLogs.rbegin(); it != g_mcpLogs.rend() && count < 7; ++it, ++count) {
            std::string line = *it;
            
            // Add color tags for BMFont
            if (line.find("[ERROR]") != std::string::npos) {
                line = "<cr>" + line + "</c>"; // Bright Red
            }
            else if (line.find("[SUCCESS]") != std::string::npos) {
                line = "<cg>" + line + "</c>"; // Bright Green
            }
            else if (line.find("[INFO]") != std::string::npos) {
                line = "<cb>" + line + "</c>"; // Sky Blue
            }
            else {
                line = "<cy>" + line + "</c>"; // Gold for everything else
            }
            
            result += line + "\n";
        }
        if (result.empty()) {
            result = "<cl>No logs yet...</c>"; // Grayish
        }
        return result;
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
