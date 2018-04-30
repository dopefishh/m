#ifndef FORMAT_H
#define FORMAT_H

#include "list.h"
#include "db.h"

enum fmt_atom_type {fmt_lit, fmt_tag};
union fmt_atom_data {
	char *lit;
	char *tag;
};
struct fmt_atom {
	union fmt_atom_data atom;
	enum fmt_atom_type which;
};

struct listitem *parse_fmt_atoms(char *);
void fformat(FILE *, struct listitem *, struct db_file *);

void fmt_free(void *);

#endif
