/**
 * @file board.c
 * @brief Implementation for the game board functions.
 *
 * Source code for the board. This fundamentaly represents the
 * functions that would be members of the board struct. All
 * functions take in a board to process all actions on.
 *
 * Design Philosophy:
 *
 * A board will initially be built and all three 2D arrays will initialized to
 * false or '*'s, depending on the data type. The "game" can then generate a 
 * board and place mines on the mine_pointer array. The game will take place
 * and be displayed entirely from the board_pointer array. The mine_pointer
 * array is to keep track of the mines without modifying their position on
 * accident. The reveal_pointer array is to keep track of positions that have
 * already beenrevealed by the player to avoid accidental duplicate reveal
 * functions.
 *
 * @author daoge_cmd
 * @date June 24, 2025
 */

#include <stdio.h>
#include <stdlib.h>

#include "board/board.h"

//------------------------------//
//   CONSTRUCTOR / DESTRUCTOR   //
//------------------------------//

/**
 * This function takes a specified height and width and builds the passed
 * in board's arrays. This function returns a boolean to specify if the
 * build was successful.
 *
 * @param width the width of the board
 * @param height the height of the board
 * @param board the board struct to be built
 *
 * @return false if height OR width < 2; true otherwise
 */
bool board__build_array(int width, int height, struct Board *board) {
  if (width < 2 || height < 2) {
    return false;
  }

  // Build array for mines
  board->mine_pointer = (bool **)malloc(height * sizeof(bool*));
  for (int i = 0; i < height; i++) {
    board->mine_pointer[i] = (bool *)malloc(width * sizeof(bool));
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      board->mine_pointer[i][j] = false;
    }
  }

  // Build array for game board
  board->board_pointer = (char **)malloc(height * sizeof(char*));
  for (int i = 0; i < height; i++) {
    board->board_pointer[i] = (char *)malloc(width * sizeof(char));
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      board->board_pointer[i][j] = '*';
    }
  }

  // Build array for reveal tracking
  board->reveal_pointer = (bool **)malloc(height * sizeof(bool*));
  for (int i = 0; i < height; i++) {
    board->reveal_pointer[i] = (bool *)malloc(width * sizeof(bool));
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      board->reveal_pointer[i][j] = false;
    }
  }

  board__set_x(height, board);
  board__set_y(width, board);

  board->board_num_mines = 0;
  board->board_num_flags = 0;
  
  return true;
}

/**
 * This function takes a board struct and frees all the arrays from memory.
 *
 * @param board the board to be freed
 */
void board__destruct(struct Board *board) {
  // free mine_pointer
  for (int i = 0; i < 4; ++i) {
    free(board->mine_pointer[i]);
  }
  
  free(board->mine_pointer);

  // free board_pointer
  for (int i = 0; i < 4; ++i) {
    free(board->board_pointer[i]);
  }
  
  free(board->board_pointer);

  // free reveal_pointer
  for (int i = 0; i < 4; ++i) {
    free(board->reveal_pointer[i]);
  }
  
  free(board->reveal_pointer);
}

//-----------------------//
//   GETTERS / SETTERS   //
//-----------------------//

/**
 * This function takes an x value and a board and sets the baord's
 * max x value.
 *
 * @param x the x length
 * @param board the board to be configured
 */
void board__set_x(int x, struct Board *board) {
  board->board_x = x;
}

/**
 * This function takes an y value and a board and sets the board's
 * max y value.
 *
 * @param y the y length
 * @param board the board to be configured
 */
void board__set_y(int y, struct Board *board) {
  board->board_y = y;
}

/**
 * This function takes a bool value anda  board and sets the board's
 * no fog value.
 *
 * @param no_fog the no_fog value
 * @param board the board to be configured
 */
void board__set_no_fog(bool no_fog, struct Board *board) {
  board->board_no_fog = no_fog;
}

/**
 * This function returns the max x value of the passed in board.
 *
 * @param board the board to get the x max of
 * 
 * @return the x max
 */
int board__get_x(struct Board *board) {
  return board->board_x;
}

/**
 * This function returns the max y value of the passed in board.
 *
 * @param board the board to get the y max of
 * 
 * @return the y max
 */
int board__get_y(struct Board *board) {
  return board->board_y;
}

/**
 * This function retuns the no fog value of the board.
 *
 * @param board the board to get the no fog value of
 * 
 * @return the no fog value
 */
bool board__get_no_fog(struct Board *board) {
  return board->board_no_fog;
}

