#include "lab_m1/tema1/Tema1.h"

#include <vector>
#include <queue>
#include <utility>
#include <iostream>

#include "lab_m1/tema1/object2d.h"
#include "lab_m1/tema1/transform2d.h"

using namespace std;
using namespace m1;
using std::cout;
using std::endl;


Tema1::Tema1() {}

Tema1::~Tema1() {}

void Tema1::Init() {
    glDisable(GL_DEPTH_TEST);
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);
    currentState = "EDITOR";

    // x e de la 0 la 12 si y de la 0 la 7
    logicSpace = {0.f, 0.f, 12.f, 7.f };
    
    leftX = 0.8f;
    leftY = 5.5f;  

    // pt stanga
    blockSize = 0.5f;
    spacingY = 1.7f;

    gridX = 2.5f;
    gridY = 4.5f;
    cellSize = blockSize;
    spacing = 0.04f;

    lives = 10;
    lastConstraintsOK = true;

    // imi fixez originea
    glm::vec3 origin = glm::vec3(0, 0, 0);
    length = 1.0f;
    
    AddMeshToList(object2D::CreateSquare("sq_white", origin, length, glm::vec3(1, 1, 1), true));
    AddMeshToList(object2D::CreateSquare("sq_grayD", origin, length, glm::vec3(0.2f, 0.2f, 0.2f), true));
    AddMeshToList(object2D::CreateSquare("sq_grayM", origin, length, glm::vec3(0.3f, 0.3f, 0.3f), true));
    AddMeshToList(object2D::CreateSquare("sq_purple", origin, length, glm::vec3(2.5f, 1.5f, 2.5f), true));
    AddMeshToList(object2D::CreateSquare("sq_red", origin, length, glm::vec3(1, 0, 0), true));
    AddMeshToList(object2D::CreateSquare("sq_blue", origin, length, glm::vec3(0, 0, 1), true));
    AddMeshToList(object2D::CreateSquare("sq_yell", origin, length, glm::vec3(1, 1, 0), true));
    AddMeshToList(object2D::CreateSquare("sq_greenDim", origin, length, glm::vec3(0, 0.6f, 0), true));
    AddMeshToList(object2D::CreateSquare("sq_border", origin, length, glm::vec3(1, 0, 1), false));
    AddMeshToList(object2D::CreateSquare("sq_grid", origin, length, glm::vec3(0.9f, 0.05f, 0.5f), true));
    AddMeshToList(object2D::CreateHeart("heart_full", origin, length, glm::vec3(1.0f, 0.2f, 0.5f)));
    AddMeshToList(object2D::CreateSquare("sq_pink", origin, length, glm::vec3(1.0f, 0.2f, 0.5f), true));  // roz neon
    AddMeshToList(object2D::CreateHeart("heart_empty", origin, length, glm::vec3(0.4f, 0.4f, 0.4f))); 
    AddMeshToList(object2D::CreateSquare("sq_solid", origin, length, glm::vec3(0.988f, 0.337f, 0.18f), true));
    AddMeshToList(object2D::CreateCircle("pong_ball", 0.2f, glm::vec3(0.9f, 0.3f, 0.1f)));
    AddMeshToList(object2D::CreateSquare("paleta", glm::vec3(0, 0, 0), 1, glm::vec3(1, 0.557f, 0.745f), true));
    AddMeshToList(object2D::CreateSquare("score", glm::vec3(0, 0, 0), 1, glm::vec3(1.0f, 0.3f, 0.8f), true));


    blockTypes = {
        { "Bumper", glm::vec3(1, 1, 0), 3, 1, "sq_yell" },
        { "Tun",    glm::vec3(0, 1, 0), 1, 3, "sq_greenDim" },
        { "Motor", glm::vec3(0, 0, 1), 1, 1, "sq_blue" },
        { "Solid",  glm::vec3(1, 0, 0), 1, 1, "sq_solid"  }
    };

    // grila
    grid.resize(GRID_ROWS * GRID_COLS);
    for (int i = 0; i < grid.size(); i++) {
        // celula goala
        grid[i].blockId = -1;
    }

    // pt pong
    ballX = 6.0f;
    ballY = 3.5f;
    ballRadius = 0.2f;

    paddleWidth = 0.25f;
    paddleHeight = 1.2f;

    paddleLeftY = 3.5f;
    paddleRightY = 3.5f;

    paddleSpeed = 4.0f;

    scoreLeft = 0;
    scoreRight = 0;
}


