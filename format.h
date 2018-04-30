#ifndef FORMAT_H
#define FORMAT_H

#include "list.h"
#include "db.h"

enum fmt_fun_type {fmt_fun_tag, fmt_fun_fallback};
enum fmt_atom_type {fmt_lit, fmt_fun};

struct fmt_atom {
	union {
		char *lit;
		struct {
			enum fmt_fun_type type;
			struct listitem *args;
		} fun;
	} atom;
	enum fmt_atom_type which;
};

struct listitem *parse_fmt_atoms(char *);
void fformat(FILE *, struct listitem *, struct db_file *);

void fmt_free(void *);

#endif
