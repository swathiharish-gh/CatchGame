#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


// Global variables - Game Variables
int x, y;  // Screen size
int basketWidth = 100;  // Basket width
int objectSize = 30;  // Size of the falling object
int objectSpeed;  // Speed of the falling object
int score = 0;  // Player's score
int objectPosX, objectPosY;  // Position of the falling object
int basketPos;  // Position of the basket
char scoreStr[50];  // String to display score
char livesStr[50];  // String to display lives
int gameOver = 0;  // Game over flag
int isPaused = 0;  // Pause flag
int currentObjectType;  // 0: CATCH, 1: BOMB
int maxSpeed = 15;  // Maximum speed limit
int speedIncrement = 1;  // Speed increase increment
int lives = 3;  // Number of lives



// Function Declarations
void setGraphics();
void loadingScreen();
void displayMenu();
void startGame();
    void drawGame();
    void spawnObject();
    int checkCollision(); 
    void updateSpeed();
    void pauseGame();
    void handlePauseInput(char key); 
    void gameOverScreen();

void displayControls();
void showCredits();
void showRules();







// Main function
int main() {
    setGraphics();  // Initialize graphics
    loadingScreen(); // show loading
    displayMenu();  // Show the menu
    closegraph();  // Close graphics on exit
    return 0;
}

// Function to initialize graphics
void setGraphics() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "C:\\Turboc3\\BGI");  // Initialize graphics mode
    x = getmaxx(); // Get maximum width
    y = getmaxy(); // Get maximum height
}

// Loading screen
void loadingScreen() {
    int i = 0;
    cleardevice();
    settextstyle(4, 0, 5);
    outtextxy(x / 2 - 250, y / 3, "Welcome To 2D Catch Game");
    delay(1000);

    settextstyle(3, 0, 1);
    outtextxy(x / 2 - 100, y - 120, "LOADING....");

    while (i != 200) {
        rectangle(x / 2 - 100, y - 85, x / 2 - 100 + i, y - 75);
        rectangle(x / 2 - 100, y - 85, x / 2 + 100, y - 75);
        i++;
        delay(10);
    }
}


//Main Menu
void displayMenu() {
    int selection = 0;
    int key;
    int i;
    const int totalOptions = 5; // Total menu options
    cleardevice(); 
    settextstyle(3, 0, 3);
    
   
    while (1) {
        cleardevice();
        score = 0; // Initial score

        // Display menu options with numbering
        for (i = 0; i < totalOptions; i++) {
            char option[50];
            sprintf(option, "%d. %s", i + 1, i == 0 ? "Play" :
                                          i == 1 ? "Controls" :
                                          i == 2 ? "Rules" :
                                          i == 3 ? "Credits" :
                                          "Exit");
            outtextxy(x / 2 - 100, 100 + 50 * i, option);
        }

        // Draw selection indicator
        for (i = 0; i < totalOptions; i++) {
            circle(x / 2 - 110, 125 + 50 * i, 5);
        }
        setfillstyle(SOLID_FILL, WHITE);
        floodfill(x / 2 - 110, 125 + 50 * selection, WHITE);

        // Handle user input
        key = getch();
        if (key == 0 || key == 224) { // Check for special keys (including arrow keys)
            key = getch(); // Get the actual key code
            switch (key) {
                case 72: // Up arrow
                case 'w': // W key
                    selection = (selection == 0) ? totalOptions - 1 : selection - 1;
                    break;
                case 80: // Down arrow
                case 's': // S key
                    selection = (selection == totalOptions - 1) ? 0 : selection + 1;
                    break;
            }
        } else if (key == 13) { // Enter key
            switch (selection) {
                case 0:
                    startGame();
                    break;
                case 1:
                    displayControls();
                    break;
                case 2:
                    showRules();
                    break;
                case 3:
                    showCredits();
                    break;
                case 4:
                    closegraph(); // Clean up graphics resources
                    exit(0);
                    break;
            }
        }

        delay(50);// to reduce flickering
    }
}

