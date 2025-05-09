/**
 * @file GameManager.cpp
 * @brief Implementation of the Game Manager for the game engine.
 * @details Manages the game state, frame timing, and overall game systems.
 * @author
 * @date
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include managers and utility headers
#include "GameManager.h"
#include "LogManager.h"
#include "InputManager.h" 
#include "ECSManager.h"
#include "SerialisationManager.h"
#include "../System/InputSystem.h"
#include "../Utility/Clock.h"
#include "../Utility/AssetPath.h"

namespace gam300 {

    // Initialize singleton instance
    GameManager::GameManager() {
        setType("GameManager");
        m_game_over = false;
        m_step_count = 0;
    }

    // Get the singleton instance
    GameManager& GameManager::getInstance() {
        static GameManager instance;
        return instance;
    }

    // Start up the GameManager - initialize all required systems
    int GameManager::startUp() {
        // Call parent's startUp() first
        if (Manager::startUp())
            return -1;

        // Start the LogManager
        LogManager& logManager = LogManager::getInstance();
        if (logManager.startUp()) {
            // Failed to start LogManager
            return -1;
        }

        logManager.writeLog("GameManager::startUp() - LogManager started successfully");

        // Start the InputManager
        if (IM.startUp()) {
            logManager.writeLog("GameManager::startUp() - Failed to start InputManager");
            logManager.shutDown();
            return -1;
        }

        logManager.writeLog("GameManager::startUp() - InputManager started successfully");

        // Start the ECSManager
        if (EM.startUp()) {
            logManager.writeLog("GameManager::startUp() - Failed to start ECSManager");
            IM.shutDown();
            logManager.shutDown();
            return -1;
        }

        logManager.writeLog("GameManager::startUp() - ECSManager started successfully");

        // Start the SerialisationManager
        if (SEM.startUp()) {
            logManager.writeLog("GameManager::startUp() - Failed to start SerialisationManager");
            EM.shutDown();
            IM.shutDown();
            logManager.shutDown();
            return -1;
        }

        logManager.writeLog("GameManager::startUp() - SerialisationManager started successfully");

        // Register the InputSystem to process our Input components
        auto inputSystem = EM.registerSystem<InputSystem>();
        if (!inputSystem) {
            logManager.writeLog("GameManager::startUp() - Failed to register InputSystem");
        }
        else {
            logManager.writeLog("GameManager::startUp() - InputSystem registered successfully");
        }

        // Load the scene
        const std::string scenePath = getAssetFilePath("Scene/Game.scn");
        if (SEM.loadScene(scenePath)) {
            logManager.writeLog("GameManager::startUp() - Scene loaded successfully from %s", scenePath.c_str());
        }
        else {
            logManager.writeLog("GameManager::startUp() - Failed to load scene, creating default scene");
            // Save to the same path
            SEM.saveScene(scenePath);
            if (SEM.loadScene(scenePath)) {
                logManager.writeLog("GameManager::startUp() - Default scene loaded successfully");
            }
            else {
                logManager.writeLog("GameManager::startUp() - WARNING: Failed to load default scene");
            }
        }

        // Initialize step count
        m_step_count = 0;

        // Game is not over yet
        m_game_over = false;

        return 0;
    }

    // Check if an event is valid for the GameManager
    bool GameManager::isValid(std::string event_name) const {
        // GameManager only accepts "step" events
        return (event_name == "step");
    }

    // Shut down the GameManager - clean up all resources
    void GameManager::shutDown() {
        // Log shutdown
        LogManager& logManager = LogManager::getInstance();
        logManager.writeLog("GameManager::shutDown() - Shutting down GameManager");

        // Set game over
        setGameOver();

        // Shut down managers in reverse order of initialization
        SEM.shutDown();
        EM.shutDown();
        IM.shutDown();
        logManager.shutDown();

        // Call parent's shutDown()
        Manager::shutDown();
    }

    // Update the game state for the current frame
    void GameManager::update(float dt) {
        // Increment step count
        m_step_count++;

        // Log every 100 steps
        if (m_step_count % 100 == 0) {
            LM.writeLog("GameManager::update() - Step count: %d", m_step_count);
        }

        // Check for escape key to quit
        if (IM.isKeyJustPressed(GLFW_KEY_ESCAPE)) {
            setGameOver(true);
            LM.writeLog("GameManager::update() - Escape key pressed, setting game over");
        }

        // Update all ECS systems
        EM.updateSystems(dt);
    }

    // Set game over status
    void GameManager::setGameOver(bool new_game_over) {
        m_game_over = new_game_over;

        // Log game over state change if setting to true
        if (new_game_over) {
            LogManager& logManager = LogManager::getInstance();
            logManager.writeLog("GameManager::setGameOver() - Game over set to true");
        }
    }

    // Get game over status
    bool GameManager::getGameOver() const {
        return m_game_over;
    }

    // Get frame time in milliseconds
    int GameManager::getFrameTime() const {
        // For now, return the default value
        // In a more complete implementation, this would be read from a config file
        return FRAME_TIME_DEFAULT;
    }

    // Get step count
    int GameManager::getStepCount() const {
        return m_step_count;
    }

} // end of namespace gam300