// 2D visualization matrix -> fix din lab
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace) {
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}


// Uniform 2D visualization matrix (same scale factor on x and y axes) -> fix din lab
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace) {
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear) {
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->GetResolution();
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0.0f, 0.1f, 0.4f), true);

    // Compute the 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);

    if (currentState._Equal("EDITOR")) {
        DrawScene(visMatrix);
    }
    else if (currentState._Equal("GAME")) {
        DrawPong(visMatrix);
    }

    if (isDragging && selectedBlock >= 0) {
        const BlockType& bt = blockTypes[selectedBlock];

        float x = mouseXLogic - blockSize / 2.0f;
        float y = mouseYLogic - blockSize / 2.0f;

        if (bt.name == "Solid") {
            DrawSquare("sq_solid", visMatrix, x, y, blockSize);
        }
        else if (bt.name == "Bumper") {
            DrawBumper(visMatrix, x, y, blockSize);
        }
        else if (bt.name == "Tun") {
            DrawTun(visMatrix, x, y, blockSize);
        }
        else if (bt.name == "Motor") {
            DrawMotor(visMatrix, x, y, blockSize);
        }
    }
    
    constraintsOK = CheckConstraints();
}

void Tema1::FrameEnd() {}

// functie auxiliara ca sa desenez patrate
void Tema1::DrawSquare(const std::string& meshId, glm::mat3 vis, float x, float y, float scale) {
    glm::mat3 m = vis * transform2D::Translate(x, y) * transform2D::Scale(scale, scale);
    RenderMesh2D(meshes[meshId], shaders["VertexColor"], m);
}

