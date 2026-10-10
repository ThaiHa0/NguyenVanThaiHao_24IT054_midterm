#ifndef SORT_H
#define SORT_H

#include "entry.h"
#include "options.h"
#include <stddef.h>

void sort_entries(FileEntry **entries, size_t count, const Options *opts);

#endif