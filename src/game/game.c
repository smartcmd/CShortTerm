/**
 * @file game.c
 * @brief Implementation for the game functions.
 *
 * Source code for the game. This contains all the functions
 * needed to run the game, exceptt command processes.
 *
 * Design Philosophy:
 *
 * This file contains the functions needed to run the game. This includes
 * the game loop, print statements, and tunnels to the command processors.
 *
 * @author daoge_cmd
 * @date June 24, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "board/board.h"
#include "game/game.h"
#include "game/command_processor.h"
#include "user/user.h"

static const char *WIN =
    "  ___    ___ ________  ___  ___          ___       __   ___  ________      \n"
    " |\\  \\  /  /|\\   __  \\|\\  \\|\\  \\        |\\  \\     |\\  \\|\\  \\|\\   ___  \\    \n"
    " \\ \\  \\/  / | \\  \\|\\  \\ \\  \\\\\\  \\       \\ \\  \\    \\ \\  \\ \\  \\ \\  \\\\ \\  \\   \n"
    "  \\ \\    / / \\ \\  \\\\\\  \\ \\  \\\\\\  \\       \\ \\  \\  __\\ \\  \\ \\  \\ \\  \\\\ \\  \\  \n"
    "   \\/  /  /   \\ \\  \\\\\\  \\ \\  \\\\\\  \\       \\ \\  \\|__\\_\\  \\ \\  \\ \\  \\\\ \\  \\ \n"
    " __/  / /      \\ \\_______\\ \\_______\\       \\ \\____________\\ \\__\\ \\__\\\\ \\__\\\n"
    "|\\___/ /        \\|_______|\\|_______|        \\|____________|\\|__|\\|__| \\|__|\n"
    "\\|___|/                                                                    \n"
    "\n"
    "Score: %f\n";

static const char *LOSS =
    "__  __               __                  \n"
    "\\ \\/ /___  __  __   / /   ____  ________ \n"
    " \\  / __ \\/ / / /  / /   / __ \\/ ___/ _ \\\n"
    " / / /_/ / /_/ /  / /___/ /_/ (__  )  __/\n"
    "/_/\\____/\\__,_/  /_____/\\____/____/\\___/ \n"
    "                                         \n";

/**
 * This function prints the win screen for the game.
 */
void game__print_win(struct Game *game) {
  // Calculate score based on time taken
  double score = difftime(game__get_end_time(game), game__get_start_time(game));
  printf(WIN, score);

  // Add score to user's total score
  struct User *current_user = user__get_current_user();
  current_user->total_scores += (int) score;
}

/**
 * This function prints the loss screen for the game.
 */
void game__print_loss() {
  printf("%s", LOSS);
}

/**
 * This function builds a specified game. This function builds
 * the board to the specified size and difficulty. The won and noFog
 * params are set to false by default.
 * 
 * @param game the game to have it's board built
 */
void game__build_game(int x, int y, int mines, struct Game *game) {
  game__set_end(false, game);
  board__build_array(x, y, &game->board);
  
  /* Place mines randomly */
  srand(time(0)); // use time for better randomness
  
  for (int i = 0; i < mines; i++) { 
    bool placed = false;
    while (!placed) {
      int x = rand() % board__get_x(&game->board);
      int y = rand() % board__get_y(&game->board);
      placed = board__place_mine(x, y, &game->board);
    }
  }

  game__set_flags(mines, game);
  board__set_no_fog(false, &game->board);
  
  game->turns = 0;
}

/**
 * This function prompts the user for game input. This function
 * then passes necessary information to the command processor to
 * be handled. Prints out invalid command information if command
 * is invalid.
 *
 * @param game the game to process input on
 */
