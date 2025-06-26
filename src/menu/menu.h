/**
 * @file menu.h
 * @brief The header for menu.
 */

#ifndef MENU_H
#define MENU_H

#include "game/game.h"

bool menu__take_input();
bool menu__build_board(struct Game* game);
void menu__print_welcome();

#endif // MENU_H
