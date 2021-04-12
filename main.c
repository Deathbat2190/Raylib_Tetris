#include "raylib.h"
#include <stdlib.h>
#include <string.h>

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

void RotateShape(Shape* shape)
{
    shape->positionX += rotationTransitions[shape->type][shape->rotation][0];
    shape->positionY += rotationTransitions[shape->type][shape->rotation][1];
    shape->rotation = (shape->rotation + 1) % 4;
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

    free(oldTiles);
}

void DrawShape(Shape* shape)
{
    for(int x = 0; x < shape->xSize; ++x)
    {
        for(int y = 0; y < shape->ySize; ++y)
        {
            if(shape->tiles[y * shape->xSize + x] == 1)
            {
                DrawRectangle(4 + shape->positionX * 72 + x * 72, shape->positionY * 72 + y * 72, 64, 64, shape->color);
            }
        }
    }
}

void DrawBoard(int board[20][10], Color colors[20][10])
{
    for(int x = 0; x < 10; ++x)
    {
        for(int y = 0; y < 20; ++y)
        {
            if(board[y][x] == 1)
            {
                DrawRectangle(4 + x * 72, y * 72, 64, 64, colors[y][x]);
            }
        }
    }
}

int main(void)
{
    const int screenWidth = 720;
    const int screenHeight = 1280;

    InitWindow(screenWidth, screenHeight, "Raylib Tetris Test");

    SetTargetFPS(60);

    int board[20][10] = {0};
    Color boardColors[20][10];

    Shape* currentShape = CreateShape(GetRandomValue(0, 6));
    Shape* nextShape;

    int frameCounter = 0;
    // Main game loop
    while (!WindowShouldClose())
    {
        ++frameCounter;
        if(frameCounter % 60 == 0)
        {
            currentShape->positionY += 1;
            // RotateShape(currentShape);
        }

        if(currentShape->positionY >= 20 - currentShape->ySize - 1)
        {
            FreeShape(currentShape);
            currentShape = CreateShape(GetRandomValue(0, 6));
        }
        
        if(IsKeyPressed(KEY_SPACE))
        {
            RotateShape(currentShape);
        }

        if(IsKeyPressed(KEY_RIGHT))
        {
            if(currentShape->positionX + currentShape->xSize < 10)
            {
                currentShape->positionX += 1;
            }
        }
        if(IsKeyPressed(KEY_LEFT))
        {
            if(currentShape->positionX - 1 >= 0)
            {
                currentShape->positionX -= 1;
            }
        }
        BeginDrawing();

        ClearBackground(DARKGRAY);
        DrawBoard(board, boardColors); 
  
        DrawShape(currentShape);
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}