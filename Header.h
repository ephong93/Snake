#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <string.h>


char** Display;
char** DisplayBuffer;

typedef struct object {
    char symbol;
    COORD coord;
    int blockType;
    struct object* next;
} Object;

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum BlockType
{
    BACKGROUND,
    WALL,
    SNAKE,
    ITEM
};


typedef struct params {
    double timeList[100];
    int timeListLength;
    int BACKGROUND_SIZE_X;
    int BACKGROUND_SIZE_Y;
    char BACKGROUND_SYMBOL;
    char SNAKE_SYMBOL;
    char WALL_SYMBOL;
    char ITEM_SYMBOL;
} Params;


Params params;
Object* snake = NULL;
Object* item = NULL;
int point = 0;

Object* makeSnakeBlock(int, int);
Object* makeSnake(int, int);
Object* makeItem();
void init();
void initDisplay();
void loadParams();
void moveSnake(int, int);
void gotoxy(int, int);
void draw(int, int, char, int);
void drawBlock(Object*);
void drawBackground();
void drawSnake();
void drawItem();
int collide(Object*, int);
void displayPoint();
void update();
void lose();



void init()
{
    loadParams();
    initDisplay();
    drawBackground();
    point = 0;
    snake = makeSnake((int)(params.BACKGROUND_SIZE_X / 2), (int)(params.BACKGROUND_SIZE_Y / 2));
    item = makeItem();
    update();
}


void loadParams()
{
    FILE* settingFile = fopen("setting.ini", "r");
    if (settingFile == NULL)
        printf("NO FILE EXISTS\n");
    char line[100];

    params.BACKGROUND_SYMBOL = ' ';

    while(fgets(line, 100, settingFile) != NULL)
    {   
        char* ptr = strtok(line, " \n");
        
        if (ptr == NULL) continue;

        if (strcmp(ptr, "BACKGROUND_SIZE_X") == 0)
        {
            ptr = strtok(NULL, " \n");
            if (ptr == NULL) {
                printf("\n설정 오류\n");
                fprintf(stderr, "SETTING HAS NO BACKGROUND_SIZE_X ATTRIBUTE");
                break;
            }
            printf("%s\n", ptr);
            params.BACKGROUND_SIZE_X = atoi(ptr);
        }
        else if (strcmp(ptr, "BACKGROUND_SIZE_Y") == 0)
        {
            ptr = strtok(NULL, " \n");
            if (ptr == NULL) {
                printf("\n설정 오류\n");
                fprintf(stderr, "SETTING HAS NO BACKGROUND_SIZE_Y ATTRIBUTE");
                break;
            }
            params.BACKGROUND_SIZE_Y = atoi(ptr);
        }
        else if (strcmp(ptr, "WALL_SYMBOL") == 0)
        {
            ptr = strtok(NULL, " \n");
            if (ptr == NULL) {
                printf("\n설정 오류\n");
                fprintf(stderr, "SETTING HAS NO WALL_SYMBOL ATTRIBUTE");
                break;
            }
            params.WALL_SYMBOL = ptr[0];
        }
        else if (strcmp(ptr, "SNAKE_SYMBOL") == 0)
        {
            ptr = strtok(NULL, " \n");
            if (ptr == NULL) {
                printf("\n설정 오류\n");
                fprintf(stderr, "SETTING HAS NO SNAKE_SYMBOL ATTRIBUTE");
                break;
            }
            else {
                params.SNAKE_SYMBOL = ptr[0];
            }
        }
        else if (strcmp(ptr, "ITEM_SYMBOL") == 0)
        {
            ptr = strtok(NULL, " \n");
            if (ptr == NULL) {
                printf("\n설정 오류\n");
                fprintf(stderr, "SETTING HAS NO ITEM_SYMBOL ATTRIBUTE");
                break;
            }
            else {
                params.ITEM_SYMBOL = ptr[0];
            }
        }
        else if (strcmp(ptr, "SPEED") == 0) {
            //printf("A");
            params.timeListLength = 0;
            while(1)
            {
                //printf("%d\n", params.timeListLength);
                ptr = strtok(NULL, " \n");
                if (ptr == NULL) {
                    break;
                }
                else {
                    params.timeList[params.timeListLength] = atof(ptr);
                    params.timeListLength++;
                }
            }
        }
        else
        {
            continue;
        }
    }

}


void initDisplay()
{
    Display = (char**)malloc(sizeof(char*) * params.BACKGROUND_SIZE_X);
    DisplayBuffer = (char**)malloc(sizeof(char*) * params.BACKGROUND_SIZE_X);

    for (int i = 0; i < params.BACKGROUND_SIZE_X; i++)
    {
        Display[i] = (char*)malloc(sizeof(char) * params.BACKGROUND_SIZE_Y);
        DisplayBuffer[i] = (char*)malloc(sizeof(char) * params.BACKGROUND_SIZE_Y);
        for (int j = 0; j < params.BACKGROUND_SIZE_Y; j++)
        {
            Display[i][j] = DisplayBuffer[i][j] = BACKGROUND;
        }
    }
}

