#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct
{
    int board[20][10];
    Color boardColors[20][10];
    int rowsToDelete[4];
    bool deleteRows;
    bool startDeleteAnimation;
    int rotationTransitions[7][4][2];
    int animationCounter;
    int score;
    time_t startTime;
} GameState;

typedef struct
{
    int rotation;
    int* tiles;
    int xSize;
    int ySize;
    int positionX;
    int positionY;
    int type;
    Color color;
} Shape;

GameState* InitGameState()
{
    GameState* gameState = (GameState*)malloc(sizeof(GameState));
    for(int i = 0; i < 20; ++i)
    {
        for(int j = 0; j < 10; ++j)
        {
            gameState->board[i][j] = 0;
        }
    }
    for(int i = 0; i < 4; ++i)
    {
        gameState->rowsToDelete[i] = -1;
    }
    gameState->deleteRows = false;
    gameState->startDeleteAnimation = false;
    int rotationTransitions[7][4][2] = {{{2, -1}, {-2, 2}, {1, -2}, {-1, 1}},  //i
        {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //j
        {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //l
        {{0, 0},  {0, 0},  {0, 0},  {0, 0}},   //o
        {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //s
        {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //t
        {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},}; //z
    memcpy(gameState->rotationTransitions, rotationTransitions, sizeof(gameState->rotationTransitions));
    gameState->animationCounter = 0;
    gameState->score = 0;
    return gameState;
}

Shape* InitShape(int xSize, int ySize)
{
    Shape* shape = (Shape*)malloc(sizeof(Shape));
    shape->rotation = 0;
    shape->xSize = xSize;
    shape->ySize = ySize;
    shape->positionX = 3;
    shape->positionY = 0;
    shape->tiles = (int *)malloc(xSize * ySize * sizeof(int));
    return shape;
}

Shape* CreateShape(int type)
{
    Shape* shape = NULL;
    switch (type)
    {
        case 0:
        shape = InitShape(4, 1);
        shape->tiles[0] = 1; shape->tiles[1] = 1; shape->tiles[2] = 1; shape->tiles[3] = 1;
        shape->color = BLUE;
        break;
        case 1:
        shape = InitShape(3, 2);
        shape->tiles[0] = 1; shape->tiles[1] = 0; shape->tiles[2] = 0; 
        shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 1;
        shape->color = DARKBLUE;
        break;
        case 2:
        shape = InitShape(3, 2);
        shape->tiles[0] = 0; shape->tiles[1] = 0; shape->tiles[2] = 1; 
        shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 1;
        shape->color = ORANGE;
        break;
        case 3:
        shape = InitShape(2, 2);
        shape->tiles[0] = 1; shape->tiles[1] = 1; 
        shape->tiles[2] = 1; shape->tiles[3] = 1;
        shape->positionX = 4;
        shape->color = YELLOW;
        break;   
        case 4:
        shape = InitShape(3, 2);
        shape->tiles[0] = 0; shape->tiles[1] = 1; shape->tiles[2] = 1; 
        shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 0;
        shape->color = GREEN;
        break;
        case 5:
        shape = InitShape(3, 2);
        shape->tiles[0] = 0; shape->tiles[1] = 1; shape->tiles[2] = 0; 
        shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 1;
        shape->color = PURPLE;
        break;
        case 6:
        shape = InitShape(3, 2);
        shape->tiles[0] = 1; shape->tiles[1] = 1; shape->tiles[2] = 0; 
        shape->tiles[3] = 0; shape->tiles[4] = 1; shape->tiles[5] = 1;
        shape->color = RED;
        break;
    }
    shape->type = type;
    return shape;
}

void FreeShape(Shape* shape)
{
    free(shape->tiles);
    free(shape);
}

bool WillShapeColide(Shape* shape, int moveX, int moveY, GameState* gameState)
{
    for(int x = 0; x < shape->xSize; ++x)
    {
        int boardX = shape->positionX + x + moveX;
        for(int y = 0; y < shape->ySize; ++y)
        {
            int boardY = shape->positionY + y + moveY;
            if(shape->tiles[y * shape->xSize + x] == 1)
            {
                if(boardX < 0 || boardX > 9 || 
                   boardY < 0 || boardY > 19 ||
                   gameState->board[boardY][boardX] == 1)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void RotateShape(Shape* shape, GameState* gameState)
{
    int moveX = gameState->rotationTransitions[shape->type][shape->rotation][0];
    int moveY = gameState->rotationTransitions[shape->type][shape->rotation][1];
    
    int xSize = shape->xSize;
    shape->xSize = shape->ySize;
    shape->ySize = xSize;
    
    int* oldTiles = (int*)malloc(shape->xSize * shape->ySize * sizeof(int));
    memcpy(oldTiles, shape->tiles, shape->xSize * shape->ySize * sizeof(int));
    for(int y = 0; y < shape->ySize; ++y)
    {
        for(int x = 0; x < shape->xSize; ++x)
        {
            shape->tiles[y * shape->xSize + x] = oldTiles[((shape->xSize - 1 - x) * shape->ySize) + y];
        }
    }
    
    if(!WillShapeColide(shape, moveX, moveY, gameState))
    {
        shape->positionX += moveX;
        shape->positionY += moveY;
        shape->rotation = (shape->rotation + 1) % 4;  
    }
    else
    {
        xSize = shape->xSize;
        shape->xSize = shape->ySize;
        shape->ySize = xSize;
        memcpy(shape->tiles, oldTiles, shape->xSize * shape->ySize * sizeof(int));
        return;
    }
    
    free(oldTiles);
}

void UpdateBoard(Shape* shape, GameState* gameState)
{
    for(int x = 0; x < shape->xSize; ++x)
    {
        int boardX = shape->positionX + x;
        for(int y = 0; y < shape->ySize; ++y)
        {
            int boardY = shape->positionY + y;
            if(shape->tiles[y * shape->xSize + x] == 1)
            {
                gameState->board[boardY][boardX] = 1;
                gameState->boardColors[boardY][boardX] = shape->color;
            }
        }
    }
    
    int rows = 0;
    for(int y = 19; y >= 0; --y)
    {
        int tilesCounter = 0;
        for(int x = 0; x < 10; ++x)
        {
            if(gameState->board[y][x] == 1)
            {
                tilesCounter += 1;
            }
        }
        
        if(tilesCounter == 10)
        {
            ++rows;
            for(int i = 0; i < 4; ++i)
            {
                if(gameState->rowsToDelete[i] == -1)
                {
                    gameState->rowsToDelete[i] = y;
                    break;
                }
            }
        }
    }
    if(gameState->rowsToDelete[0] > -1)
    {
        if(rows == 4)
        {
            gameState->score += 800;
        }
        else
        {
            gameState->score += rows * 100;
        }
        gameState->startDeleteAnimation = true;
    }
}

void DeleteRows(GameState* gameState)
{
    for(int i = 0; i < 4; ++i)
    {
        if(gameState->rowsToDelete[i] > -1)
        {
            for(int x = 0; x < 10; ++x)
            {
                for(int y = gameState->rowsToDelete[i] - 1; y >= 0; --y)
                {
                    gameState->board[y + 1][x] = gameState->board[y][x];
                    gameState->boardColors[y + 1][x] = gameState->boardColors[y][x];
                }
                gameState->board[0][x] = 0;
                gameState->boardColors[0][x] = DARKGRAY;
            }
            
            if(i < 3 && gameState->rowsToDelete[i + 1] > -1)
            {
                for(int j = i + 1; j < 4; ++j)
                {
                    gameState->rowsToDelete[j] += 1;
                }
            }
            gameState->rowsToDelete[i] = -1;
        }
    }
    gameState->deleteRows = false;
}

void DrawShape(Shape* shape)
{
    for(int x = 0; x < shape->xSize; ++x)
    {
        for(int y = 0; y < shape->ySize; ++y)
        {
            if(shape->tiles[y * shape->xSize + x] == 1)
            {
                DrawRectangle(4 + shape->positionX * 64 + x * 64, shape->positionY * 64 + y * 64, 56, 56, shape->color);
            }
        }
    }
}

void DrawNextShape(Shape* shape)
{
    int positionX = 640 + (400 - shape->xSize * 64) / 2;
    for(int x = 0; x < shape->xSize; ++x)
    {
        for(int y = 0; y < shape->ySize; ++y)
        {
            if(shape->tiles[y * shape->xSize + x] == 1)
            {
                DrawRectangle(4 + positionX + x * 64, 440 + y * 64, 56, 56, shape->color);
            }
        }
    }
}

void DrawBoard(GameState* gameState)
{
    for(int x = 0; x < 10; ++x)
    {
        for(int y = 0; y < 20; ++y)
        {
            if(gameState->board[y][x] == 1)
            {
                DrawRectangle(4 + x * 64, y * 64, 56, 56, gameState->boardColors[y][x]);
            }
        }
    }
}

void AnimateRows(GameState* gameState)
{
    ++(gameState->animationCounter);
    
    for(int i = 0; i < 4; ++i)
    {
        if(gameState->rowsToDelete[i] > -1)
        {
            for(int x = 0; x < 10; ++x)
            {
                Color oldColor = gameState->boardColors[gameState->rowsToDelete[i]][x];
                unsigned char r = WHITE.r - oldColor.r;
                unsigned char g = WHITE.g - oldColor.g;
                unsigned char b = WHITE.b - oldColor.b;
                oldColor.r += (gameState->animationCounter / 30.0) * r;
                oldColor.g += (gameState->animationCounter / 30.0) * g;
                oldColor.b += (gameState->animationCounter / 30.0) * b;
                gameState->boardColors[gameState->rowsToDelete[i]][x] = oldColor;
            }
        }
    }
    
    if(gameState->animationCounter == 30)
    {
        gameState->startDeleteAnimation = false;
        gameState->deleteRows = true;
        gameState->animationCounter = 0;
    }
}

void TimeToString(GameState* gameState, char* destination)
{
    int difference = time(0) - gameState->startTime;
    int minutes = difference / 60;
    int seconds = difference % 60;
    char buffer[10];
    sprintf(buffer, "%d", minutes);
    strcat(destination, buffer);
    strcat(destination, ":");
    if(seconds < 10)
    {
        strcat(destination, "0");
    }
    sprintf(buffer, "%d", seconds);
    strcat(destination, buffer);
}

int main(void)
{
    const int screenWidth = 1030;
    const int screenHeight = 1280;
    InitWindow(screenWidth, screenHeight, "Raylib Tetris Test");
    InitAudioDevice();
    
    Sound collisionSound = LoadSound("../resources/collision.wav");
    Sound rowSound = LoadSound("../resources/row.wav");
    
    SetTargetFPS(60);
    GameState* gameState = InitGameState();
    
    Shape* currentShape = CreateShape(GetRandomValue(0, 6));
    Shape* nextShape = CreateShape(GetRandomValue(0, 6));
    
    int frameCounter = 0;
    char scoreString[12];
    char gameTime[10];
    sprintf(scoreString, "%d", gameState->score);
    
    time(&(gameState->startTime));
    
    while (!WindowShouldClose())
    {
        gameTime[0] = '\0';
        TimeToString(gameState, gameTime);
        ++frameCounter;
        if(frameCounter % 30 == 0)
        {
            if(WillShapeColide(currentShape, 0, 1, gameState))
            {
                PlaySound(collisionSound);
                UpdateBoard(currentShape, gameState);
                sprintf(scoreString, "%d", gameState->score);
                FreeShape(currentShape);
                
                currentShape = nextShape;
                nextShape = CreateShape(GetRandomValue(0, 6));
            }
            else
            {
                currentShape->positionY += 1;
            }
            frameCounter = 0;
        }
        
        if(gameState->deleteRows)
        {
            PlaySound(rowSound);
            DeleteRows(gameState);
        }
        
        if(gameState->startDeleteAnimation)
        {
            AnimateRows(gameState);
        }
        
        if(IsKeyPressed(KEY_SPACE))
        {
            RotateShape(currentShape, gameState);
        }
        
        if(IsKeyPressed(KEY_RIGHT))
        {
            if(!WillShapeColide(currentShape, 1, 0, gameState))
            {
                currentShape->positionX += 1;
            }
        }
        if(IsKeyPressed(KEY_LEFT))
        {
            if(!WillShapeColide(currentShape, -1, 0, gameState))
            {
                currentShape->positionX -= 1;
            }
        }
        if(IsKeyPressed(KEY_DOWN))
        {
            for(int i = currentShape->positionY + 1; i <= 20; ++i)
            {
                if(WillShapeColide(currentShape, 0, i - currentShape->positionY, gameState))
                {
                    currentShape->positionY = i - 1;
                    break;
                }
            }
        }
        BeginDrawing();
        
        ClearBackground((Color){30, 30, 30, 255});
        DrawRectangle(640, 0, 10, 1280, (Color){60, 60, 60, 255});
        DrawText("TETRIS", 720, 100, 60, RAYWHITE);
        DrawBoard(gameState); 
        DrawText("NEXT SHAPE", 706, 350, 40, RAYWHITE);  
        DrawShape(currentShape);
        DrawNextShape(nextShape);
        
        DrawText("SCORE", 772, 800, 40, RAYWHITE);
        
        DrawText(scoreString, 840 - (MeasureText(scoreString, 40) / 2), 860, 40, RAYWHITE);
        DrawText("TIME", 788, 1000, 40, RAYWHITE);
        DrawText(gameTime, 840 - (MeasureText(gameTime, 40) / 2), 1060, 40, RAYWHITE);
        
        EndDrawing();
    }
    
    free(gameState);
    free(nextShape);
    free(currentShape);
    
    StopSoundMulti();
    UnloadSound(collisionSound);
    UnloadSound(rowSound);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}