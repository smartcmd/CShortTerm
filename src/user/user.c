/**
 * @file user.c
 * @brief Implementation for the user functions.
 *
 * Source code for the user functions. This contains all the functions
 * needed to manage the user.
 *
 * Design Philosophy:
 *
 * The code in this function will be called from main or other places to assist
 * with running the program.
 *
 * @author daoge_cmd
 * @date June 30, 2025
 */

#include "user/user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_NAME_LEN 32
#define USER_FILE "users.txt"

static struct User user_list[MAX_USERS];
static int user_count = 0;
static struct User* current_user = NULL;

void user__init() {
    // Load user data
    FILE* fp = fopen(USER_FILE, "r");
    user_count = 0;
    if (!fp) return;
    char name[MAX_NAME_LEN];
    int score;
    while (fscanf(fp, "%31s %d", name, &score) == 2 && user_count < MAX_USERS) {
        user_list[user_count].name = strdup(name);
        user_list[user_count].total_scores = score;
        user_count++;
    }
    fclose(fp);

    // Choose a user
    user__print_user_list();
    printf("Please choose a user by name, or add a new user by typing a new name: ");
    char input[MAX_NAME_LEN];
    scanf("%31s", input);
    if (user__choose_user(input) != NULL) {
        printf("User %s chosen.\n", input);
    } else {
        user__add_user(input);
        printf("New user %s added.\n", input);
    }
}

void user__save() {
    FILE* fp = fopen(USER_FILE, "w");
    if (!fp) {
        return;
    }

    for (int i = 0; i < user_count; ++i) {
        fprintf(fp, "%s %d\n", user_list[i].name, user_list[i].total_scores);
    }
    fclose(fp);
}

void user__print_user_list() {
    if (user_count == 0) {
        printf("No users found. Please add a user.\n");
        return;
    }

    printf("User list: \n");
    for (int i = 0; i < user_count; ++i) {
        printf("%d. %s (score: %d)\n", i + 1, user_list[i].name, user_list[i].total_scores);
    }
}

struct User* user__choose_user(char* name) {
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(user_list[i].name, name) == 0) {
            current_user = &user_list[i];
            return current_user;
        }
    }

    return NULL;
}

struct User* user__add_user(char* name) {
    if (user_count >= MAX_USERS) return false;
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(user_list[i].name, name) == 0) return false;
    }
    user_list[user_count].name = strdup(name);
    user_list[user_count].total_scores = 0;
    current_user = &user_list[user_count];
    user_count++;
    return current_user;
}

void user__print_current_user() {
    if (current_user) {
        printf("Current user: %s (score: %d)\n", current_user->name, current_user->total_scores);
    } else {
        printf("Current user is not choosed\n");
    }
}

struct User* user__get_current_user() {
    return current_user;
}