/**
 * This function returns the number of unflagged mines.
 *
 * @param board the board to return the value on
 *
 * @return the number of mines
 */
int board__get_num_mines(struct Board *board) {
  return board->board_num_mines;
}

/**
 * This function returns the number of flagged tiles.
 *
 * @param board the board to return the value on
 *
 * @return the number of flags
 */
int board__get_num_flags(struct Board *board) {
  return board->board_num_flags;
}

//-----------------------------//
//   USER COMMAND PROCESSORS   //
//-----------------------------//

/**
 * This function replaces the specified position with a flag (F).
 * This function returns true if operation can be completed, ie. 
 * the spot has yet to be revealed. Returns false otherwise.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param board the board to be modified
 *
 * @return true if flag is successful; false otherwise
 */
bool board__flag(int x, int y, struct Board *board) {
  if (board->reveal_pointer[x][y] == false) {
    board->board_pointer[x][y] = 'F';
    board->board_num_flags++;
    return true;
  } else {
    return false;
  }
}

/**
 * This function reveals the specified position. This function
 * will only change the specified square to reveal either a loss
 * or a number of mines adjacent. The function game__reveal will
 * handle recursive reveal operations. Returns true if reveal is
 * successful, ie. the spot has yet to be revealed. Returns false
 * otherwise.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param board the board to be modified
 * 
 * @return true if reveal is succesful; false otherwise
 */
bool board__reveal(int x, int y, struct Board *board) {
  if (board->reveal_pointer[x][y] == false) {
    if (x >= board__get_x(board)) {
      return false;
    } else if (y >= board__get_y(board)) {
      return false;
    }

    if (board->board_pointer[x][y] == 'F') {
      board->board_num_flags--;
    }
    board->board_pointer[x][y] = board__count_num_adjacent(x, y, board) + '0';
    board->reveal_pointer[x][y] = true;

    if (board__count_num_adjacent(x, y, board) == 0) {
      bool x_left = x - 1 >= 0;
      bool x_right = x + 1 < board__get_x(board);
      bool y_top = y + 1 < board__get_y(board);
      bool y_bottom = y - 1 >= 0;
      // reveal right column
      if (x_right) {
        if (y_top) {
          board__reveal(x + 1, y + 1, board);
        }
        if (y_bottom) {
          board__reveal(x + 1, y - 1, board);
        }
        board__reveal(x + 1, y, board);
      }
      // reveal left column
      if (x_left) {
        if (y_top) {
          board__reveal(x - 1, y + 1, board);
        }
        if (y_bottom) {
          board__reveal(x - 1, y - 1, board);
        }
        board__reveal(x - 1, y, board);
      }
      // reveal tile above
      if (y_top) {
        board__reveal(x, y + 1, board);
      }
      // reveal tile below
      if (y_bottom) {
        board__reveal(x, y - 1, board);
      }
    }
    
    return true;
  } else {
    return false;
  }
}

/**
 * This function places a guess marker on the specified position.
 * This function will only do this if the spot has yet to be revealed.
 * Returns true if successful, false otherwise.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param board the board to be modified
 *
 * @return true if successful; false otherwise
 */
bool board__guess(int x, int y, struct Board *board) {
  /* Check if the spot has already been revealed */
  if (board->reveal_pointer[x][y] == true) {
    // TODO: Change when game design is finalized
    printf("\nTile already revealed\n");
    return false;
  } else {
    board->board_pointer[x][y] = '?';
    return true;
  }
}

//-----------------//
//   MINE PLACER   //
//-----------------//

/**
 * This function takes an x and y coordinate and places
 * the mine on the mine_pointer array to keep track of the mines.
 * Returns true if mine was placed, returns false otherwise.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @board the board for the mine to be placed on
 *
 * @return true if mine is placed; false otherwise
 */
bool board__place_mine(int x, int y, struct Board *board) {
  if (board->mine_pointer[x][y] == false) {
    board->mine_pointer[x][y] = true;
    board->board_num_mines++;
    return true;
  } else {
    return false;
  }
}

//--------------------//
//   REVEAL UTILITY   //
//--------------------//

/**
 * This function takes in an x and y and counts the number of mines
 * in the adjacent tiles. Returns this value.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param the board to check
 *
 * @return the number of mines adjacent to this square
 */
