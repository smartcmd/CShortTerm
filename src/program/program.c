/**
 * @file program.c
 * @brief Implementation for the program functions.
 *
 * This is the main program file, that contains all the code to run the program
 * on a base level. From start to stop and the loops inbetween.
 *
 * Design Philosophy:
 *
 * The code in this function will be called from main or other places to assist
 * with running the program.
 *
 * @author daoge_cmd
 * @date June 24, 2025
 */

#include "program/program.h"
#include "menu/menu.h"
#include "user/user.h"

/* Global Variables */
bool running = true;

/**
 * This function runs the program loop to allow for
 * multiple games to be played in one run.
 */
void program__run() {
  menu__print_welcome();
  user__init();

  bool gameRan;
  
  while(running) {
    gameRan = menu__take_input();
    if (gameRan == false && running == true) {
      running = false;
    } else if (gameRan == true && running == false) {
      running = false;
    } else {
      running = true;
    }
  }
}
