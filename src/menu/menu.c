/**
 * @file menu.c
 * @brief Implementation for the menu functions.
 *
 * Source code for the menu functions. This contains all the functions
 * needed to manage the menu, except command processes.
 *
 * Design Philosophy:
 *
 * This file contains the functions needed to manage the menu. This includes
 * print functions and tunnels to the command processors.
 *
 * @author daoge_cmd
 * @date June 24, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "menu/menu.h"
#include "user/user.h"

static const char *WELCOME =
    "    __  ____           _____                                  \n"
    "   /  |/  (_)___  ___ / ___/      _____  ___  ____  ___  _____\n"
    "  / /|_/ / / __ \\/ _ \\\\__ \\ | /| / / _ \\/ _ \\/ __ \\/ _ \\/ ___/\n"
    " / /  / / / / / /  __/__/ / |/ |/ /  __/  __/ /_/ /  __/ /    \n"
    "/_/  /_/_/_/ /_/\\___/____/|__/|__/\\___/\\___/ .___/\\___/_/     \n"
    "                              coded in C  /_/ v. 0.2.0        \n";

/**
 * In current implementation with simple yes/no menu, this
 * function only takes yes or no AND process the game to be run.
 * Returns true if it runs, false otherwise;
 *
 * @return true if yes; false otherwise 
 */
bool menu__take_input() {
  struct Game game;
  char input[4];

  user__print_current_user();
  printf("\nStart New Game? Y/N: ");
  
  scanf("%3s", input);
    
  if (strncmp(input, "N", 1) == 0 || strncmp(input, "n", 1) == 0 ||
      strncmp(input, "No", 2) == 0 || strncmp(input, "no", 2) == 0) {
    printf("Saving user data and exiting...\n");
    user__save();
    return false;
  } else {
    menu__build_board(&game);
    return true;
  }
}

/**
 * This function takes in a game and asks the user for height, width and density
 * paramaters. Then it builds the game.
 *
 * @param game the game to be built
 * 
 * @return true if runs; false otherwise
 */
bool menu__build_board(struct Game *game) {
  char input[5];

  int height = -1;
  int width = -1;
  int num_mines = -1;
  
  bool valid = false;

  while (!valid) {
    printf("\nEnter Height (Max 24): ");
    scanf("%4s", input);
    height = atoi(input);
    if (height < 25 && height > 1) {
      valid = true;
    } else {
      printf("\nInvalid Height, try again\n");
    }
  }

  valid = false;

  while (!valid) {
    printf("\nEnter Width (Max 32): ");
    scanf("%4s", input);
    width = atoi(input);
    if (width < 33 && width > 1) {
      valid = true;
    } else {
      printf("\nInvalid Width, try again\n");
    }
  }

  valid = false;

  while (!valid) {
    printf("\nEnter Mines (Up to 26 percent of board area, max 200): ");
    scanf("%4s", input);
    num_mines = atoi(input);
    if (num_mines < ceil((height * width) * .261) && num_mines > 0) {
      valid = true;
    } else {
      printf("\nInvalid Number of Mines, try again\n");
    }
  }
  
  game__build_game(width, height, num_mines, game);
  
  // Clear the input buffer to avoid issues with leftover characters ("\n")
  int c;
  while((c = getchar()) != '\n' && c != EOF);

  game__play(game);

  return true;
}

/**
 * This function prints the welcome screen for the game.
 */
void menu__print_welcome() {
  printf("%s", WELCOME);
}
