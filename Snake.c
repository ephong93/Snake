#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "Header.h"



void startGame() 
{
    clock_t start, current;
    system("cls");
    init();

    int timeIndex = 0;
    double* timeList = params.timeList;
    int timeListLength = params.timeListLength;


    start = clock();
    
    int dir = RIGHT;
    int next_dir = dir;
    while (1)
    {
        if (kbhit())
        {
            int keycode = getch();
            if (keycode == 224) // if pressed key is an arrow key
            {
                int dirCode = getch();
                switch (dirCode)
                {
                case 72: // key up
                    next_dir = UP;
                    break;
                case 80: // key down
                    next_dir = DOWN;
                    break;
                case 77: // key right
                    next_dir = RIGHT;
                    break;
                case 75: // key left
                    next_dir = LEFT;
                    break;
                }
            }
        }

        current = clock();
        
        if (((double)current - (double)start) / CLOCKS_PER_SEC > timeList[timeIndex])
        {

            switch (next_dir)
            {
            case UP:
                if (dir == DOWN) break;
                dir = next_dir;
                break;
            case DOWN:
                if (dir == UP) break;
                dir = next_dir;
                break;
            case RIGHT:
                if (dir == LEFT) break;
                dir = next_dir;
                break;
            case LEFT:
                if (dir == RIGHT) break;
                dir = next_dir;
                break;
            }
            next_dir = dir;
            int blockType;
            blockType = collide(snake, dir);
            if (blockType == WALL || blockType == SNAKE)
            {
                break;
            }
            else if (blockType == ITEM)
            {
                point++;
                moveSnake(dir, 1);
                makeItem();
                update();
                start = clock();

                timeIndex = min(timeListLength-1, timeIndex + 1);
            }
            else
            {
                moveSnake(dir, 0);
                update();
                start = clock();
            }
        }
    }
    lose();
}


void displayMenu(int select, char menu[][100], int menuLength)
{
    gotoxy(0, 10);
    
    for (int i = 0; i < menuLength; i++)
    {
        if (select == i) printf("     > ");
        else printf("       ");

        printf("%s                              \n", menu[i]);
    }
}


void displayRanking()
{
    FILE* rankingFile = fopen("ranking.dat", "r");
    char line[100];

    gotoxy(0, 3);
    printf("*****RANKING*****                     \n");
    while (fgets(line, 100, rankingFile) != NULL)
    {
        char* name;
        int point;
        int rank;
        char* ptr = strtok(line, " ");
        if (ptr == NULL) continue;
        rank = atoi(ptr);
        ptr = strtok(NULL, " ");
        if (ptr == NULL) continue;
        name = ptr;
        ptr = strtok(NULL, " ");
        if (ptr == NULL) continue;
        point = atoi(ptr);

        printf("%d. %s %d                          \n", rank, name, point);
    }
    fclose(rankingFile);
}


void record(int myPoint)
{
    char myName[50];
    system("cls");
    printf("Enter your name: ");
    scanf("%s", myName);

    FILE* rankingFile = fopen("ranking.dat", "r");
    FILE* rankingFile2 = fopen("ranking2.dat", "w");

    printf("MYPOINT: %d    \n", myPoint);


    char line[100];
    int rank = 0;
    int haveWritten = 0;
    int offset = 1;
    while (fgets(line, 100, rankingFile) != NULL)
    {
        int othersPoint;
        char* name;
        char* ptr = strtok(line, " \n");
        if (ptr == NULL) break;
        rank = atoi(ptr);
        ptr = strtok(NULL, " ");
        name = ptr;
        ptr = strtok(NULL, " ");
        othersPoint = atoi(ptr);

        if (othersPoint <= myPoint && !haveWritten) {
            fprintf(rankingFile2, "%d %s %d\n", rank, myName, myPoint);
            haveWritten = 1;
        }

        if (haveWritten)
        {
            if (othersPoint == myPoint)
            {
                fprintf(rankingFile2, "%d %s %d\n", rank, name, othersPoint);
                offset = 0;
            }
            else
            {
                fprintf(rankingFile2, "%d %s %d\n", rank + offset, name, othersPoint);
            }
        }
        else
        {
            fprintf(rankingFile2, "%d %s %d\n", rank, name, othersPoint);
        }
    }

    if (!haveWritten)
        fprintf(rankingFile2, "%d %s %d\n", rank + offset, myName, myPoint);
    fclose(rankingFile);
    fclose(rankingFile2);

    int rem = remove("ranking.dat");
    int ren = rename("ranking2.dat", "ranking.dat");
}

int getRank(int myPoint)
{
    FILE* rankingFile = fopen("ranking.dat", "r");
    char line[100];
    int rank = 0;
    while (fgets(line, 100, rankingFile) != NULL)
    {
        int othersPoint;
        char* name;
        char* ptr = strtok(line, " \n");
        if (ptr == NULL) break;
        rank = atoi(ptr);
        ptr = strtok(NULL, " ");
        name = ptr;
        ptr = strtok(NULL, " ");
        othersPoint = atoi(ptr);

        if (othersPoint <= myPoint) {
            fclose(rankingFile);
            return rank;
        }
    }
    fclose(rankingFile);
    return rank+1;
}

void lose()
{
    remove_snake();
    remove_item();
    remove_filling();
    system("cls");

    int rank = getRank(point);
    int recorded = 0;
    printf("     Point: %d     \n", point);
    printf("     Rank: %d     \n", rank);
    

    displayRanking();

    int select = 0;
    int menuLength = 3;
    char menu[3][100] = { "Play again", "Record", "Exit" };

    displayMenu(select, menu, menuLength);

    while (1)
    {
        if (kbhit())
        {
            int keycode = getch();
            if (keycode == 224)
            {
                int dirCode = getch();
                switch (dirCode)
                {
                case 72:
                    select = max(0, select - 1);
                    break;
                case 80:
                    select = min(menuLength-1, select + 1);
                    break;
                default:
                    break;
                }
                displayMenu(select, menu, menuLength);
            }
            else if (keycode == 32) // Space key down
            {
                if (select == 1 && !recorded)
                {
                    record(point);
                    recorded = 1;
                    displayRanking();
                    displayMenu(select, menu, menuLength);
                }
                else if (select == 0 || select == 2) break;
            }
        }
    }

    if (select == 0) startGame();
    return;
}


int main()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = 0;
    SetConsoleCursorInfo(out, &cursorInfo);


    printf(" ####   #     #       #      #      #  ########    \n");
    printf("#    #  ##    #      # #     #    #    #           \n");
    printf("#       # #   #     #   #    #  #      #           \n");
    printf(" ####   #  #  #    #     #   ## #      ########    \n");
    printf("     #  #   # #   #########  #   #     #           \n");
    printf("#    #  #    ##  #         # #     #   #           \n");
    printf(" ####   #     # #           ##       # ########    \n");
    printf("\n");
    printf("\n");
    printf("Space를 눌러서 시작");
    
    int c1 = getch();
    if (c1 == 32)  // space: 32
    {
        startGame();
        //loadParams();
        //printf("%d %d\n", params.BACKGROUND_SIZE_X, params.BACKGROUND_SIZE_Y);
        //while (1) {}
    }
    return 0;
}


