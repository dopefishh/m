#ifndef DB_IO_H
#define DB_IO_H

#include <stdio.h>
#include <stdint.h>

#include "../db.h"

void serialize_db(FILE *, struct db *, bool);
struct db *deserialize_db(FILE *);
#endif
