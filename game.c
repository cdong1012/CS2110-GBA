#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include "gba.h"
#include "images/starting_screen.h"
#include "images/ending_screen.h"
#include "images/trophy.h"

ENEMY enemies[ENEMY_NUM];
int start_screen_on = 0; // if start screen is already drawn, this is set to 1
int end_screen_on = 0; // if end screen is already drawn, this is set to 1
int death_count = 0;
struct player PLAYER;
int RANDOM_SEED = 110400; // random seed to generate enemies

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial game state
  GBAState state = START;

  while (1) {
    currentButtons = BUTTONS;  // Load the current state of the buttons

    switch (state) {
      case START:
        if (!start_screen_on) { // If startscreen is not on, draw it. If not, don't. Save time!
          waitForVBlank();
          drawFullScreenImageDMA(starting_screen);
          start_screen_on = 1;
        } else {
          if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) { // if the Start button is pressed, go to PLAY
            state = PLAY;
            waitForVBlank();
            drawRectDMA(0, 0, 240, 160, BLACK);
            start_screen_on = 0;
          }
        }
        break;
      case PLAY:
        init_enemies();
        init_player();
        drawRectDMA(0, 220, 20, 160, GREEN); // Draw the goal
        drawChar(30, 227, 'G', MAGENTA);
        drawChar(60, 227, 'O', MAGENTA);
        drawChar(90, 227, 'A', MAGENTA);
        drawChar(120, 227, 'L', MAGENTA);
        char death_string[10];
        sprintf(death_string, "death: %d", death_count); // death count to keep the player updated
        while (1) {
          currentButtons = BUTTONS;
          draw_enemies();
          draw_player(currentButtons);
          drawString(140, 0, death_string, CYAN);
          if (collision_system() == -1) { // if colides with an enemy, go to LOSE.
            death_count++;
            state = LOSE;
            RANDOM_SEED += 1104;      // random the next wave of enemies
            break;
          } else if (collision_system() == 1) { // colides with the goal! Win and go to TROPHY
            state = TROPHY;
            drawRectDMA(0, 0, 240, 160, BLACK);
            break;
          }
          // restart with A
          if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons)) {
            state = START;
            waitForVBlank();
            drawRectDMA(0, 0, 240, 160, BLACK);
            death_count = 0;
            break;
          }
        }
        break;
      case TROPHY:
        init_player();
        drawImageDMA(90, 100, TROPHY_WIDTH, TROPHY_HEIGHT, trophy); // Draw trophy
        while (1) {
          currentButtons = BUTTONS;
          waitForVBlank();
          draw_player(currentButtons);
          drawString(50, 80, "Congratulation!", CYAN);
          drawString(70, 50, "You deserve this trophy!", CYAN);
          if (get_trophy_yet()) { // if colides with trophy, go to WIN.
            state = WIN;
            drawRectDMA(0, 0, 240, 160, BLACK);
            break;
          }
          // restart with A
          if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons)) {
            state = START;
            waitForVBlank();
            drawRectDMA(0, 0, 240, 160, BLACK);
            death_count = 0;
            break;
          }
        }
        break;
      case WIN:
        if (!end_screen_on) { // if end screen is not on, draw it. If not, don't draw!
          waitForVBlank();
          drawFullScreenImageDMA(ending_screen);
          death_count = 0;
          end_screen_on = 1;
        } else {
          if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons)) { // restart back to start
            state = START;
            waitForVBlank();
            drawRectDMA(0, 0, 240, 160, BLACK);
            death_count = 0;
            end_screen_on = 0;
          }
        }
        break;
      case LOSE:
        waitForVBlank();
        drawRectDMA(0, 0, 240, 160, BLACK); // draw losing message
        drawCenteredString(30, 110, 20, 20, "YOU LOST!", CYAN);
        char death_note[20];
        sprintf(death_note, "You have died %d %s", death_count, death_count == 1 ? "time" : "times");
        drawCenteredString(50, 110, 20, 20, death_note, CYAN);
        drawCenteredString(70, 110,20, 20, "Please hit B to go", CYAN);
        drawCenteredString(90, 110, 20, 20, "back to the starting screen", CYAN);
        while(1) {
          currentButtons = BUTTONS;
          if (KEY_JUST_PRESSED(BUTTON_B, currentButtons, previousButtons)) { // check if B is pressed. If it is, go back to PLAY!
            state = PLAY;
            waitForVBlank();
            drawRectDMA(0, 0, 240, 160, BLACK);
            break;
          } else if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons)) {
            state = START;
            waitForVBlank();
            drawRectDMA(0, 0, 240, 160, BLACK);
            death_count = 0;
            break;
          }
        }
        break;
    }
    previousButtons = currentButtons;  // Store the current state of the buttons
  }

  return 0;
}


