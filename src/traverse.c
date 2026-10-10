#include "traverse.h"
#include "entry.h"
#include "sort.h"
#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/* Hàm duyệt và xử lý nội dung bên trong một thư mục */
static void traverse_directory(const char *dir_path, const Options *opts, bool print_header) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "ls: %s: Permission denied\n", dir_path);
        return;
    }

    /* In tên thư mục trước khi liệt kê nội dung nếu có nhiều operand hoặc đang đệ quy[cite: 1] */
    if (print_header) {
        printf("%s:\n", dir_path);
    }

    struct dirent *dp;
    FileEntry **entries = NULL;
    size_t count = 0, capacity = 0;

    /* Đọc toàn bộ file con trong thư mục */
    while ((dp = readdir(dir)) != NULL) {
        /* Xử lý lọc file ẩn theo cờ -a và -A[cite: 1] */
        if (!opts->opt_a && !opts->opt_A && dp->d_name[0] == '.') {
            continue; /* Mặc định bỏ qua file bắt đầu bằng '.' */
        }
        if (opts->opt_A && !opts->opt_a) {
            /* Cờ -A bỏ qua chính xác 2 mục '.' và '..'[cite: 1] */
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                continue;
            }
        }

        FileEntry *e = create_entry(dir_path, dp->d_name);
        if (!e) continue;

        /* Cấp phát động mảng lưu trữ entry */
        if (count >= capacity) {
            capacity = (capacity == 0) ? 16 : capacity * 2;
            entries = realloc(entries, capacity * sizeof(FileEntry *));
        }
        entries[count++] = e;
    }
    closedir(dir);

    /* Sắp xếp và in nội dung thư mục hiện tại */
    sort_entries(entries, count, opts);
    print_entries(entries, count, opts, true);

    /* Duyệt đệ quy nếu có cờ -R[cite: 1] */
    if (opts->opt_R) {
        for (size_t i = 0; i < count; i++) {
            if (entries[i]->is_dir && entries[i]->error == 0) {
                /* Bỏ qua '.' và '..' để tránh lặp vô hạn trong đệ quy */
                if (strcmp(entries[i]->name, ".") != 0 && strcmp(entries[i]->name, "..") != 0) {
                    putchar('\n');
                    traverse_directory(entries[i]->path, opts, true);
                }
            }
        }
    }

    /* Dọn dẹp bộ nhớ */
    for (size_t i = 0; i < count; i++) {
        free_entry(entries[i]);
    }
    free(entries);
}

/* Hàm phân tách và xử lý các đường dẫn đầu vào (operands) */
int process_operands(int count, char **paths, const Options *opts) {
    /* Nếu không có operand nào, mặc định là thư mục hiện tại '.'[cite: 1] */
    if (count == 0) {
        traverse_directory(".", opts, false);
        return 0;
    }

    FileEntry **non_dirs = NULL;
    size_t non_dir_count = 0, non_dir_cap = 0;

    FileEntry **dirs = NULL;
    size_t dir_count = 0, dir_cap = 0;

    /* Phân loại từng đường dẫn đầu vào thành non-directory và directory[cite: 1] */
    for (int i = 0; i < count; i++) {
        FileEntry *e = create_entry("", paths[i]);
        if (e->error != 0) {
            fprintf(stderr, "ls: %s: No such file or directory\n", paths[i]);
            free_entry(e);
            continue;
        }

        /* Nếu bật cờ -d, coi thư mục như file thông thường[cite: 1] */
        if (!e->is_dir || opts->opt_d) {
            if (non_dir_count >= non_dir_cap) {
                non_dir_cap = (non_dir_cap == 0) ? 8 : non_dir_cap * 2;
                non_dirs = realloc(non_dirs, non_dir_cap * sizeof(FileEntry *));
            }
            non_dirs[non_dir_count++] = e;
        } else {
            if (dir_count >= dir_cap) {
                dir_cap = (dir_cap == 0) ? 8 : dir_cap * 2;
                dirs = realloc(dirs, dir_cap * sizeof(FileEntry *));
            }
            dirs[dir_count++] = e;
        }
    }

    /* 1. Xử lý các mục non-directory trước tiên[cite: 1] */
    if (non_dir_count > 0) {
        sort_entries(non_dirs, non_dir_count, opts);
        print_entries(non_dirs, non_dir_count, opts, false);
        
        for (size_t i = 0; i < non_dir_count; i++) free_entry(non_dirs[i]);
        free(non_dirs);
        
        if (dir_count > 0) putchar('\n'); /* Tách biệt giữa non-dir và dir */
    }

    /* 2. Xử lý các mục directory sau đó[cite: 1] */
    if (dir_count > 0) {
        sort_entries(dirs, dir_count, opts);
        
        /* Nếu có tổng cộng hơn 1 operand (cả dir và non-dir) thì in tên thư mục làm tiêu đề */
        bool print_hdr = (count > 1 || opts->opt_R);
        
        for (size_t i = 0; i < dir_count; i++) {
            if (i > 0) putchar('\n');
            traverse_directory(dirs[i]->path, opts, print_hdr);
            free_entry(dirs[i]);
        }
        free(dirs);
    }

    return 0;
}