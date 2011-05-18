#ifndef LOG_H
#define LOG_H

#define DL_ERROR   3
#define DL_WARNING 2
#define DL_INFO    1
#define DL_DEBUG   0

typedef enum _debug_level {
	l_error,
	l_warning,
	l_info,
	l_debug
} debug_level_e;

void inline error(char *str, ...);
void inline warn(char *str, ...);
void inline info(char *str, ...);
void inline debug(char *str, ...);

void inline setDebugLevel(debug_level_e lvl);

#endif

