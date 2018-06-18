#ifndef FORMAT_H
#define FORMAT_H

#include <stdbool.h>

#include "list.h"
#include "db.h"

enum fmt_atom_type {fmt_lit, fmt_fun};

struct fmt_atom {
	union {
		char *lit;
		struct {
			char *name;
			struct listitem *args;
		} fun;
	} atom;
	bool islit;
};

struct listitem *parse_fmt_atoms(char *);
void  fformat(FILE *, struct listitem *, struct db_file *);
char *sformat(struct listitem *, struct db_file *);
void fprint(FILE *, struct listitem *);

void fmt_free(struct listitem *);

#endif
