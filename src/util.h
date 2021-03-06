#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

#include "util.h"

// Dying
void perrordie(char *prg);
void die(const char *msg, ...);

// Allocation
void *safe_malloc(size_t s);
void *safe_calloc(size_t nm, size_t s);
void *safe_realloc(void *ptr, size_t s);
void safe_free(int count, ...);

// String
char *safe_strcat(int count, ...);
char *safe_strdup(const char *s);
char *safe_strndup(const char *s, size_t n);

/*
 * Trims the whitespace off the string in-place on both sides.
 */
char *trim(char *s);

/*
 * This trims off given characters from the end of the string in-place.
 */
char *rtrimc(char *s, char c);

/*
 * Retrieves a line from the given file. If the newline flag is set, escaped
 * newlines are inserted in the string.
 */
char *get_line(FILE *, bool);

// File
FILE *safe_fopen(char *path, char *mode);
void safe_fclose(FILE *f);
void safe_fputs(char *m, FILE *f);
void safe_fputc(char m, FILE *f);
void safe_fgets(char *m, int size, FILE *f);
char safe_getc(FILE *f);
void safe_ungetc(int c, FILE *f);
void safe_fprintf(FILE *f, char *m, ...);
void safe_vfprintf(FILE *f, char *m, va_list ap);
size_t safe_fread(void *ptr, size_t size, size_t nmemb, FILE *f);
size_t safe_fwrite(void *ptr, size_t size, size_t nmemb, FILE *f);

// Directory
DIR *safe_opendir(char *d);
void safe_closedir(DIR *d);
long safe_telldir(DIR *d);
void safe_seekdir(DIR *d, long loc);
struct dirent *safe_readdir(DIR *d);

// Time
time_t safe_time(time_t *tloc);

// Environment
char *safe_getenv(char *env, char *def);

// Path functions
char *get_file(char *home, char *file);
char *get_file_if_exist(char *home, char *file);
bool path_exists(const char *path);
bool is_file(const char *path);
char *resolve_tilde(const char *path);
void mkdir_p(char *path);

#endif