//----------------------------------------------------
// Function to start the game
void startGame() {
    x = getmaxx();  // Get screen width
    y = getmaxy();  // Get screen height
    basketPos = x / 2 - basketWidth / 2;  // Initial basket position

    spawnObject();  // Spawn the first object

    while (!gameOver) {
        if (!isPaused) {
            drawGame();  // Draw game screen
            objectPosY += objectSpeed;  // Move object down

            // Check if object reaches the bottom
            if (objectPosY >= y - 30) {
                // If a catchable item is missed, decrease lives
                if (currentObjectType == 0) {
                    lives--;  // Lose a life
                    if (lives <= 0) {
                        gameOver = 1;  // Game over if no lives are left
                    }
                }
                spawnObject();  // Spawn a new object regardless
            } else {
                if (checkCollision()) {  // Check for collision with the basket
                    if (currentObjectType == 1) {
                        lives--;  // Lose a life if a bomb is caught
                        if (lives <= 0) {
                            gameOver = 1;  // Game over if no lives are left
                        }
                    } else {
                        score++;  // Increase score on successful catch
                        updateSpeed();  // Update object speed based on score
                    }
                    spawnObject();  // Spawn a new object
                }
            }
        } else {
            // If the game is paused, handle pause input
            if (kbhit()) {
                char key = getch();  // Capture input
                handlePauseInput(key);  // Handle input while paused
             
            }
            // Optionally, redraw the paused screen if needed
        }

        // Basket movement with keyboard
        if (!isPaused && kbhit()) {
            int key = getch();
            if (key == 0 || key == 224) {  // Special key pressed (like arrows)
                key = getch();  // Get the arrow key code
                switch (key) {
                    case 75:  // Left arrow key
                        basketPos = (basketPos <= 0) ? 0 : basketPos - 20;
                        break;
                    case 77:  // Right arrow key
                        basketPos = (basketPos + basketWidth >= x) ? x - basketWidth : basketPos + 20;
                        break;
                }
            } else {
                // WASD keys
                switch (key) {
                    case 'a':  // Move left
                        basketPos = (basketPos <= 0) ? 0 : basketPos - 20;
                        break;
                    case 'd':  // Move right
                        basketPos = (basketPos + basketWidth >= x) ? x - basketWidth : basketPos + 20;
                        break;
                    case 'p':  // Pause or resume game
                        pauseGame();
                        break;
                }
            }
        }

        delay(50);  // Control game speed
    }

    gameOverScreen();  // Display game over screen
}



// Function to draw game elements
void drawGame() {
    cleardevice();
    
    // Use a black background for better visibility
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, getmaxx(), getmaxy());
    // Set text color and style for score and lives
    setcolor(YELLOW);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);  // Smaller font for better fit
    // Display the score
    sprintf(scoreStr, "Score: %d", score);
    outtextxy(10, 10, scoreStr);
    // Display the lives with more spacing
    sprintf(livesStr, "Lives: %d", lives);
    outtextxy(10, 30, livesStr);  // Below the score
    // Display controls with good spacing
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(10, 50, "Controls: A/D or Left/Right Arrow to Move");
    outtextxy(10, 70, "P to Pause");
    // Draw the falling object
    setcolor(currentObjectType == 1 ? RED : GREEN);
    rectangle(objectPosX, objectPosY, objectPosX + objectSize, objectPosY + objectSize);
    setcolor(WHITE);
    outtextxy(objectPosX + 5, objectPosY + 5, currentObjectType == 1 ? "BOMB" : "CATCH");
    // Draw the basket
    setcolor(YELLOW);
    rectangle(basketPos, y - 30, basketPos + basketWidth, y - 10);
}


// Function to spawn a new falling object at a random position
void spawnObject() {
    static int lastObjectType = -1; // -1 indicates no last object type
    // Generate a random X position within the screen width, ensuring the object fits
    objectPosX = rand() % (x - objectSize);  
    objectPosY = 0;  // Reset Y position to the top of the screen
    
    // Randomly decide the type of object, ensuring 20% chance of being a bomb (1)
    currentObjectType = (rand() % 5 == 0) ? 1 : 0;  // 20% chance of bomb, 80% for catch

    // Optional: Keep track of the last object type and ensure no immediate repetitions
    
    while (currentObjectType == lastObjectType) {
        currentObjectType = (rand() % 5 == 0) ? 1 : 0;  // Re-randomize if same as last
    }
    lastObjectType = currentObjectType; // Update last object type
    updateSpeed();  // Set speed for the new object
}