void Tema1::DrawScene(glm::mat3 visMatrix) {
    // grila
    // latime grila
    float gridW = GRID_COLS * cellSize + (GRID_COLS - 1) * spacing;
    // inaltime grila
    float gridH = GRID_ROWS * cellSize + (GRID_ROWS - 1) * spacing;
    // partea de jos a grilei = partea de sus - inaltime
    float gridDown = gridY + cellSize - gridH;

    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {

            float gx = gridX + c * (cellSize + spacing);
            float gy = gridY - r * (cellSize + spacing);

            DrawSquare("sq_grid", visMatrix, gx, gy, cellSize);
        }
    }

    // plasare blocuri pe grila
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {

            int index = r * GRID_COLS + c;
            int id = grid[index].blockId;

            if (id < 0) continue;

            const BlockType& bt = blockTypes[id];

            float gx = gridX + c * (cellSize + spacing) + (cellSize - blockSize) / 2;
            float gy = gridY - r * (cellSize + spacing) + (cellSize - blockSize) / 2;

            if (bt.name == "Solid") {
                DrawSquare("sq_solid", visMatrix, gx, gy, blockSize);
            }
            else if (bt.name == "Bumper") {
                DrawBumper(visMatrix, gx, gy, blockSize);
            }
            else if (bt.name == "Tun") {
                DrawTun(visMatrix, gx, gy, blockSize);
            }
            else if (bt.name == "Motor") {
                DrawMotor(visMatrix, gx, gy, blockSize);
            }
        }
    }

    // conturul din jurul grilei
    const float margin = 0.09f;
    DrawFrame("sq_border", visMatrix, gridX - margin, gridDown - margin, gridW + 2 * margin,
        gridH + 2 * margin);

    // partea stanga
    for (int i = 0; i < (int)blockTypes.size(); ++i) {
        float px = leftX;
        float py = leftY - i * spacingY;

        if (blockTypes[i].name == "Solid") {
            DrawSquare("sq_solid", visMatrix, px, py + blockSize * 0.25f, blockSize);
        } else if (blockTypes[i].name == "Bumper") {
            DrawBumper(visMatrix, px, py + blockSize * 0.2f, blockSize);
        } else if (blockTypes[i].name == "Tun") {
            DrawTun(visMatrix, px, py - blockSize * 0.4f, blockSize);
        } else if (blockTypes[i].name == "Motor") {
            DrawMotor(visMatrix, px, py + blockSize * 0.4f, blockSize);
        }
    }
    // liniile dintre blocuri
    float frameWidth = blockSize * 4.0f;         
    float frameHeight = spacingY * blockTypes.size() + blockSize;

    float frameX = leftX - 0.75f;
    float frameY = leftY + blockSize * 1.45f;

    DrawFrame("sq_border", visMatrix, frameX, frameY - frameHeight, frameWidth, frameHeight * 1.1f);

    float lineThickness = 0.03f;

    for (int i = 1; i < (int)blockTypes.size(); i++) {
        // pozitia pe Y la mijlocul dintre doua blocuri
        float lineY = leftY - i * spacingY + spacingY / 2.0f;
        // linia dintre bumper si tun
        if (i == 1) {
            lineY += spacingY * 0.35f;
        }
        // linia dintre tun si motor
        else if (i == 2) {
            lineY += spacingY * 0.25f;
        }
        // linia dintre motor si solid
        else if (i == 3) {
            lineY += spacingY * 0.18f;
        }
        glm::mat3 mLine = visMatrix * transform2D::Translate(frameX, lineY) * transform2D::Scale(frameWidth, lineThickness);

        RenderMesh2D(meshes["sq_border"], shaders["VertexColor"], mLine);
    }

    // partea de sus -> life bar
    float barCount = 11;
    float barSize = 0.58f;
    float barSpacing = 0.2f;
    // mi-am luat de unde incepe bara si pozitia pe axa Y ca sa le centrez
    float barStartX = 3.0f;
    float barY = 6.5f;

    for (int i = 0; i < barCount - 1; ++i) {
        // desenez fiecare patrat pe rand mai la dreapta decat cel anterior
        float x = barStartX + i * (barSize + barSpacing);
        const bool available = (i < lives);

        // alegerea culorii
        std::string meshId;
        if (available) {
            DrawSquare("heart_full", visMatrix, x, barY, barSize);
        } else {
            DrawSquare("heart_empty", visMatrix, x, barY, barSize);
        }
        DrawSquare(meshId, visMatrix, x, barY, barSize);
    }

    // pt ultimul bloc = stegulet - > fac 3 triunghiui: jos, stanga si jos
    float flagX = barStartX + (barCount - 1) * (barSize + barSpacing);
    float flagY = 6.13f;
    glm::vec3 startColor;
    if (constraintsOK) {
        startColor = glm::vec3(0.969f, 0.988f, 0);
    }
    else {
        startColor = glm::vec3(1.0f, 0.2f, 0.2f);
    }
    // coordonate varfuri
    std::vector<VertexFormat> firstT = {
        VertexFormat(glm::vec3(0, barSize, 0), startColor),
        VertexFormat(glm::vec3(barSize, barSize, 0), startColor),
        VertexFormat(glm::vec3(barSize * 0.5f, barSize / 2, 0), startColor)
    };

    // ordine varfuri
    std::vector<unsigned int> ind = {0, 1, 2};
    Mesh* flag1 = new Mesh("flag1");
    flag1->InitFromData(firstT, ind);

    // triunghiul de jos
    std::vector<VertexFormat> secondT = {
         VertexFormat(glm::vec3(0, 0, 0), startColor),
         VertexFormat(glm::vec3(barSize, 0, 0), startColor),
         VertexFormat(glm::vec3(barSize * 0.5f, barSize / 2, 0), startColor)
    };
    std::vector<unsigned int> secInd = { 0, 1, 2 };
    Mesh* flag2 = new Mesh("flag_bot");
    flag2->InitFromData(secondT, secInd);

    // triunghiul din stanga
    std::vector<VertexFormat> thirdT = {
        VertexFormat(glm::vec3(0, 0, 0), startColor),
        VertexFormat(glm::vec3(0, barSize, 0), startColor),
        VertexFormat(glm::vec3(barSize * 0.5f, barSize / 2, 0), startColor)
    };
    std::vector<unsigned int> thirdInd = { 0, 1, 2 };
    Mesh* flag3 = new Mesh("flag_left");
    flag3->InitFromData(thirdT, thirdInd);


    glm::mat3 m = visMatrix * transform2D::Translate(flagX, flagY) * transform2D::Scale(1.0f, 1.0f);
    RenderMesh2D(flag1, shaders["VertexColor"], m);
    RenderMesh2D(flag2, shaders["VertexColor"], m);
    RenderMesh2D(flag3, shaders["VertexColor"], m);
}

