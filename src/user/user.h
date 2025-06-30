/**
 * @file user.h
 * @brief The header for user.
 */
#ifndef USER_H
#define USER_H

#include <stdbool.h>

struct User {
    char* name;
    int   total_scores;
};

void         user__init();
void         user__save();
void         user__print_user_list();
struct User* user__choose_user(char* name);
struct User* user__add_user(char* name);
void         user__print_current_user();
struct User* user__get_current_user();

#endif // USER_H