void game__take_game_input(struct Game *game) {
  //---VARIABLES---//
  char rawInput[20];

  char * input;
  char * xChar;
  char * yChar;
  
  int x;
  int y;

  // Take in command
  printf("Enter a command: ");
  
  fgets(rawInput, 20, stdin);

  // Parse command
  input = strtok(rawInput, " ");
  xChar = strtok(NULL, " ");
  yChar = strtok(NULL, " ");

  // Checks if input requiring coordinates is in form [input] [x] [y]
  if ((xChar == NULL || yChar == NULL) &&
      (strncmp(input, "r", 1) == 0 || strncmp(input, "f", 1) == 0 || strncmp(input, "g", 1) == 0)) {
    // Makes input invalid
    input = "x";
  } else if (strncmp(input, "r", 1) == 0 || strncmp(input, "f", 1) == 0 || strncmp(input, "g", 1) == 0) {
    // Sets x and y
    x = atoi(xChar);
    y = atoi(yChar);
  }

  // Processes Input by calling correct functions
  if (strncmp(input, "r", 1) == 0 || strncmp(input, "reveal", 6) == 0) {
    command_processor__reveal(x, y, game);
  } else if (strncmp(input, "f", 1) == 0 || strncmp(input, "flag", 4) == 0) {
    command_processor__flag(x, y, game);
  } else if (strncmp(input, "h", 1) == 0 || strncmp(input, "help", 4) == 0) {
    command_processor__help();
    game->turns--;
  } else if (strncmp(input, "g", 1) == 0 || strncmp(input, "guess", 5) == 0) {
    command_processor__guess(x, y, game);
  } else if (strncmp(input, "noFog", 5) == 0) {
    command_processor__no_fog(game);
  } else if (strncmp(input, "q", 1) == 0 || strncmp(input, "quit", 4) == 0) {
    command_processor__quit(game);
  } else if (strncmp(input, "c", 1) == 0 || strncmp(input, "close", 5) == 0) {
    command_processor__close(game);
  } else {
    printf("\nInvalid Command: %s\n", input); // TODO: Change when game design is finalized
    game->turns--;
  }

  game->turns++;
  
}

/**
 * This function runs the play loop for the passed in game.
 *
 * @param game the game to be played
 */
void game__play(struct Game *game) {
  game__set_start_time(game);
  while (!game__get_end(game)) {
    printf("\n");

    printf("Turn: %d\n", game->turns);
    if (board__get_no_fog(&game->board) == true) {
      board__print_no_fog(&game->board);
      board__set_no_fog(false, &game->board);
    } else {
      board__print_board(&game->board);
    }

    printf("\n");

    game__take_game_input(game);
    
    if (board__get_num_mines(&game->board) == 0 &&
        board__get_num_flags(&game->board) == game__get_flags(game)) {
      game__set_end_time(game);
      game__print_win(game);
      game__set_end(true, game);
    }
  }
}

//---------------------//
//   GETTERS/SETTERS   //
//---------------------//

/**
 * This function takes in a bool and a game and sets
 * end status in game.
 *
 * @param end the end status
 * @param game the game to be set
 */
void game__set_end(bool end, struct Game *game) {
  game->end = end;
}

/**
 * This function takes in a game and returns it's end
 * variable.
 *
 * @param game the game to get the end status of
 *
 * @return the end status
 */
bool game__get_end(struct Game *game) {
  return game->end;
}

/**
 * This function takes an int and a game and sets
 * the number of flags needed in the game.
 *
 * @param flags the number of flags
 * @param game the game to be set
 */
void game__set_flags(int flags, struct Game *game) {
  game->flags_needed = flags;
}

/**
 * This function takes a game and returns the game's
 * flags needed.
 *
 * @param game the game to get the flags of
 *
 * @return the number of flags needed
 */
int game__get_flags(struct Game *game) {
  return game->flags_needed;
}

/**
 * This function takes a game and sets the game's
 * start time based on the system time.
 *
 * @param game the game to set the start time of
 */
void game__set_start_time(struct Game *game) {
  game->start_time = time(NULL);
}

/**
 * This function takes a game and returns the game's
 * start time.
 *
 * @param game the game to get the start time of
 *
 * @return the start time
 */
time_t game__get_start_time(struct Game *game) {
  return game->start_time;
}

/**
 * This function takes a game and sets the game's
 * end time based on the system time.
 *
 * @param game the game to set the end time of
 */
void game__set_end_time(struct Game *game) {
  game->end_time = time(NULL);
}

/**
 * This function takes a game and returns the game's
 * end time.
 *
 * @param game the game to get the end time of
 *
 * @return the end time
 */
time_t game__get_end_time(struct Game *game) {
  return game->end_time;
}