void drawBackground()
{
    for (int i = 0; i < params.BACKGROUND_SIZE_X; i++)
    {
        for (int j = 0; j < params.BACKGROUND_SIZE_Y; j++)
        {
            if (i == 0 || i == params.BACKGROUND_SIZE_X - 1 || j == 0 || j == params.BACKGROUND_SIZE_Y - 1)
            {
                draw(i, j, params.WALL_SYMBOL, WALL);
            }
            else
            {
                draw(i, j, ' ', 0);
            }
        }
    }
}


Object* makeItem()
{
    if (item == NULL)
    {
        item = (Object*)malloc(sizeof(Object));
    }
    srand((unsigned int)time(NULL));
    int x, y;
    do {
        x = rand() % (params.BACKGROUND_SIZE_X - 1) + 1;
        y = rand() % (params.BACKGROUND_SIZE_Y - 1) + 1;
    } while (DisplayBuffer[x][y]);

    item->coord.X = x;
    item->coord.Y = y;
    item->symbol = params.ITEM_SYMBOL;
    item->blockType = ITEM;
    item->next = NULL;
    return item;
}


Object* makeSnakeBlock(int x, int y)
{
    Object* head = (Object*)malloc(sizeof(Object));
    head->symbol = params.SNAKE_SYMBOL;
    head->coord.X = x;
    head->coord.Y = y;
    head->blockType = SNAKE;
    head->next = NULL;
    return head;
}

Object* makeSnake(int x, int y)
{
    Object* head = makeSnakeBlock(x, y);
    Object* body = makeSnakeBlock(x - 1, y);
    head->next = body;
    return head;
}

void moveSnake(int dir, int expand)
{
    COORD currentCoord, nextCoord;
    currentCoord.X = snake->coord.X;
    currentCoord.Y = snake->coord.Y;

    switch (dir)
    {
    case UP:
        snake->coord.Y--;
        break;
    case DOWN:
        snake->coord.Y++;
        break;
    case LEFT:
        snake->coord.X--;
        break;
    case RIGHT:
        snake->coord.X++;
        break;
    default:
        break;
    }


    Object* currentBlock = snake;
    while (currentBlock->next != NULL)
    {
        nextCoord.X = currentBlock->next->coord.X;
        nextCoord.Y = currentBlock->next->coord.Y;
        currentBlock->next->coord.X = currentCoord.X;
        currentBlock->next->coord.Y = currentCoord.Y;

        currentCoord.X = nextCoord.X;
        currentCoord.Y = nextCoord.Y;
        currentBlock = currentBlock->next;
    }

    if (expand)
    {
        Object* newBlock = makeSnakeBlock(currentCoord.X, currentCoord.Y);
        currentBlock->next = newBlock;
    }
}



void gotoxy(int x, int y)
{
    COORD Pos;
    Pos.X = x;
    Pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}


void draw(int x, int y, char symbol, int filling)
{
    DisplayBuffer[x][y] = filling;
}

void drawBlock(Object* block)
{
    draw(block->coord.X, block->coord.Y, block->symbol, block->blockType);
}





void drawSnake()
{
    Object* block = snake;
    while (block != NULL)
    {
        drawBlock(block);
        block = block->next;
    }
}

void drawItem()
{
    drawBlock(item);
}


int collide(Object* snake, int dir)
{
    COORD coord;

    coord.X = snake->coord.X;
    coord.Y = snake->coord.Y;

    switch (dir)
    {
    case UP:
        coord.Y--;
        break;
    case DOWN:
        coord.Y++;
        break;
    case LEFT:
        coord.X--;
        break;
    case RIGHT:
        coord.X++;
        break;
    }

    return DisplayBuffer[coord.X][coord.Y];
}


void remove_snake()
{
    Object* snakeBlock = snake;
    while (snakeBlock != NULL)
    {
        Object* nextBlock = snakeBlock->next;
        free(snakeBlock);
        snakeBlock = nextBlock;
    }
    snake = NULL;
}

void remove_item()
{
    if (item != NULL)
        free(item);
    item = NULL;
}


void remove_filling()
{
    for (int i = 0; i < params.BACKGROUND_SIZE_X; i++)
    {
        for (int j = 0; j < params.BACKGROUND_SIZE_Y; j++)
        {
            DisplayBuffer[i][j] = BACKGROUND;
        }
    }
}


void displayPoint()
{
    gotoxy(params.BACKGROUND_SIZE_X + 2, 0);
    printf("Point: %d          ", point);
}


void flushDisplayBuffer()
{
    for (int i = 0; i < params.BACKGROUND_SIZE_X; i++)
    {
        for (int j = 0; j < params.BACKGROUND_SIZE_Y; j++)
        {
            if (Display[i][j] != DisplayBuffer[i][j])
            {
                gotoxy(i, j);
                switch (DisplayBuffer[i][j])
                {
                case BACKGROUND:
                    printf("%c", params.BACKGROUND_SYMBOL);
                    break;
                case WALL:
                    printf("%c", params.WALL_SYMBOL);
                    break;
                case SNAKE:
                    printf("%c", params.SNAKE_SYMBOL);
                    break;
                case ITEM:
                    printf("%c", params.ITEM_SYMBOL);
                    break;
         
                }
                Display[i][j] = DisplayBuffer[i][j];
            }
        }
    }
}


void update()
{
    drawBackground();
    drawSnake();
    drawItem();
    displayPoint();
    flushDisplayBuffer();


    
}


