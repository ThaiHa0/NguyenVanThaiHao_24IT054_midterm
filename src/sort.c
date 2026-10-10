#include "sort.h"
#include <string.h>
#include <stdlib.h>

/* Sử dụng biến toàn cục tĩnh để truyền Options vào hàm compare của qsort */
static const Options *g_opts = NULL;

/* Hàm hỗ trợ lấy mốc thời gian tùy theo cờ -c hoặc -u */
static time_t get_entry_time(const FileEntry *e) {
    if (g_opts->opt_c) {
        return e->sb.st_ctime; /*[cite: 1] */
    }
    if (g_opts->opt_u) {
        return e->sb.st_atime; /*[cite: 2] */
    }
    return e->sb.st_mtime;     /* Mặc định[cite: 1, 2] */
}

/* Hàm so sánh chuẩn dành cho qsort */
static int compare_entries(const void *p1, const void *p2) {
    const FileEntry *e1 = *(const FileEntry **)p1;
    const FileEntry *e2 = *(const FileEntry **)p2;
    int res = 0;

    if (g_opts->opt_S) {
        /* Sắp xếp theo kích thước (st_size) giảm dần[cite: 1] */
        if (e1->sb.st_size < e2->sb.st_size) res = 1;
        else if (e1->sb.st_size > e2->sb.st_size) res = -1;
        else res = strcmp(e1->name, e2->name); /* Bằng nhau thì so thứ tự từ điển */
    } else if (g_opts->opt_t) {
        /* Sắp xếp theo mốc thời gian giảm dần (mới nhất lên đầu)[cite: 2] */
        time_t t1 = get_entry_time(e1);
        time_t t2 = get_entry_time(e2);
        if (t1 < t2) res = 1;
        else if (t1 > t2) res = -1;
        else res = strcmp(e1->name, e2->name); /* Bằng nhau thì so thứ tự từ điển[cite: 2] */
    } else {
        /* Sắp xếp theo thứ tự từ điển (lexicographical order)[cite: 1] */
        res = strcmp(e1->name, e2->name);
    }

    /* Đảo ngược kết quả nếu có cờ -r[cite: 1] */
    return g_opts->opt_r ? -res : res;
}

void sort_entries(FileEntry **entries, size_t count, const Options *opts) {
    /* Bỏ qua toàn bộ bước sắp xếp nếu có cờ -f hoặc số lượng <= 1[cite: 1] */
    if (opts->opt_f || count <= 1) {
        return;
    }
    
    g_opts = opts;
    qsort(entries, count, sizeof(FileEntry *), compare_entries);
    g_opts = NULL;
}