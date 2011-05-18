#include "log.h"
#include "c_io.h"

debug_level_e level;

void inline error(char *str, ...) {
	if (level <= l_error) {
		c_vprintf(&str);
	}
}

void inline warn(char *str, ...) {
	if (level <= l_warning) {
		c_vprintf(&str);
	}
}

void inline info(char *str, ...) {
	if (level <= l_info) {
		c_vprintf(&str);
	}
}

void inline debug(char *str, ...) {
	if (level <= l_debug) {
		c_vprintf(&str);
	}
}

void inline setDebugLevel(debug_level_e lvl) {
	level = lvl;
}

