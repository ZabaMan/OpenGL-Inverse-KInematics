#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "model.h"
#include "game_object.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    unsigned int            Width, Height;
    glm::vec3               LightPos;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt, GLFWwindow* window);
    void ProcessInput(string text);
    void Update(float dt);
    void CCD(bool reset = false);
    void RotateBone(GameObject* effector, GameObject* bone, glm::vec3 target);
    void DebugCCD();
    void DebugRotateBone(GameObject* effector, GameObject* bone, glm::vec3 target);
    void DebugReset();
    void SyncSkinnedMesh();
    void Render();
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);
    unsigned loadTexture(char const* path);
    string toString(glm::vec3 toString);
};

#endif