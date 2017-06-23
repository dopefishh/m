#include <stdbool.h>
#include <stdbool.h>
#include "util.h"

char *get_config_path(void)
{
	char *cf, *c;

	/* Check user xdg config file */
	if ((cf = get_file_if_exist(
		safe_getenv("XDG_CONFIG_HOME", "~/.config"),
			"/m/config")) != NULL){
		return cf;
	}

	/* Check system xdg config files */
	char *systempaths = safe_getenv("XDG_CONFIG_DIRS", "/etc/xdg");
	char *token, *spaths = systempaths;
	while ((token = strsep(&systempaths, ":")) != NULL){
		if ((cf = get_file_if_exist(token, "/m/config")) != NULL){
			free(spaths);
			return cf;
		}
	}

	/* No config file found, thus going for the default*/
	free(spaths);
	cf = safe_getenv("XDG_CONFIG_HOME", "~/.config");
	c = safe_strcat(2, cf, "/m/config");
	free(cf);
	cf = resolve_tilde(c);
	free(c);
	return cf;
}

char *get_data_dir(void)
{
	char *df, *home = safe_getenv("XDG_DATA_HOME", "~/.local/share");

	/* Check user data dir */
	if ((df = get_file_if_exist(home, "/m")) != NULL){
		free(home);
		return df;
	}

	/* Check system xdg config files */
	char *systempaths = safe_getenv("XDG_DATA_DIRS",
		"/usr/local/share:/usr/share");
	char *token, *spaths = systempaths;
	while ((token = strsep(&systempaths, ":")) != NULL){
		if ((df = get_file_if_exist(token, "/m")) != NULL){
			free(home);
			free(spaths);
			return df;
		}
	}

	/* No data found, thus going for the default */
	df = get_file(home, "m");
	free(spaths);
	free(home);
	return df;
}