// fct auxiliara pt chenarul din jurul grilei
void Tema1::DrawFrame(const std::string& meshId, glm::mat3 vis, float x, float y, float sx, float sy) {
    // matricea de tranformare
    glm::mat3 m = vis * transform2D::Translate(x, y) * transform2D::Scale(sx, sy);
    RenderMesh2D(meshes[meshId], shaders["VertexColor"], m);
}

void Tema1::DrawTun(glm::mat3 visMatrix, float x, float y, float size) {
    float semiRadius = size / 2.0f;
    float baseH = size * 0.5f;
    // grosimea tevii
    float barrelW = size * 0.75f;
    // inaltimea
    float barrelH = size * 1.6f;

    // BAZA -> jumatate de patrat
    glm::vec3 baseColor = glm::vec3(0.98f, 0.38f, 0.91f);
    if (meshes.find("tun_base_square") == meshes.end()) {
        AddMeshToList(object2D::CreateSquare("tun_base_square", glm::vec3(0, 0, 0), 1, baseColor, true));
    }
    glm::mat3 mBase = visMatrix * transform2D::Translate(x, y) * transform2D::Scale(size, size * 0.5f);
    RenderMesh2D(meshes["tun_base_square"], shaders["VertexColor"], mBase);

    // TEAVA
    glm::vec3 barrelColor = glm::vec3(0.839f, 0.427f, 0.78f);
    if (meshes.find("tun_barrel") == meshes.end()) {
        AddMeshToList(object2D::CreateSquare("tun_barrel", glm::vec3(0, 0, 0), 1, barrelColor, true));
    }

    glm::mat3 m2 = visMatrix * transform2D::Translate(x + size / 2.0f - barrelW / 2, y + size + semiRadius - (semiRadius * 0.4f) - size / 2.0f) 
        * transform2D::Scale(barrelW, barrelH);
    RenderMesh2D(meshes["tun_barrel"], shaders["VertexColor"], m2);

    // partea de sus a tevii
    if (meshes.find("tun_barrel") == meshes.end()) {
        AddMeshToList(object2D::CreateSquare("tun_barrel", glm::vec3(0, 0, 0), 1, barrelColor, true));
    }
    float stepCount = 4;
    // inaltimea fiecarui dreptunghi
    float stepH = barrelH * 0.06f;
    float widen = 0.1f;
    // desenez fiecare nivel
    for (int i = 0; i < stepCount; i++) {
        float currWidth = barrelW * (1.0f + widen * (i + 1));
        float currY = y + size + semiRadius - (semiRadius * 0.45f) + barrelH + i * stepH - size / 2.0f;

        glm::mat3 mBarrelStep = visMatrix * transform2D::Translate(x + size / 2.0f - currWidth / 2.0f, currY) * transform2D::Scale(currWidth, stepH);
        RenderMesh2D(meshes["tun_barrel"], shaders["VertexColor"], mBarrelStep);
    }

    // SEMICERC
    glm::vec3 semiColor = glm::vec3(1.0f, 0.439f, 0.937f);
    if (meshes.find("tun_base_semi") == meshes.end()) {
        AddMeshToList(object2D::CreateSemicircle("tun_base_semi", semiRadius, semiColor));
    };
    glm::mat3 m1 = visMatrix * transform2D::Translate(x + size * 0.5f, y + size * 0.5f);
    RenderMesh2D(meshes["tun_base_semi"], shaders["VertexColor"], m1);
}

void Tema1::DrawBumper(glm::mat3 visMatrix, float x, float y, float size) {
    // PATRAT
    glm::vec3 baseColor = glm::vec3(0.502f, 0.227f, 0.949f);
    if (meshes.find("bumper_base_square") == meshes.end()) {
        AddMeshToList(object2D::CreateSquare("bumper_base_square", glm::vec3(0, 0, 0), 1, baseColor, true));
    }
    glm::mat3 mBase = visMatrix * transform2D::Translate(x, y) * transform2D::Scale(size, size);
    RenderMesh2D(meshes["bumper_base_square"], shaders["VertexColor"], mBase);

    // SEMICERC
    float semiRadius = size / 0.8f;
    glm::vec3 semiColor = glm::vec3(0.38f, 0.078f, 0.878f);
    if (meshes.find("bumper_base_semi") == meshes.end()) {
        AddMeshToList(object2D::CreateSemicircle("bumper_base_semi", semiRadius, semiColor));
    };
    glm::mat3 m1 = visMatrix * transform2D::Translate(x + size / 2.0f, y + size - size * 0.2f);
    RenderMesh2D(meshes["bumper_base_semi"], shaders["VertexColor"], m1);
}