int board__count_num_adjacent(int x, int y, struct Board *board) {
  int count = 0;

  bool x_left = (x - 1) >= 0;
  bool x_right = (x + 1) < board__get_x(board);
  bool y_top = (y + 1) < board__get_y(board);
  bool y_bottom = (y - 1) >= 0;
  
  // Left Column
  if (x_left) {
    if (y_top) { // left top
      if (board->mine_pointer[x - 1][y + 1] == true) {
        count++;
      }
    }
    if (y_bottom) { // left bottom
      if (board->mine_pointer[x - 1][y - 1] == true) {
        count++;
      }
    }
    // left center
    if (board->mine_pointer[x - 1][y] == true) {
      count++;
    }
  }

  // Right Column
  if (x_right) {
    if (y_top) { // right top
      if (board->mine_pointer[x + 1][y + 1] == true) {
        count++;
      }
    }
    if (y_bottom) { // right bottom
      if (board->mine_pointer[x + 1][y - 1] == true) {
        count++;
      }
    }
    // right center
    if (board->mine_pointer[x + 1][y] == true) {
      count++;
    }
  }

  // Tile Above
  if (y_top) {
    if (board->mine_pointer[x][y + 1] == true) {
      count++;
    }
  }

  // Tile Below
  if (y_bottom) {
    if (board->mine_pointer[x][y - 1] == true) {
      count++;
    }
  }

  return count;
}

/**
 * This function takes an x, a y, and a board struct and returns true
 * if the specified position contains a mine. Returns false otherwise.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param board the board to be checked
 *
 * @return true if the spot contains a mine; false otherwise
 */
bool board__contains_mine(int x, int y, struct Board *board) {
  if (board->mine_pointer[x][y] == true) {
    return true;
  } else {
    return false;
  }
}

//---------------------//
//   PRINT FUNCTIONS   //
//---------------------//

/**
 * This function takes in a board and prints the board to the screen.
 *
 * @param board the board to be printed
 */
void board__print_board(struct Board *board) {
  for (int i = board__get_x(board) - 1; i >= 0; i--) {
    if (i >= 10) {
      printf("%d ", i);
    } else {
      printf(" %d ", i);
    }
    for (int j = 0; j < board__get_y(board); j++) {
      char c = board->board_pointer[i][j];
      const char *bg = "\033[100m";
      if (c == '0') {
        bg = "\033[42m"; // Green
      } else if (c == '1' || c == '2') {
        bg = "\033[43m"; // Yellow
      } else if (c == '3' || c == '4') {
        bg = "\033[48;5;208m"; // Orange
      } else if (c == '5' || c == '6') {
        bg = "\033[45m"; // Purple
      } else if (c == '7' || c == '8') {
        bg = "\033[41m"; // Red
      }
      if (board__get_y(board) > 10) {
        printf("%s[ %c ]\033[0m", bg, c);
      } else {
        printf("%s[%c]\033[0m", bg, c);
      }
    }
    printf("\n");
  }

  if (board__get_y(board) > 10) {
    printf("     "); 
  } else {
    printf("    "); 
  }
  
  for (int i = 0; i < board__get_y(board); i++) {
    if (board__get_y(board) > 10) {
      if (i < 10) {
        printf("%d    ", i);
      } else {
        printf("%d   ", i);
      }
    } else {
      printf("%d  ", i);
    }
  }
}

/**
 * This function takes in a board and prints the board to the screen
 * with mines visable to the user.
 *
 * @param board the board to print
 */
void board__print_no_fog(struct Board *board) {
  for (int i = board__get_x(board) - 1; i >= 0; i--) {
    if (i >= 10) {
      printf("%d ", i);
    } else {
      printf(" %d ", i);
    }
    for (int j = 0; j < board__get_y(board); j++) {
      if (board__get_y(board) > 10) {
        if (board->mine_pointer[i][j] == true) {
          printf("[ @ ]");
        } else {
          printf("[ %c ]", board->board_pointer[i][j]);
        }
      } else {
        if (board->mine_pointer[i][j] == true) {
          printf("[@]");
        } else {
          printf("[%c]", board->board_pointer[i][j]);
        }
      }
    }
    printf("\n");
  }

  if (board__get_y(board) > 10) {
    printf("     "); 
  } else {
    printf("    "); 
  }
  
  for (int i = 0; i < board__get_y(board); i++) {
    if (board__get_y(board) > 10) {
      if (i < 10) {
        printf("%d    ", i);
      } else {
        printf("%d   ", i);
      }
    } else {
      printf("%d  ", i);
    }
  }
}
