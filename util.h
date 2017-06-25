#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "util.h"

// Dying
void perrordie(char *prg);
void die(char *msg, ...);

// Allocation
void *safe_malloc(size_t s);
void *safe_realloc(void *ptr, size_t s);
void safe_free(int count, ...);

// String
char *safe_strcat(int count, ...);
char *safe_strdup(const char *s);
char *trim(char *s);

// File
FILE *safe_fopen(char *path, char *mode);
void safe_fclose(FILE *f);
void safe_fputs(char *m, FILE *f);
void safe_fgets(char *m, int size, FILE *f);
void safe_fprintf(FILE *f, char *m, ...);
void safe_vfprintf(FILE *f, char *m, va_list ap);
size_t safe_fread(void *ptr, size_t size, size_t nmemb, FILE *f);
size_t safe_fwrite(void *ptr, size_t size, size_t nmemb, FILE *f);

// Time
time_t safe_time(time_t *tloc);

// Environment
char *safe_getenv(char *env, char *def);

// Path functions
char *get_file(char *home, char *file);
char *get_file_if_exist(char *home, char *file);
bool path_exists(const char *path);
char *resolve_tilde(const char *path);
void mkdir_p(char *path);

#endif
