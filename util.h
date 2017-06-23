#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "util.h"

void perrordie(char *prg);
void die(char *msg, ...);
void *safe_malloc(unsigned long int s);
void safe_free(int count, ...);
char *safe_strcat(int count, ...);
char *safe_strdup(const char *s);
char *safe_getenv(char *env, char *def);
char *get_file(char *home, char *file);
char *get_file_if_exist(char *home, char *file);
bool path_exists(const char *path);
char *resolve_tilde(const char *path);

#endif
