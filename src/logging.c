#include "logging.h"

LogLevel current_log_level = LOG_LEVEL_DEBUG;

void set_log_level(LogLevel level) { current_log_level = level; }
