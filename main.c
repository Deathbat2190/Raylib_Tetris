#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

int rotationTransitions[7][4][2] = {{{2, -1}, {-2, 2}, {1, -2}, {-1, 1}},  //i
                                    {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //j
                                    {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //l
                                    {{0, 0},  {0, 0},  {0, 0},  {0, 0}},   //o
                                    {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //s
                                    {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},   //t
                                    {{1, 0},  {-1, 1}, {0, -1}, {0, 0}},}; //z

int board[20][10] = {0};
Color boardColors[20][10];
int rowsToDelete[4] = {-1, -1, -1, -1};
bool deleteRows = false;
bool WillShapeColide(Shape* shape, int moveX, int moveY)
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
                   board[boardY][boardX] == 1)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void RotateShape(Shape* shape)
{
    int moveX = rotationTransitions[shape->type][shape->rotation][0];
    int moveY = rotationTransitions[shape->type][shape->rotation][1];

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
    
    if(!WillShapeColide(shape, moveX, moveY))
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

void UpdateBoard(Shape* shape)
{
    for(int x = 0; x < shape->xSize; ++x)
    {
        int boardX = shape->positionX + x;
        for(int y = 0; y < shape->ySize; ++y)
        {
            int boardY = shape->positionY + y;
            if(shape->tiles[y * shape->xSize + x] == 1)
            {
                board[boardY][boardX] = 1;
                boardColors[boardY][boardX] = shape->color;
            }
        }
    }

    for(int y = 19; y >= 0; --y)
    {
        int tilesCounter = 0;
        for(int x = 0; x < 10; ++x)
        {
            if(board[y][x] == 1)
            {
                tilesCounter += 1;
            }
        }

        if(tilesCounter == 10)
        {
            deleteRows = true;
            for(int i = 0; i < 4; ++i)
            {
                if(rowsToDelete[i] == -1)
                {
                    rowsToDelete[i] = y;
                    break;
                }
            }
        }
    }
}

void DeleteRows()
{
    for(int i = 0; i < 4; ++i)
    {
        if(rowsToDelete[i] > -1)
        {
            for(int x = 0; x < 10; ++x)
            {
                for(int y = rowsToDelete[i] - 1; y >= 0; --y)
                {
                    board[y + 1][x] = board[y][x];
                    boardColors[y + 1][x] = boardColors[y][x];
                }
                board[0][x] = 0;
                boardColors[0][x] = DARKGRAY;
            }

            if(i < 3 && rowsToDelete[i + 1] > -1)
            {
                for(int j = i + 1; j < 4; ++j)
                {
                    rowsToDelete[j] += 1;
                }
            }
            rowsToDelete[i] = -1;
        }
    }
    deleteRows = false;
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

void DrawBoard()
{
    for(int x = 0; x < 10; ++x)
    {
        for(int y = 0; y < 20; ++y)
        {
            if(board[y][x] == 1)
            {
                DrawRectangle(4 + x * 64, y * 64, 56, 56, boardColors[y][x]);
            }
        }
    }
}

int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 1280;
    InitWindow(screenWidth, screenHeight, "Raylib Tetris Test");

    SetTargetFPS(60);

    Shape* currentShape = CreateShape(GetRandomValue(0, 6));
    Shape* nextShape;

    int frameCounter = 0;
    // Main game loop
    while (!WindowShouldClose())
    {
        ++frameCounter;
        if(frameCounter % 30 == 0)
        {
            if(WillShapeColide(currentShape, 0, 1))
            {
                UpdateBoard(currentShape);
                if(deleteRows)
                {
                    DeleteRows();
                }
                FreeShape(currentShape);
                currentShape = CreateShape(GetRandomValue(0, 6));
            }
            else
            {
                currentShape->positionY += 1;
            }
            
        }
     
        if(IsKeyPressed(KEY_SPACE))
        {
            RotateShape(currentShape);
        }

        if(IsKeyPressed(KEY_RIGHT))
        {
            if(!WillShapeColide(currentShape, 1, 0))
            {
                currentShape->positionX += 1;
            }
        }
        if(IsKeyPressed(KEY_LEFT))
        {
            if(!WillShapeColide(currentShape, -1, 0))
            {
                currentShape->positionX -= 1;
            }
        }
        if(IsKeyPressed(KEY_DOWN))
        {
            for(int i = currentShape->positionY + 1; i <= 20; ++i)
            {
                if(WillShapeColide(currentShape, 0, i - currentShape->positionY))
                {
                    currentShape->positionY = i - 1;
                    break;
                }
            }
        }
        BeginDrawing();

        ClearBackground(DARKGRAY);
        DrawBoard(); 
  
        DrawShape(currentShape);
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}