#ifndef ENTRY_H
#define ENTRY_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef struct {
    char *name;         /* Tên hiển thị */
    char *path;         /* Đường dẫn đầy đủ để gọi lstat/readlink */
    struct stat sb;     /* Metadata của file từ lstat */
    char *link_target;  /* Chuỗi đích nếu là symbolic link */
    int error;          /* Lưu errno nếu lstat thất bại */
    bool is_dir;        /* Xác định có phải directory không */
} FileEntry;

FileEntry *create_entry(const char *parent, const char *name);
void free_entry(FileEntry *entry);

#endif