void Tema1::DrawMotor(glm::mat3 visMatrix, float x, float y, float size) {
    // PATRAT
    glm::vec3 baseColor = glm::vec3(0.541, 0.078, 0.078);
    if (meshes.find("motor_square") == meshes.end()) {
        AddMeshToList(object2D::CreateSquare("motor_square", glm::vec3(0, 0, 0), 1, baseColor, true));
    }
    glm::mat3 mBase = visMatrix * transform2D::Translate(x, y) * transform2D::Scale(size, size);
    RenderMesh2D(meshes["motor_square"], shaders["VertexColor"], mBase);

    // FLACARA -> 4 triunghiuri
    glm::vec3 flameColor = glm::vec3(0.541, 0.078, 0.078);
    // inaltime flacara
    float flameH = size * 1.7f;
    // latime flacara
    float flameW = size * 2.0f;

    std::vector<VertexFormat> flameVertices = {
        // triunghi stanga (dreptunghic)
        VertexFormat(glm::vec3(0, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW * 0.25f, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW * 0.125f, -flameH, 0), flameColor),

        // triunghi centru-stanga (mic, subtire)
        VertexFormat(glm::vec3(flameW * 0.25f, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW * 0.5f, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW * 0.375f, -flameH * 0.8f, 0), flameColor),

        // triunghi centru-dreapta (mic, subtire)
        VertexFormat(glm::vec3(flameW * 0.5f, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW * 0.75f, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW * 0.625f, -flameH * 0.8f, 0), flameColor),

        // triunghi dreapta (dreptunghic)
        VertexFormat(glm::vec3(flameW * 0.75f, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW, 0, 0), flameColor),
        VertexFormat(glm::vec3(flameW * 0.875f, -flameH, 0), flameColor)
    };
    std::vector<unsigned int> flameIndices = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11
    };

    Mesh* flameMesh = new Mesh("engine_flame");
    flameMesh->InitFromData(flameVertices, flameIndices);

    glm::mat3 mFlame = visMatrix * transform2D::Translate(x, y) * transform2D::Translate(0, 0) * transform2D::Scale(size, size);
    RenderMesh2D(flameMesh, shaders["VertexColor"], mFlame);
}

void Tema1::DrawPong(glm::mat3 visMatrix) {
    // chenar 
    float left = 1.5f;
    float right = 10.5f;
    float top = 6.0f;
    float bottom = 0.8f;
    float border = 0.05f;

    RenderMesh2D(meshes["sq_white"], shaders["VertexColor"], visMatrix * transform2D::Translate(left, top) 
        * transform2D::Scale(right - left, border));

    RenderMesh2D(meshes["sq_white"], shaders["VertexColor"], visMatrix * transform2D::Translate(left, bottom) * 
        transform2D::Scale(right - left, border));

    RenderMesh2D(meshes["sq_white"], shaders["VertexColor"], visMatrix * transform2D::Translate(left, bottom) * 
        transform2D::Scale(border, top - bottom));

    RenderMesh2D(meshes["sq_white"], shaders["VertexColor"], visMatrix * transform2D::Translate(right - border, bottom) * 
        transform2D::Scale(border, top - bottom));

    // ajustare palete
    float paddleLeftX = left + 0.15f;
    float paddleRightX = right - paddleWidth - 0.15f;

    // paleta stanga
    glm::mat3 l = visMatrix * transform2D::Translate(paddleLeftX, paddleLeftY - paddleHeight / 2) * transform2D::Scale(paddleWidth, paddleHeight);
    RenderMesh2D(meshes["paleta"], shaders["VertexColor"], l);

    // paleta dreapta
    glm::mat3 r = visMatrix * transform2D::Translate(paddleRightX, paddleRightY - paddleHeight / 2) * transform2D::Scale(paddleWidth, paddleHeight);
    RenderMesh2D(meshes["sq_pink"], shaders["VertexColor"], r);

    // linie punctata
    for (float y = bottom + 0.1f; y < top - 0.1f; y += 0.6f) {
        glm::mat3 line = visMatrix * transform2D::Translate(6.0f, y) * transform2D::Scale(0.1f, 0.3f);
        RenderMesh2D(meshes["sq_white"], shaders["VertexColor"], line);
    }

    // bila
    AddMeshToList(object2D::CreateCircle("pong_ball", ballRadius, glm::vec3(0, 0.824f, 1)));
    glm::mat3 ball = visMatrix * transform2D::Translate(ballX, ballY);
    RenderMesh2D(meshes["pong_ball"], shaders["VertexColor"], ball);
}

