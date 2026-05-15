/*
 =============================================
   SNAKE GAME IN C++
   Compatible with: Dev-C++, Code::Blocks
   OS: Windows
   Author: Made for CS Programming Fundamentals
 =============================================
*/

#include <iostream>
#include <conio.h>       // For _kbhit() and _getch()
#include <windows.h>     // For Sleep() and system()
#include <cstdlib>       // For rand() and srand()
#include <ctime>         // For time()

using namespace std;

// ─── CONSTANTS ───────────────────────────────
const int WIDTH  = 40;   // Board width
const int HEIGHT = 20;   // Board height
const int MAX_LENGTH = WIDTH * HEIGHT;

// ─── DIRECTION ENUM ──────────────────────────
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

// ─── GLOBAL VARIABLES ────────────────────────
bool   gameOver;
int    snakeX[MAX_LENGTH], snakeY[MAX_LENGTH];  // Snake body positions
int    snakeLen;          // Current length of snake
int    foodX, foodY;      // Food position
int    score;
Direction dir;

// ─── FUNCTION: Move cursor to position ───────
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// ─── FUNCTION: Hide the blinking cursor ──────
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize   = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// ─── FUNCTION: Setup (Initialize game) ───────
void setup() {
    gameOver = false;
    dir      = STOP;
    score    = 0;
    snakeLen = 3;

    // Snake starts in the middle
    snakeX[0] = WIDTH  / 2;
    snakeY[0] = HEIGHT / 2;

    // Initial body
    for (int i = 1; i < snakeLen; i++) {
        snakeX[i] = snakeX[0] + i;
        snakeY[i] = snakeY[0];
    }

    // Spawn first food
    srand((unsigned)time(0));
    foodX = rand() % (WIDTH  - 2) + 1;
    foodY = rand() % (HEIGHT - 2) + 1;
}

// ─── FUNCTION: Draw the board ─────────────────
void draw() {
    // Build board in a 2D char array to avoid flicker
    char board[HEIGHT][WIDTH];

    // Fill with empty spaces
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            board[y][x] = ' ';

    // Draw borders
    for (int x = 0; x < WIDTH; x++) {
        board[0][x]          = '#';
        board[HEIGHT-1][x]   = '#';
    }
    for (int y = 0; y < HEIGHT; y++) {
        board[y][0]         = '#';
        board[y][WIDTH-1]   = '#';
    }

    // Draw food
    board[foodY][foodX] = '@';

    // Draw snake body (from tail to neck)
    for (int i = snakeLen - 1; i >= 1; i--)
        board[snakeY[i]][snakeX[i]] = 'o';

    // Draw snake head
    board[snakeY[0]][snakeX[0]] = 'O';

    // Render board
    gotoxy(0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++)
            cout << board[y][x];
        cout << "\n";
    }

    // Score display
    gotoxy(0, HEIGHT);
    cout << "  Score: " << score
         << "   Length: " << snakeLen
         << "   [Arrow Keys] to move   [ESC/X] to quit   ";
}

// ─── FUNCTION: Handle keyboard input ─────────
// Arrow keys send TWO characters: first 224 (0xE0), then the key code
// UP=72, DOWN=80, LEFT=75, RIGHT=77
void input() {
    if (_kbhit()) {
        char key = _getch();
        if (key == (char)224) {          // Arrow key prefix
            char arrow = _getch();
            switch (arrow) {
                case 72: if (dir != DOWN)  dir = UP;    break;  // Up arrow
                case 80: if (dir != UP)    dir = DOWN;  break;  // Down arrow
                case 75: if (dir != RIGHT) dir = LEFT;  break;  // Left arrow
                case 77: if (dir != LEFT)  dir = RIGHT; break;  // Right arrow
            }
        } else {
            // Also keep WASD as backup + ESC/X to quit
            switch (key) {
                case 'w': case 'W': if (dir != DOWN)  dir = UP;    break;
                case 's': case 'S': if (dir != UP)    dir = DOWN;  break;
                case 'a': case 'A': if (dir != RIGHT) dir = LEFT;  break;
                case 'd': case 'D': if (dir != LEFT)  dir = RIGHT; break;
                case 'x': case 'X': case 27: gameOver = true;      break; // X or ESC
            }
        }
    }
}