// check if PLAYER collides with TROPHY, return 1 for true, 0 for false
int get_trophy_yet(void) {
  if (PLAYER.row >= TROPHY_Y && PLAYER.row < TROPHY_Y + TROPHY_HEIGHT && PLAYER.col >= TROPHY_X && PLAYER.col < TROPHY_X + TROPHY_WIDTH) {
    return 1;
  }
  return 0;
}

// check if PLAYER collides with enemies or goal, return 1 for goal, 0 for not colide, and -1 for enemies
int collision_system(void) { // 0 means not colide, 1 means colide with the end goal, -1 means colide with enemy
  int i;
  for (i = 0; i < ENEMY_NUM; i++) {
    ENEMY *enemy = &enemies[i];
    int row_diff = enemy->row > PLAYER.row ? enemy->row - PLAYER.row : PLAYER.row - enemy->row;
    int col_diff = enemy->col > PLAYER.col ? enemy->col - PLAYER.col : PLAYER.col - enemy->col;
    if (row_diff < enemy->edge && col_diff < enemy->edge) {
      return -1;
    }
  }
  if (PLAYER.col + PLAYER.edge > 221) {
    return 1;
  }
  return 0;
}


// initalize enemies, randomly populate each one.
void init_enemies(void) {
  srand(RANDOM_SEED); 
  int i;
  for (i = 0; i < ENEMY_NUM; i++) {
    ENEMY *enemy = &enemies[i];
    enemy->col = (i + 2) * 20;
    enemy->direction = (rand() % 2) == 0 ? 1 : -1; 
    enemy->edge = 10;
    int random = rand() % 160;
    enemy->prev_row = random - random % 10;
    enemy->row = enemy->prev_row;
  }

  waitForVBlank();
  for (int i = 0; i < ENEMY_NUM; i++) {
    ENEMY *enemy = &enemies[i];
    drawRectDMA(enemy->row, enemy->col, enemy->edge, enemy->edge, BLUE);
  }
  return;
}


// initialize PLAYER
void init_player(void) {
  PLAYER.col = 0;
  PLAYER.row = 0;
  PLAYER.prev_col = 0;
  PLAYER.prev_row = 0;
  PLAYER.edge = 10;
  PLAYER.speed = 2;
  waitForVBlank();
  drawRectDMA(PLAYER.prev_col, PLAYER.prev_row, PLAYER.edge, PLAYER.edge, RED);
}


// Draw the player out dependinging on the buttons
void draw_player(u32 currentButtons) {
  if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
    PLAYER.col += PLAYER.speed;
    if (PLAYER.col > 230) {
      PLAYER.col = 230;
    }
  } else if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
    PLAYER.col -= PLAYER.speed;
    if (PLAYER.col < 0) {
      PLAYER.col = 0;
    }
  }

  if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
    PLAYER.row += PLAYER.speed;
    if (PLAYER.row > 150) {
      PLAYER.row = 150;
    }
  } else if (KEY_DOWN(BUTTON_UP, currentButtons)) {
    PLAYER.row -= PLAYER.speed;
    if (PLAYER.row < 0) {
      PLAYER.row = 0;
    }
  }

  if (PLAYER.prev_col != PLAYER.col || PLAYER.prev_row != PLAYER.row) {
    drawRectDMA(PLAYER.prev_row, PLAYER.prev_col, 10, 10, BLACK);
    drawRectDMA(PLAYER.row, PLAYER.col, 10, 10, RED);
  }

  PLAYER.prev_row = PLAYER.row;
  PLAYER.prev_col = PLAYER.col;
}

// draw enemies to automatically move up and down
void draw_enemies(void) {
  for (int i = 0; i < ENEMY_NUM; i++) {
    ENEMY *enemy = &enemies[i];
    if (enemy->row > 155) {
      enemy->row = 155;
      enemy->direction = -enemy->direction;
    } else if (enemy->row < 0) {
      enemy->row = 0;
      enemy->direction = -enemy->direction;
    }
    enemy->row += enemy->direction * ((rand() % 3) + 1) ;
  }
  waitForVBlank();
  for (int i = 0; i < ENEMY_NUM; i++) {
    ENEMY *enemy = &enemies[i];
    drawRectDMA(enemy->prev_row, enemy->col, enemy->edge, enemy->edge, BLACK);
  }
  for (int i = 0; i < ENEMY_NUM; i++) {
    ENEMY *enemy = &enemies[i];
    drawRectDMA(enemy->row, enemy->col, enemy->edge, enemy->edge, BLUE);
    enemy->prev_row = enemy->row;
  }
}