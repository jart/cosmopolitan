// -*- mode:c; indent-tabs-mode:nil; c-basic-offset:4 -*-
// vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8

// asteroids by tsotchke
// https://github.com/tsotchke/asteroids

// clang-format off

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/select.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define MAX_ASTEROIDS 5
#define MAX_BULLETS 5

typedef struct {
    float x, y;
} Vector2;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float angle;
    float radius;
} GameObject;

GameObject spaceship;
GameObject asteroids[MAX_ASTEROIDS];
GameObject bullets[MAX_BULLETS];

int score = 0;
time_t startTime;
int isGameOver = 0;
int shouldExit = 0;
int finalTime = 0; // To store final time at game over
char display[SCREEN_HEIGHT][SCREEN_WIDTH];

// Function to clear the screen buffer
void clearDisplay() {
    memset(display, ' ', sizeof(display));
}

// Function to draw a pixel on the screen
void drawPixel(int x, int y) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        display[y][x] = '*';
    }
}

// Function to draw a line using Bresenham's algorithm
void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = (x1 < x2) ? 1 : -1;
    int dy = -abs(y2 - y1), sy = (y1 < y2) ? 1 : -1;
    int error = dx + dy, e2;

    while (1) {
        drawPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * error;
        if (e2 >= dy) { error += dy; x1 += sx; }
        if (e2 <= dx) { error += dx; y1 += sy; }
    }
}

// Function to draw a circle
void drawCircle(int centerX, int centerY, int radius) {
    int x = radius - 1, y = 0, dx = 1, dy = 1, err = dx - (radius << 1);
    while (x >= y) {
        drawPixel(centerX + x, centerY + y);
        drawPixel(centerX + y, centerY + x);
        drawPixel(centerX - y, centerY + x);
        drawPixel(centerX - x, centerY + y);
        drawPixel(centerX - x, centerY - y);
        drawPixel(centerX - y, centerY - x);
        drawPixel(centerX + y, centerY - x);
        drawPixel(centerX + x, centerY - y);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

// Initialize a game object
void initializeGameObject(GameObject *obj, float x, float y, float angle, float radius) {
    obj->position = (Vector2){x, y};
    obj->velocity = (Vector2){0, 0};
    obj->angle = angle;
    obj->radius = radius;
}

// Wrap position of the spaceship and asteroids within screen bounds
void wrapPosition(Vector2 *pos) {
    if (pos->x < 0) pos->x = SCREEN_WIDTH - 1;
    if (pos->x >= SCREEN_WIDTH) pos->x = 0;
    if (pos->y < 0) pos->y = SCREEN_HEIGHT - 1;
    if (pos->y >= SCREEN_HEIGHT) pos->y = 0;
}

// Check if two game objects are colliding
int checkCollision(GameObject *a, GameObject *b) {
    float deltaX = a->position.x - b->position.x;
    float deltaY = a->position.y - b->position.y;
    return sqrt(deltaX * deltaX + deltaY * deltaY) < (a->radius + b->radius);
}

// Initialize game state
void initGame() {
    score = 0; // Reset the score
    initializeGameObject(&spaceship, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 2);

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        initializeGameObject(&asteroids[i],
            rand() % SCREEN_WIDTH,
            rand() % SCREEN_HEIGHT,
            0,
            2 + rand() % 3);
        asteroids[i].velocity.x = ((float)rand() / RAND_MAX) * 2 - 1;
        asteroids[i].velocity.y = ((float)rand() / RAND_MAX) * 2 - 1;
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].position.x = -1; // Mark bullet as inactive
        bullets[i].position.y = -1;
    }

    startTime = time(NULL);
    isGameOver = 0;
    finalTime = 0; // Reset final time
}

// Draw the spaceship on the screen
void drawSpaceship() {
    int x = (int)spaceship.position.x;
    int y = (int)spaceship.position.y;
    int size = 3;

    float cosAngle = cos(spaceship.angle);
    float sinAngle = sin(spaceship.angle);

    int x1 = x + size * cosAngle;
    int y1 = y + size * sinAngle;
    int x2 = x + size * cos(spaceship.angle + 2.5);
    int y2 = y + size * sin(spaceship.angle + 2.5);
    int x3 = x + size * cos(spaceship.angle - 2.5);
    int y3 = y + size * sin(spaceship.angle - 2.5);

    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

// Draw all entities on the screen
void drawEntities(GameObject *entities, int count, void (*drawFunc)(GameObject *)) {
    for (int i = 0; i < count; i++) {
        drawFunc(&entities[i]);
    }
}

// Draw a bullet on the screen
void drawBullet(GameObject *bullet) { // Changed to non-const
    if (bullet->position.x >= 0) {
        drawPixel((int)bullet->position.x, (int)bullet->position.y);
    }
}

// Draw an asteroid on the screen
void drawAsteroid(GameObject *asteroid) { // Changed to non-const
    drawCircle((int)asteroid->position.x, (int)asteroid->position.y, (int)asteroid->radius);
}

// Refresh the display
void updateDisplay() {
    clearDisplay();
    if (!isGameOver) {
        drawSpaceship();
        drawEntities(asteroids, MAX_ASTEROIDS, drawAsteroid);
        drawEntities(bullets, MAX_BULLETS, drawBullet);
    }

    // Print the screen buffer
    printf("\033[H");
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            putchar(display[y][x]);
        }
        putchar('\n');
    }
    
    // Display score and elapsed time
    time_t currentTime = time(NULL);
    int elapsedTime = isGameOver ? finalTime : (currentTime - startTime);
    printf("Score: %d | Time: %02d:%02d | %s\n", score, elapsedTime / 60, elapsedTime % 60, isGameOver ? "Game Over!" : "           ");
}

