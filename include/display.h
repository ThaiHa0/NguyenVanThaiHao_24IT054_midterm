#ifndef DISPLAY_H
#define DISPLAY_H

#include "entry.h"
#include "options.h"
#include <stddef.h>

void print_entries(FileEntry **entries, size_t count, const Options *opts, bool is_dir_content);

#endif