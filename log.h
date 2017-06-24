#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include "config.h"

enum loglevel { warn, info, debug };

void init_logging();
void set_logfile(char *f);

void logmsg(enum loglevel lvl, char *msg, ...);

void increase_loglevel();
void decrease_loglevel();

void close_logfile();
#endif