// ─── FUNCTION: Game logic / movement ─────────
void logic() {
    if (dir == STOP) return;  // Wait until player presses a key

    // Save old tail position
    int prevX = snakeX[snakeLen - 1];
    int prevY = snakeY[snakeLen - 1];

    // Shift body — each segment follows the one ahead
    for (int i = snakeLen - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Move head
    switch (dir) {
        case LEFT:  snakeX[0]--; break;
        case RIGHT: snakeX[0]++; break;
        case UP:    snakeY[0]--; break;
        case DOWN:  snakeY[0]++; break;
        default: break;
    }

    // ── Wall collision ──
    if (snakeX[0] <= 0 || snakeX[0] >= WIDTH - 1 ||
        snakeY[0] <= 0 || snakeY[0] >= HEIGHT - 1) {
        gameOver = true;
        return;
    }

    // ── Self collision ──
    for (int i = 1; i < snakeLen; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameOver = true;
            return;
        }
    }

    // ── Food eaten ──
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        score  += 10;
        snakeLen++;

        // Add new segment at old tail
        snakeX[snakeLen - 1] = prevX;
        snakeY[snakeLen - 1] = prevY;

        // Spawn new food (not on snake)
        bool onSnake = true;
        while (onSnake) {
            foodX = rand() % (WIDTH  - 2) + 1;
            foodY = rand() % (HEIGHT - 2) + 1;
            onSnake = false;
            for (int i = 0; i < snakeLen; i++) {
                if (foodX == snakeX[i] && foodY == snakeY[i]) {
                    onSnake = true;
                    break;
                }
            }
        }
    }
}

// ─── FUNCTION: Show Game Over screen ─────────
void showGameOver() {
    system("cls");
    cout << "\n\n";
    cout << "   ================================\n";
    cout << "           GAME OVER!\n";
    cout << "   ================================\n\n";
    cout << "   Final Score  : " << score    << "\n";
    cout << "   Snake Length : " << snakeLen << "\n\n";
    cout << "   Press [R] to Restart\n";
    cout << "   Press [Q] to Quit\n\n";
    cout << "   ================================\n";
}

// ─── MAIN FUNCTION ────────────────────────────
int main() {
    // Window & cursor setup
    system("title Snake Game - C++");
    system("mode con: cols=60 lines=25");
    hideCursor();

    char choice = 'r';

    while (choice == 'r' || choice == 'R') {
        system("cls");
        setup();

        // ── Welcome Screen ──
        cout << "\n\n";
        cout << "   ================================\n";
        cout << "        SNAKE GAME IN C++\n";
        cout << "   ================================\n\n";
        cout << "   Controls:\n";
        cout << "     Arrow Up    = Move Up\n";
        cout << "     Arrow Down  = Move Down\n";
        cout << "     Arrow Left  = Move Left\n";
        cout << "     Arrow Right = Move Right\n";
        cout << "     X or ESC    = Quit\n\n";
        cout << "   Rules:\n";
        cout << "     Eat @ to grow and earn points\n";
        cout << "     Avoid walls and yourself!\n\n";
        cout << "   Press any key to START...\n";
        cout << "   ================================\n";
        _getch();
        system("cls");

        // ── Game Loop ──
        while (!gameOver) {
            draw();
            input();
            logic();

            // Speed increases with score
            int speed = 150 - (score / 10) * 5;
            if (speed < 60) speed = 60;  // Minimum delay
            Sleep(speed);
        }

        // ── Game Over ──
        showGameOver();

        choice = _getch();
    }

    gotoxy(0, 12);
    cout << "\n   Thanks for playing! Goodbye!\n\n";
    return 0;
}