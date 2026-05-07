#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <imm.h>

#define WIDTH 20
#define HEIGHT 20

typedef struct {
    int x, y;
} Position;

typedef struct Node {
    Position pos;
    struct Node *next;
} Node;

Node *snake = NULL;
Position food;
int gameOver = 0;
int score = 0;

// === 游標定位與隱藏 ===
void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

// === 主要函式宣告 ===
void initializeGame();
void generateFood();
void drawGame();
void updateGame(char direction);
void saveScore();
void endGame();
void restartGame();
void freeSnake();

void setEnglishInput() {
    HKL hkl = GetKeyboardLayout(0);
    if (LOWORD(hkl) != 0x0409) {
        HKL newHkl = LoadKeyboardLayout("00000409", KLF_ACTIVATE);
        ActivateKeyboardLayout(newHkl, 0);
    }
}

int main() {
    setEnglishInput();
    hideCursor();  // 隱藏游標，減少閃爍
    srand(time(NULL));

    char direction = 'w';
    initializeGame();

    while (!gameOver) {
        if (_kbhit()) {
            char newDir = _getch();
            if ((direction == 'w' && newDir != 's') ||
                (direction == 's' && newDir != 'w') ||
                (direction == 'a' && newDir != 'd') ||
                (direction == 'd' && newDir != 'a')) {
                direction = newDir;
            }
        }
        updateGame(direction);
        drawGame();
        Sleep(100);
    }

    return 0;
}

void initializeGame() {
    score = 0;
    gameOver = 0;

    Node *head = (Node *)malloc(sizeof(Node));
    head->pos.x = WIDTH / 2;
    head->pos.y = HEIGHT / 2;
    head->next = NULL;
    snake = head;

    generateFood();
}

void generateFood() {
    int valid = 0;
    while (!valid) {
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;

        valid = 1;
        Node *current = snake;
        while (current != NULL) {
            if (current->pos.x == food.x && current->pos.y == food.y) {
                valid = 0;
                break;
            }
            current = current->next;
        }
    }
}

void drawBorder() {
    for (int i = 0; i < WIDTH + 2; i++) printf("*");
    printf("\n");
}

void drawGame() {
    gotoxy(0, 0);  // 替代 system("cls")
    printf("======================\n");
    printf("      SNAKE GAME      \n");
    printf("======================\n");
    printf("Score: %d\n", score);

    drawBorder();
    for (int y = 0; y < HEIGHT; y++) {
        printf("*");
        for (int x = 0; x < WIDTH; x++) {
            int isSnake = 0;
            int isHead = 1;
            Node *current = snake;
            while (current != NULL) {
                if (current->pos.x == x && current->pos.y == y) {
                    isSnake = 1;
                    if (isHead) {
                        printf("@");  // 頭
                    } else {
                        printf("#");  // 身體
                    }
                    break;
                }
                current = current->next;
                isHead = 0;
            }

            if (!isSnake) {
                if (x == food.x && y == food.y) {
                    printf("F");
                } else {
                    printf(" ");
                }
            }
        }
        printf("*\n");
    }
    drawBorder();
}

void updateGame(char direction) {
    Node *newHead = (Node *)malloc(sizeof(Node));
    newHead->pos = snake->pos;

    switch (direction) {
        case 'w': newHead->pos.y--; break;
        case 's': newHead->pos.y++; break;
        case 'a': newHead->pos.x--; break;
        case 'd': newHead->pos.x++; break;
    }

    // 撞牆
    if (newHead->pos.x < 0 || newHead->pos.x >= WIDTH || newHead->pos.y < 0 || newHead->pos.y >= HEIGHT) {
        free(newHead);
        gameOver = 1;
        endGame();
        return;
    }

    // 撞自己
    Node *check = snake;
    while (check != NULL) {
        if (check->pos.x == newHead->pos.x && check->pos.y == newHead->pos.y) {
            free(newHead);
            gameOver = 1;
            endGame();
            return;
        }
        check = check->next;
    }

    newHead->next = snake;
    snake = newHead;

    if (snake->pos.x == food.x && snake->pos.y == food.y) {
        score += 10;
        generateFood();
    } else {
        Node *current = snake;
        while (current->next->next != NULL) {
            current = current->next;
        }
        free(current->next);
        current->next = NULL;
    }
}

void saveScore() {
    FILE *file = fopen("score.txt", "a");
    if (file) {
        fprintf(file, "Score: %d\n", score);
        fclose(file);
    }
}

void endGame() {
    saveScore();
    drawGame();
    printf("======================\n");
    printf("      GAME OVER       \n");
    printf("======================\n");
    printf("Final Score: %d\n", score);
    printf("Press 'r' to Restart or any other key to Exit.\n");

    char choice = _getch();
    if (choice == 'r') {
        restartGame();
    } else {
        freeSnake();
        exit(0);
    }
}

void restartGame() {
    freeSnake();
    initializeGame();
}

void freeSnake() {
    Node *current = snake;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    snake = NULL;
}

