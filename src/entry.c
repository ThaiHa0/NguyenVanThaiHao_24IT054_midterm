#include "entry.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

FileEntry *create_entry(const char *parent, const char *name) {
    FileEntry *entry = calloc(1, sizeof(FileEntry));
    if (!entry) {
        return NULL;
    }

    entry->name = strdup(name);
    if (!entry->name) {
        free(entry);
        return NULL;
    }

    /* Xây dựng đường dẫn đầy đủ phù hợp */
    if (parent == NULL || parent[0] == '\0') {
        entry->path = strdup(name);
    } else {
        size_t p_len = strlen(parent);
        size_t n_len = strlen(name);
        bool needs_slash = (parent[p_len - 1] != '/');
        size_t total_len = p_len + n_len + (needs_slash ? 2 : 1);

        entry->path = malloc(total_len);
        if (!entry->path) {
            free(entry->name);
            free(entry);
            return NULL;
        }

        if (needs_slash) {
            snprintf(entry->path, total_len, "%s/%s", parent, name);
        } else {
            snprintf(entry->path, total_len, "%s%s", parent, name);
        }
    }

    /* Thu thập metadata bằng lstat để giữ nguyên trạng thái symbolic link */
    if (lstat(entry->path, &entry->sb) == -1) {
        entry->error = errno;
        entry->is_dir = false;
        entry->link_target = NULL;
        return entry;
    }

    entry->error = 0;
    entry->is_dir = S_ISDIR(entry->sb.st_mode);

    /* Nếu là symbolic link, lấy đường dẫn file/thư mục đích */
    if (S_ISLNK(entry->sb.st_mode)) {
        char link_buf[1024];
        ssize_t len = readlink(entry->path, link_buf, sizeof(link_buf) - 1);
        if (len != -1) {
            link_buf[len] = '\0';
            entry->link_target = strdup(link_buf);
        } else {
            entry->link_target = NULL;
        }
    } else {
        entry->link_target = NULL;
    }

    return entry;
}

void free_entry(FileEntry *entry) {
    if (!entry) return;

    if (entry->name) {
        free(entry->name);
    }
    if (entry->path) {
        free(entry->path);
    }
    if (entry->link_target) {
        free(entry->link_target);
    }
    free(entry);
}