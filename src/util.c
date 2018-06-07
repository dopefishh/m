#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <glob.h>
#include <sys/stat.h>
#include <errno.h>

#include "log.h"
#include "util.h"

void perrordie(char *prg)
{
	perror(prg);
	die("Aborting\n");
}

void die(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void *safe_malloc(size_t s)
{
	void *r = malloc(s);
	if (r == NULL)
		perrordie("malloc");
	return r;
}

void *safe_calloc(size_t nm, size_t s)
{
	void *r = calloc(nm, s);
	if (r == NULL)
		perrordie("calloc");
	return r;
}

void *safe_realloc(void *ptr, size_t s)
{
	ptr = realloc(ptr, s);
	if (ptr == NULL)
		perrordie("malloc");
	return ptr;
}

void safe_free(int count, ...)
{
	va_list ap;
	va_start(ap, count);
	for (int i = 0; i<count; i++)
		free(va_arg(ap, void *));
	va_end(ap);
}

char *safe_strcat(int count, ...)
{
	va_list ap;
	va_start(ap, count);
	unsigned long int len = 0;
	for (int i = 0; i<count; i++)
		len += strlen(va_arg(ap, char *));
	va_end(ap);

	va_start(ap, count);
	char *r = safe_malloc(len+1);
	r[0] = '\0';
	for (int i = 0; i<count; i++){
		strcat(r, va_arg(ap, char *));
	}
	va_end(ap);
	return r;
}

char *safe_strdup(const char *s)
{
	char *r = strdup(s);
	if (r == NULL)
		perrordie("strdup");
	return r;
}

char *safe_strndup(const char *s, size_t n)
{
	char *r = strndup(s, n);
	if (r == NULL)
		perrordie("strndup");
	return r;
}

char *trim(char *s)
{
	while(isspace(s[0]))
		s++;
	size_t i = strlen(s)-1;
	while(isspace(s[i]) && i>0)
		s[i--] = '\0';
	return s;
}

char *rtrimc(char *s, char c)
{
	size_t i = strlen(s)-1;
	while(c == s[i] && i>0)
		s[i--] = '\0';
	return s;
}

char *get_line(FILE *f, bool nl)
{
	size_t total = 128, read = 0;
	char *b = safe_malloc(total);
	b[0] = '\0';
	while(true){
		if(total > 1024*1024*1024)
			die("Too big of a line\n");

		if(read >= total - 1)
			b = safe_realloc(b, total+=128);

		safe_fgets(b+read, total-read, f);
		read += strlen(b+read);

		if(b[read-1] == '\n'){
			b[--read] = '\0';
			if(read > 0 && b[read-1] == '\\'){
				if(nl)
					b[read-1] = '\n';
				else
					b[--read] = '\0';
			} else {
				break;
			}
		}

		if(feof(f))
			break;
	}
	return b;
}

char *safe_getenv(char *env, char *def)
{
	char *t;
	if ((t = getenv(env)) != NULL)
		def = t;
	return safe_strdup(def);
}

FILE *safe_fopen(char *path, char *mode)
{
	FILE *r = fopen(path, mode);
	if(r == NULL)
		perrordie("fopen");
	return r;
}

void safe_fclose(FILE *f)
{
	if(fclose(f) != 0)
		perrordie("fclose");
}

void safe_fputs(char *m, FILE *f)
{
	if(fputs(m, f) < 0)
		perrordie("fputs");
}

void safe_fputc(char m, FILE *f)
{
	if(fputc(m, f) == EOF)
		perrordie("fputc");
}

void safe_fgets(char *m, int size, FILE *f)
{
	if(fgets(m, size, f) == NULL && !feof(f))
		perrordie("fgets");
}

char safe_getc(FILE *f)
{
	char c = getc(f);
	if(c == EOF && !feof(f))
		perrordie("getc");
	return c;
}

void safe_ungetc(int c, FILE *f)
{
	if(ungetc(c, f) == EOF)
		perrordie("ungetc");
}

void safe_fprintf(FILE *f, char *m, ...)
{
	va_list ap;
	va_start(ap, m);
	safe_vfprintf(f, m, ap);
	va_end(ap);
}

void safe_vfprintf(FILE *f, char *m, va_list ap)
{
	if(vfprintf(f, m, ap) < 0)
		perrordie("vfprintf");
}

size_t safe_fread(void *ptr, size_t size, size_t nmemb, FILE *f)
{
	size_t r = fread(ptr, size, nmemb, f);
	if(r == 0 && ferror(f))
		perrordie("fread");
	return r;
}

size_t safe_fwrite(void *ptr, size_t size, size_t nmemb, FILE *f)
{
	size_t r = fwrite(ptr, size, nmemb, f);
	if(r == 0 && ferror(f))
		perrordie("fread");
	return r;
}

DIR *safe_opendir(char *d)
{
	DIR *r = opendir(d);
	if(r == NULL)
		perrordie("opendir");
	return r;
}

void safe_closedir(DIR *d)
{
	if(closedir(d) == -1)
		perrordie("closedir");

}

long safe_telldir(DIR *d)
{
	long r = telldir(d);
	if(r == -1)
		perrordie("telldir");
	return r;
}

void safe_seekdir(DIR *d, long loc)
{
	seekdir(d, loc);
}

struct dirent *safe_readdir(DIR *d)
{
	errno = 0;
	struct dirent *r = readdir(d);
	if(r == NULL && errno != 0)
		perrordie("readdir");
	return r;
}

time_t safe_time(time_t *tloc)
{
	time_t t = time(tloc);
	if(t == (time_t)-1)
		perrordie("time");
	return t;
}

char *get_file(char *home, char *file)
{
	char *path = safe_strcat(3, home, "/", file);
	char *resolved_path = resolve_tilde(path);
	free(path);
	return resolved_path;
}

char *get_file_if_exist(char *home, char *file)
{
	char *path = get_file(home, file);
	if (path_exists(path))
		return path;
	free(path);
	return NULL;
}

bool path_exists(const char *path)
{
	struct stat buf;
	return (stat(path, &buf) == 0);
}

char *resolve_tilde(const char *path)
{
	static glob_t globbuf;
	char *head, *tail, *result = NULL;

	tail = strchr(path, '/');
	head = strndup(path, tail ? (size_t)(tail - path) : strlen(path));

	int res = glob(head, GLOB_TILDE, NULL, &globbuf);
	free(head);
	if (res == GLOB_NOMATCH || globbuf.gl_pathc != 1) {
		result = safe_strdup(path);
	} else if (res != 0) {
		die("glob failed\n");
	} else {
		head = globbuf.gl_pathv[0];
		result = calloc(strlen(head) +
			(tail ? strlen(tail) : 0) + 1, 1);
		strncpy(result, head, strlen(head));
		if (tail)
			strncat(result, tail, strlen(tail));
	}
	globfree(&globbuf);

	return result;
}

void mkdir_p(char *path)
{
	if(path_exists(path))
		return;
	char oldc;
	char *tail = strchr(path, '/');
	while(tail != NULL){
		oldc = tail[1];
		tail[1] = '\0';
		if(!path_exists(path)){
			logmsg(debug, "Creating directory: %s\n", path);
			mkdir(path, 0755);
		}
		tail[1] = oldc;
		tail = strchr(tail+1, '/');
	}
}