void Tema1::OnInputUpdate(float deltaTime, int mods) {}

// transform pozitia mouse-ului in coordonate
glm::vec2 Tema1::MouseCoord(int mouseX, int mouseY) {
    glm::ivec2 res = window->GetResolution();
    float logicX = logicSpace.x + (mouseX / (float)res.x) * logicSpace.width;
    float logicY = logicSpace.y + ((res.y - mouseY) / (float)res.y) * logicSpace.height;
    return glm::vec2(logicX, logicY);
}

void Tema1::OnKeyPress(int key, int mods) {}


void Tema1::OnKeyRelease(int key, int mods) {}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    if (isDragging)
    {
        glm::vec2 logicPos = MouseCoord(mouseX, mouseY);
        mouseXLogic = logicPos.x;
        mouseYLogic = logicPos.y;
    }
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    glm::vec2 logicPos = MouseCoord(mouseX, mouseY);
    float x = logicPos.x;
    float y = logicPos.y;

    // click dreapta -> stergere bloc
    if (button == 2) {
        bool wasValid = constraintsOK;
        for (int r = 0; r < GRID_ROWS; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                float gx = gridX + c * (cellSize + spacing);
                float gy = gridY - r * (cellSize + spacing);

                if (x >= gx && x <= gx + cellSize && y >= gy && y <= gy + cellSize) {
                    grid[r * GRID_COLS + c].blockId = -1;
                    {
                        grid[r * GRID_COLS + c].blockId = -1;

                        if (lives < 10) lives++;
                        return;
                    }
                }
            }
        }
        return;
    }
    // click start
    if (button == 1) {
        // coordonatele steguletului/start
        float barCount = 11;
        float barSize = 0.58f;
        float barSpacing = 0.2f;
        float barStartX = 3.0f;
        float barY = 6.5f;

        float flagX = barStartX + (barCount - 1) * (barSize + barSpacing);
        float flagY = 6.13f;

        // daca apas fix pe zona steguletului
        if (x >= flagX && x <= flagX + barSize && y >= flagY && y <= flagY + barSize)
        {
            if (!constraintsOK) {
                return;
            }

            // start galben - > joc
            currentState = "GAME";
            return;
        }
    }
    // click stanga -> drag and drop
    if (button != 1) return;

    selectedBlock = -1;

    // verific daca am apasat pe un bloc din bara stanga
    for (int i = 0; i < (int)blockTypes.size(); i++) {
        float px = leftX;
        float py = leftY - i * spacingY;

        float top = py;
        float bottom = top + blockSize;

        if (blockTypes[i].name == "Solid") {
            top = py;
            bottom = py + blockSize;
        }

        if (blockTypes[i].name == "Motor") {
            top = py + blockSize * 0.40f;
            bottom = top + blockSize;
        }

        if (blockTypes[i].name == "Bumper") {
            top = py + blockSize * 0.20f;
            bottom = top + blockSize * 1.8f;
        }

        if (blockTypes[i].name == "Tun") {
            top = py - + blockSize * 0.8f;
            bottom = top + blockSize * 3.0f;
        }

        if (x >= px && x <= px + blockSize && y >= top && y <= bottom) {
            isDragging = true;
            selectedBlock = i;
            mouseXLogic = x;
            mouseYLogic = y;
            return;
        }
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    if (button != 1) return;
    if (!isDragging) return;

        glm::vec2 logicPos = MouseCoord(mouseX, mouseY);
        float x = logicPos.x;
        float y = logicPos.y;
        isDragging = false;

        if (selectedBlock < 0) {
            isDragging = false;
            selectedBlock = -1;
            return;
        }

        // caut in ce celula a fost eliberat
        for (int r = 0; r < GRID_ROWS; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                float gx = gridX + c * (cellSize + spacing);
                float gy = gridY - r * (cellSize + spacing);

                if (x >= gx && x <= gx + cellSize && y >= gy && y <= gy + cellSize) {
                    if (grid[r * GRID_COLS + c].blockId == -1) {
                        if (lives > 0) lives--;
                    }
                    grid[r * GRID_COLS + c].blockId = selectedBlock;
                    selectedBlock = -1;
                    constraintsOK = CheckConstraints();
                    return;
                }
            }
        }

        selectedBlock = -1;
        constraintsOK = CheckConstraints();
    
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

