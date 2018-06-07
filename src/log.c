#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "config.h"
#include "util.h"
#include "log.h"

FILE *logfile;
enum loglevel loglevel = info;

void init_logging()
{
	logfile = stdout;
}

void set_logfile(char *f)
{
	close_logfile();
	logfile = safe_fopen(f, "w");
}

void logmsg(enum loglevel lvl, char *msg, ...)
{
	if(loglevel >= lvl){
		va_list ap;
		va_start(ap, msg);
		if(lvl == debug)
			safe_fputs("debug: ", logfile);
		else if (lvl == info)
			safe_fputs("info : ", logfile);
		else
			safe_fputs("warn : ", logfile);
		safe_vfprintf(logfile, msg, ap);
		va_end(ap);
	}
}

void increase_loglevel()
{
	loglevel = loglevel == debug ? debug : loglevel + 1;
}

void decrease_loglevel()
{
	loglevel = loglevel == warn ? warn : loglevel - 1;
}

void close_logfile()
{
	if(logfile != stdout && logfile != stderr)
		safe_fclose(logfile);
	logfile = stdout;
}