// Function to check if the falling object is caught by the basket
int checkCollision() {
    return (objectPosY + objectSize >= y - 30 && objectPosY + objectSize <= y - 10 &&
            objectPosX + objectSize > basketPos && objectPosX < basketPos + basketWidth);
}

// Function to update object speed based on score
void updateSpeed() {
    objectSpeed = 8 + (score * speedIncrement);  // Start from speed of 8
    if (objectSpeed > maxSpeed) {
        objectSpeed = maxSpeed;  // Cap at maximum speed
    }
}


// Function to pause or resume the game
void pauseGame() {
    isPaused = !isPaused;  // Toggle pause state
    if (isPaused) {
        // Clear the screen and display pause message
        setfillstyle(SOLID_FILL, BLACK);
        bar(0, 0, getmaxx(), getmaxy());  // Clear screen
        setcolor(YELLOW);
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);  // Larger font for visibility
        
        // Display pause message with options
        outtextxy(getmaxx() / 2 - 150, getmaxy() / 2 - 20, "Game Paused");
        outtextxy(getmaxx() / 2 - 150, getmaxy() / 2 + 10, "Press 'P' to Resume");
        outtextxy(getmaxx() / 2 - 150, getmaxy() / 2 + 30, "Anything else to exit");
    } else {
        cleardevice();  // Clear the pause message
        // Resume game logic goes here (if needed)
    }
}

// Function to handle key press for menu or resume
void handlePauseInput(char key) {
    if (isPaused) {
        if (key == 'p' || key == 'P') {
            isPaused = 0;  // Resume the game
        } else {
            // Logic to go to the menu
            exit(0);
        }
    }
}

// Function to display game over screen
void gameOverScreen() {
    cleardevice();
    settextstyle(4, 0, 4);
    outtextxy(x / 2 - 150, y / 3, "GAME OVER");

    sprintf(scoreStr, "Final Score: %d", score);
    outtextxy(x / 2 - 150, y / 3 + 50, scoreStr);

    delay(2000);  // Pause to show the score
    outtextxy(x / 2 - 150, y / 3 + 100, "Press any key to exit...");
    getch();  // Wait for the player to press a key before exiting
    exit(0);
}

//--------------------------------------------------------



void displayControls() {
    cleardevice();
    outtextxy(x / 2 - 200, 80, "Controls:");
    outtextxy(x / 2 - 150, 130, "Use arrow keys to navigate.");
    outtextxy(x / 2 - 150, 180, "Press Enter to select.");
    outtextxy(x / 2 - 150, 230, "Navigation(without arrow keys):");
    outtextxy(x / 2 - 100, 280, "w - up, s - down.");
    outtextxy(x / 2 - 100, 330, "d - right, a - left.");
    outtextxy(x / 2 - 150, 380, "Press any key to return.");
    getch();
}

void showCredits() {
    cleardevice();
    outtextxy(x / 2 - 100, 100, "Credits:");
    outtextxy(x / 2 - 100, 150, "Developed by meee!!!");

    // ASCII Art representation of a heart
   
    outtextxy(x / 2 - 75, 200, "  * * * *  ");
    outtextxy(x / 2 - 75, 210, " *   *   * ");
    outtextxy(x / 2 - 75, 220, " *       * ");
    outtextxy(x / 2 - 75, 230, "  *     *  ");
    outtextxy(x / 2 - 75, 240, "   *   *   ");
    outtextxy(x / 2 - 75, 250, "    * *    ");
    outtextxy(x / 2 - 75, 260, "     *     ");

    outtextxy(x / 2 - 100, 300, "Press any key to return.");
    getch();
}


void showRules() {
    int offsetX = 30;  // Adjust this value to position the text

    cleardevice();  // Clear the screen
    settextstyle(3, 0, 1);  // Set text style

   
    // Display rules
    outtextxy(offsetX, 80, "Rules:");
    outtextxy(offsetX, 130, "1. Catch the green box (catch) and leave the red box (bomb).");
    outtextxy(offsetX, 180, "2. Score increases on catching the green box.");
    outtextxy(offsetX, 230, "3. Missing the catch or catching the bomb decreases your lives.");
    outtextxy(offsetX, 280, "4. You have a total of 3 lives to play.");
    outtextxy(offsetX, 330, "Press any key to return.");
    outtextxy(offsetX, 380, "Enjoy the game!!!");

    getch();  // Wait for user input
}