bool Tema1::CheckConstraints()
{
    int countBlocks = 0;
    int motor_r = -1, motor_c = -1;
    int tun_r = -1, tun_c = -1;
    std::vector<std::pair<int, int>> bumpers;

    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            int id = grid[r * GRID_COLS + c].blockId;
            if (id == -1) continue;
            ++countBlocks;
            const BlockType& bt = blockTypes[id];

            if (bt.name == "Motor") {
                motor_r = r; motor_c = c;
            }
            else if (bt.name == "Tun") {
                tun_r = r; tun_c = c;
            }
            else if (bt.name == "Bumper") {
                bumpers.push_back({ r, c });
            }
        }
    }

    // max 10 blocuri
    if (countBlocks > 10) return false;

    //conex
    {
        // gasesc un bloc de start
        bool foundStart = false;
        int sr = -1;
        int sc = -1;
        for (int r = 0; r < GRID_ROWS && !foundStart; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                if (grid[r * GRID_COLS + c].blockId != -1) {
                    sr = r; sc = c;
                    foundStart = true;
                    break;
                }
            }
        }
        if (!foundStart) return true;
        std::vector<std::vector<bool>> visited(GRID_ROWS, std::vector<bool>(GRID_COLS, false));
        std::queue<std::pair<int, int>> q;
        q.push({ sr,sc });
        visited[sr][sc] = true;
        int visitedCount = 1;

        // BFS
        int dr[4] = { 1,-1,0,0 };
        int dc[4] = { 0,0,1,-1 };
        while (!q.empty()) {
            auto cell = q.front();
            int r = cell.first;
            int c = cell.second;
            q.pop();
            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS) {
                    if (!visited[nr][nc] && grid[nr * GRID_COLS + nc].blockId != -1) {
                        visited[nr][nc] = true;
                        q.push({ nr,nc });
                        visitedCount++;
                    }
                }
            }
        }
        if (visitedCount != countBlocks) return false;
    }

    // niciun bloc sub motor
    if (motor_r != -1) {
        for (int r = motor_r + 1; r < GRID_ROWS; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                if (grid[r * GRID_COLS + c].blockId != -1) return false;
            }
        }
    }

    // niciun bloc deasupra tunului
    if (tun_r != -1) {
        for (int r = 0; r < tun_r; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                if (grid[r * GRID_COLS + c].blockId != -1) return false;
            }
        }
    }

    // tun fix langa bumper
    for (auto& b : bumpers) {
        int br = b.first, bc = b.second;
        if (tun_r != -1) {
            if (abs(br - tun_r) + abs(bc - tun_c) <= 1) return false;
        }
    }

    // doua bumpere vecine
    for (auto& b1 : bumpers) {
        for (auto& b2 : bumpers) {
            if (&b1 == &b2) continue;
            if (abs(b1.first - b2.first) + abs(b1.second - b2.second) == 1) return false;
        }
    }

    // zona de deasupra bumperului (3 coloane)
    if (!bumpers.empty()) {
        int br = bumpers[0].first;
        int bc = bumpers[0].second;
        for (int c = bc - 1; c <= bc + 1; ++c) {
            if (c < 0 || c >= GRID_COLS) continue;
            for (int r = 0; r < br; ++r) {
                if (grid[r * GRID_COLS + c].blockId != -1) return false;
            }
        }
    }

    return true;
}
