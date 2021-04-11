#include "raylib.h"
#include <stdlib.h>

typedef struct
{
    int rotation;
    int* tiles;
    int xSize;
    int ySize;
    int positionX;
    int positionY;
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

Shape* CreateShape(char type)
{
    Shape* shape = NULL;
    switch (type)
    {
        case 'o':
            shape = InitShape(2, 2);
            shape->tiles[0] = 1; shape->tiles[1] = 1; 
            shape->tiles[2] = 1; shape->tiles[3] = 1;
            shape->positionX = 4;
            break;
        case 'j':
            shape = InitShape(3, 2);
            shape->tiles[0] = 1; shape->tiles[1] = 0; shape->tiles[2] = 0; 
            shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 1;
            break;
        case 'l':
            shape = InitShape(3, 2);
            shape->tiles[0] = 0; shape->tiles[1] = 0; shape->tiles[2] = 1; 
            shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 1;
            break;
        case 's':
            shape = InitShape(3, 2);
            shape->tiles[0] = 0; shape->tiles[1] = 1; shape->tiles[2] = 1; 
            shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 0;
            break;
        case 'z':
            shape = InitShape(3, 2);
            shape->tiles[0] = 1; shape->tiles[1] = 1; shape->tiles[2] = 0; 
            shape->tiles[3] = 0; shape->tiles[4] = 1; shape->tiles[5] = 1;
            break;
        case 't':
            shape = InitShape(3, 2);
            shape->tiles[0] = 0; shape->tiles[1] = 1; shape->tiles[2] = 0; 
            shape->tiles[3] = 1; shape->tiles[4] = 1; shape->tiles[5] = 1;
            break;
        case 'i':
            shape = InitShape(4, 1);
            shape->tiles[0] = 1; shape->tiles[1] = 1; shape->tiles[2] = 1; shape->tiles[3] = 1;
            break;
    }
    return shape;
}

void FreeShape(Shape* shape)
{
    free(shape->tiles);
    free(shape);
}

void DrawShape(Shape* shape)
{
    for(int x = 0; x < shape->xSize; ++x)
    {
        for(int y = 0; y < shape->ySize; ++y)
        {
            if(shape->tiles[y * shape->xSize + x] == 1)
            {
                DrawRectangle(4 + shape->positionX * 72 + x * 72, shape->positionY * 72 + y * 72, 64, 64, BLACK);
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
            if(board[y][x] == 0)
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
    char shapes[7] = {'o', 'l', 'j', 's', 'z', 't', 'i'};

    int board[20][10] = {0};
    Color boardColors[20][10];

    Shape* currentShape = CreateShape('o');
    Shape* nextShape;

    for(int i = 0; i < 20; ++i)
    {
        for(int j = 0; j < 10; ++j)
        {
            boardColors[i][j] = RED;
        }
    }
    int frameCounter = 0;
    int currentShapeIndex = 0;
    // Main game loop
    while (!WindowShouldClose())
    {
        ++frameCounter;
        if(frameCounter % 60 == 0)
        {
            currentShape->positionY += 1;
        }

        if(currentShape->positionY == 20 - currentShape->ySize - 1)
        {
            ++currentShapeIndex;
            if(currentShapeIndex == 7)
            {
                currentShapeIndex = 0;
            }
            FreeShape(currentShape);
            currentShape = CreateShape(shapes[currentShapeIndex]);
        }
        
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawBoard(board, boardColors);
        // for(int i = 0; i < 7; ++i)
        // {
        //     Shape* shape = CreateShape(shapes[i]);
        //     DrawShape(shape);
        //     FreeShape(shape);
        // }
        
        DrawShape(currentShape);
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}