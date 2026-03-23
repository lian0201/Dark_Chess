#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "raylib.h"

// 定義常數
#define ROWS 4
#define COLS 8
#define CELL_SIZE 100
#define INFO_BAR_HEIGHT 80 
#define SCREEN_WIDTH (COLS * CELL_SIZE)
#define SCREEN_HEIGHT (ROWS * CELL_SIZE + INFO_BAR_HEIGHT) 

typedef struct {
    int type;       
    Color color;    
    bool isRevealed;
} Piece;

Piece board[ROWS][COLS];
int turn = 0; 
int framesCounter = 0; 
long gameStartTick = 0; 
int moveCount = 0; 

// --- 字型全域變數 ---
Font chineseFont; 

// 取得棋子對應的中文字
const char* GetPieceName(int type) {
    switch(type) {
        case 0: return "帥"; case 1: return "仕"; case 2: return "相";
        case 3: return "車"; case 4: return "馬"; case 5: return "炮"; case 6: return "兵";
        case 7: return "將"; case 8: return "士"; case 9: return "象";
        case 10: return "車"; case 11: return "馬"; case 12: return "包"; case 13: return "卒";
        default: return "?";
    }
}

void InitGame() {
    Piece deck[32];
    int index = 0;
    
    // 紅方
    for (int i = 0; i < 1; i++) deck[index++] = (Piece){0, RED, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){1, RED, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){2, RED, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){3, RED, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){4, RED, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){5, RED, false}; 
    for (int i = 0; i < 5; i++) deck[index++] = (Piece){6, RED, false}; 

    // 黑方
    for (int i = 0; i < 1; i++) deck[index++] = (Piece){7, BLACK, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){8, BLACK, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){9, BLACK, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){10, BLACK, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){11, BLACK, false}; 
    for (int i = 0; i < 2; i++) deck[index++] = (Piece){12, BLACK, false}; 
    for (int i = 0; i < 5; i++) deck[index++] = (Piece){13, BLACK, false}; 

    srand(time(NULL));
    for (int i = 31; i > 0; i--) {
        int j = rand() % (i + 1);
        Piece temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }

    index = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c] = deck[index++];
        }
    }

    gameStartTick = (long)GetTime(); 
    moveCount = 0; 
}

// 載入中文字型
void LoadGameFont() {
    const char* neededChars = "帥仕相車馬炮兵將士象包卒計時回合玩家電腦手數: 0123456789";
    int count = 0;
    int *codepoints = LoadCodepoints(neededChars, &count);
    
    chineseFont = LoadFontEx("chinese.ttf", 160, codepoints, count); 
    
    UnloadCodepoints(codepoints);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dark Chess Game - Chinese Font");
    SetTargetFPS(60);
    
    LoadGameFont(); 
    InitGame();

    while (!WindowShouldClose()) {
        if (turn == 0) { 
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                int mouseX = GetMouseX() / CELL_SIZE;
                int mouseY = (GetMouseY() - INFO_BAR_HEIGHT) / CELL_SIZE; 

                if (mouseY >= 0 && mouseY < ROWS && mouseX >= 0 && mouseX < COLS) {
                    if (!board[mouseY][mouseX].isRevealed) {
                        board[mouseY][mouseX].isRevealed = true;
                        moveCount++; 
                        turn = 1; 
                        framesCounter = 0;
                    }
                }
            }
        } 
        else if (turn == 1) { 
            framesCounter++;
            if (framesCounter > 30) { 
                int hiddenRows[32], hiddenCols[32];
                int hiddenCount = 0;
                
                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        if (!board[r][c].isRevealed) {
                            hiddenRows[hiddenCount] = r;
                            hiddenCols[hiddenCount] = c;
                            hiddenCount++;
                        }
                    }
                }

                if (hiddenCount > 0) {
                    int rIndex = rand() % hiddenCount;
                    board[hiddenRows[rIndex]][hiddenCols[rIndex]].isRevealed = true;
                    moveCount++; 
                    turn = 0; 
                }
            }
        }

        BeginDrawing();
        ClearBackground(BROWN); 

        DrawRectangle(0, 0, SCREEN_WIDTH, INFO_BAR_HEIGHT, DARKGRAY); 

        int currentTime = (int)(GetTime() - gameStartTick);
        const char* timeText = TextFormat("計時: %02d:%02d", currentTime/60, currentTime%60);
        const char* turnText = TextFormat("回合: %s", turn == 0 ? "玩家" : "電腦");
        const char* moveText = TextFormat("手數: %d", moveCount);

        DrawTextEx(chineseFont, timeText, (Vector2){10, INFO_BAR_HEIGHT / 2 - 18}, 36, 2, LIGHTGRAY);
        
        Vector2 turnSize = MeasureTextEx(chineseFont, turnText, 36, 2);
        DrawTextEx(chineseFont, turnText, (Vector2){SCREEN_WIDTH / 2 - turnSize.x / 2, INFO_BAR_HEIGHT / 2 - 18}, 36, 2, LIGHTGRAY);
        
        Vector2 moveSize = MeasureTextEx(chineseFont, moveText, 36, 2);
        DrawTextEx(chineseFont, moveText, (Vector2){SCREEN_WIDTH - moveSize.x - 10, INFO_BAR_HEIGHT / 2 - 18}, 36, 2, LIGHTGRAY);

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                int x = c * CELL_SIZE;
                int y = INFO_BAR_HEIGHT + r * CELL_SIZE; 

                DrawRectangle(x + 5, y + 5, CELL_SIZE - 10, CELL_SIZE - 10, BEIGE);
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);

                if (!board[r][c].isRevealed) {
                    DrawCircle(x + CELL_SIZE/2, y + CELL_SIZE/2, CELL_SIZE/2 - 10, DARKGRAY);
                } else {
                    DrawCircle(x + CELL_SIZE/2, y + CELL_SIZE/2, CELL_SIZE/2 - 10, BEIGE);
                    DrawCircleLines(x + CELL_SIZE/2, y + CELL_SIZE/2, CELL_SIZE/2 - 10, board[r][c].color);
                    
                    const char* pieceName = GetPieceName(board[r][c].type);
                    Vector2 textSize = MeasureTextEx(chineseFont, pieceName, 54, 2);
                    Vector2 textPos = { x + CELL_SIZE/2 - textSize.x/2, y + CELL_SIZE/2 - textSize.y/2 };
                    
                    DrawTextEx(chineseFont, pieceName, textPos, 54, 2, board[r][c].color);
                }
            }
        }

        EndDrawing();
    }
    
    UnloadFont(chineseFont); 
    CloseWindow();
    return 0;
}