#pragma once

#include "components/simple_scene.h"
#include <map>
#include <vector>
#include <string>

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {
            }
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {
            }
            float x;
            float y;
            float width;
            float height;
        };

        struct BlockType
        {
            std::string name;
            glm::vec3   color;
            int         w;
            int         h;
            std::string meshId;
        };

       enum GameState {
            EDITOR,
            GAME
        };

        std::string currentState;

        struct Cell {
            int blockId = -1;
        };

    public:
       Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawScene(glm::mat3 visMatrix);
        void DrawTun(glm::mat3 visMatrix, float x, float y, float size);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        glm::vec2 Tema1::MouseCoord(int mouseX, int mouseY);

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

        // Editor helpers
        bool CheckConstraints();
        void DrawSquare(const std::string& meshId, glm::mat3 visMatrix, float x, float y, float scale = 1.0f);
        void Tema1::DrawFrame(const std::string& meshId, glm::mat3 vis, float x, float y, float sx, float sy);
        void Tema1::DrawBumper(glm::mat3 visMatrix, float x, float y, float size);
        void Tema1::DrawMotor(glm::mat3 visMatrix, float x, float y, float size);

        void Tema1::DrawPong(glm::mat3 vis);
        void Tema1::DrawScore(glm::mat3 vis);

    protected:
        const int GRID_ROWS = 9;
        const int GRID_COLS = 17;

        std::vector<Cell>     grid;
        std::vector<BlockType> blockTypes;

        // drag and drop
        // index in blockTypes
        int   selectedBlock = -1;
        // cand dau click pe un block din stanga devine true, in rest e false
        bool  isDragging = false;
        float mouseXLogic = 0.0f;
        float mouseYLogic = 0.0f;

        // limits
        int   blockCount = 0;
        const int MAX_BLOCKS = 10;
        bool  constraintsOK = false;
        bool lastConstraintsOK = true;
        int lives;

        float cellSize = 0.8f;
        // pt partea stanga
        float leftX, leftY;
        float blockSize, spacingY;
        // pt grila
        float gridX, gridY;
        float startBtnX = 4.0f, startBtnY = 3.5f;
        float spacing;

        float length;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 modelMatrix, visMatrix;

        // pt pong
        float ballX, ballY;
        float ballSpeedX, ballSpeedY;
        float ballRadius;

        float paddleLeftY, paddleRightY;
        float paddleWidth, paddleHeight;
        float paddleSpeed;

        int scoreLeft, scoreRight;
    };
}   // namespace m1
#pragma once