// Update the position of game objects
void updateGameObject(GameObject *obj, int isBullet) {
    obj->position.x += obj->velocity.x;
    obj->position.y += obj->velocity.y;

    // If it's a bullet, check if it's out of bounds
    if (isBullet) {
        if (obj->position.x < 0 || obj->position.x >= SCREEN_WIDTH || obj->position.y < 0 || obj->position.y >= SCREEN_HEIGHT) {
            obj->position.x = -1; // Deactivate bullet
            obj->position.y = -1;
        }
    } else {
        wrapPosition(&obj->position);
    }
}

// Update the game state
void updateGame() {
    if (isGameOver) return; 

    // Update spaceship and apply friction
    updateGameObject(&spaceship, 0); // 0 indicates it's not a bullet
    spaceship.velocity.x *= 0.98;
    spaceship.velocity.y *= 0.98;

    // Move asteroids and check for collisions
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        updateGameObject(&asteroids[i], 0);
        if (checkCollision(&spaceship, &asteroids[i])) {
            isGameOver = 1;
            finalTime = time(NULL) - startTime;
            return;
        }
    }

    // Update bullet positions
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].position.x >= 0) {
            updateGameObject(&bullets[i], 1); // 1 indicates it's a bullet
        }
    }

    // Check for bullet collisions with asteroids
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].position.x >= 0) {
            for (int j = 0; j < MAX_ASTEROIDS; j++) {
                if (checkCollision(&bullets[i], &asteroids[j])) {
                    bullets[i].position.x = -1; // Deactivate bullet
                    bullets[i].position.y = -1;
                    asteroids[j].position.x = rand() % SCREEN_WIDTH;
                    asteroids[j].position.y = rand() % SCREEN_HEIGHT;
                    score += 100;
                }
            }
        }
    }
}

// Fire a bullet
void shootBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].position.x < 0) {
            bullets[i].position = spaceship.position;
            bullets[i].velocity.x = cos(spaceship.angle) * 2;
            bullets[i].velocity.y = sin(spaceship.angle) * 2;
            break;
        }
    }
}

// Check if a key was hit
int isKeyHit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

// Configure terminal settings
void configureTerminal(struct termios *old_tio, struct termios *new_tio) {
    tcgetattr(STDIN_FILENO, old_tio);
    *new_tio = *old_tio;
    new_tio->c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, new_tio);
}

// Restore terminal settings
void restoreTerminal(struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}

void onSignal(int sig) {
    shouldExit = 1;
}

// Main game loop
int main() {
    signal(SIGINT, onSignal); // Capture ^C
    srand(time(NULL)); // Seed the random number generator
    initGame(); // Initialize the game state

    struct termios old_tio, new_tio;
    configureTerminal(&old_tio, &new_tio);

    printf("\033[?25l");  // Hide the cursor

    while (!shouldExit) {
        if (isKeyHit()) {
            char input = getchar();
            if (input == 27) {  // ESC key
                if (getchar() == '[') {  // Handle arrow keys
                    switch (getchar()) {
                        case 'A':  // Up arrow
                            spaceship.velocity.x += cos(spaceship.angle) * 0.2;
                            spaceship.velocity.y += sin(spaceship.angle) * 0.2;
                            break;
                        case 'B':  // Down arrow
                            spaceship.velocity.x -= cos(spaceship.angle) * 0.2;
                            spaceship.velocity.y -= sin(spaceship.angle) * 0.2;
                            break;
                        case 'D': spaceship.angle -= 0.2; break;  // Left arrow
                        case 'C': spaceship.angle += 0.2; break;  // Right arrow
                    }
                }
            } else if (input == ' ') {
                shootBullet(); // Fire a bullet
            } else if (input == 'q') {
                break; // Quit the game
            } else if (input == 'r' && isGameOver) {
                initGame(); // Restart the game
            }
        }

        updateGame(); // Update game state
        updateDisplay(); // Refresh the display
        usleep(50000);  // Wait for 50ms (20 FPS)
    }

    printf("\033[?25h");  // Show the cursor
    restoreTerminal(&old_tio); // Restore terminal settings
    return 0;
}
