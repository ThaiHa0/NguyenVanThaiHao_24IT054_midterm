#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/types.h>

/* Chuyển st_mode thành chuỗi 10 ký tự quyền hạn theo chuẩn NetBSD[cite: 2, 3] */
static void mode_to_str(mode_t mode, char *out) {
    strcpy(out, "----------");
    
    /* Xác định loại file[cite: 2] */
    if (S_ISDIR(mode)) out[0] = 'd';
    else if (S_ISLNK(mode)) out[0] = 'l';
    else if (S_ISCHR(mode)) out[0] = 'c';
    else if (S_ISBLK(mode)) out[0] = 'b';
    else if (S_ISFIFO(mode)) out[0] = 'p';
    else if (S_ISSOCK(mode)) out[0] = 's';
#ifdef S_ISWHT
    else if (S_ISWHT(mode)) out[0] = 'w'; /* Whiteout đặc thù của BSD[cite: 2] */
#endif

    /* Quyền Owner[cite: 3] */
    if (mode & S_IRUSR) out[1] = 'r';
    if (mode & S_IWUSR) out[2] = 'w';
    if (mode & S_ISUID) out[3] = (mode & S_IXUSR) ? 's' : 'S';
    else if (mode & S_IXUSR) out[3] = 'x';

    /* Quyền Group[cite: 3] */
    if (mode & S_IRGRP) out[4] = 'r';
    if (mode & S_IWGRP) out[5] = 'w';
    if (mode & S_ISGID) out[6] = (mode & S_IXGRP) ? 's' : 'S';
    else if (mode & S_IXGRP) out[6] = 'x';

    /* Quyền Other & Sticky bit[cite: 3] */
    if (mode & S_IROTH) out[7] = 'r';
    if (mode & S_IWOTH) out[8] = 'w';
    if (mode & S_ISVTX) out[9] = (mode & S_IXOTH) ? 't' : 'T';
    else if (mode & S_IXOTH) out[9] = 'x';
}

/* Định dạng kích thước sang Human Readable nếu có cờ -h[cite: 1] */
static void format_human_size(off_t size, char *buf, size_t len) {
    const char *units[] = {"B", "K", "M", "G", "T", "P"};
    int unit = 0;
    double d_size = (double)size;
    while (d_size >= 1024.0 && unit < 5) {
        d_size /= 1024.0;
        unit++;
    }
    if (unit == 0) snprintf(buf, len, "%lldB", (long long)size);
    else snprintf(buf, len, "%.1f%s", d_size, units[unit]);
}

/* In tên file, xử lý ký tự không in được nếu cờ -q đang bật[cite: 1] */
static void print_sanitized_name(const char *name, bool opt_q) {
    for (size_t i = 0; name[i] != '\0'; i++) {
        unsigned char c = (unsigned char)name[i];
        if (opt_q && !isprint(c)) putchar('?');
        else putchar(c);
    }
}

/* In hậu tố đặc biệt theo loại file nếu cờ -F đang bật[cite: 1] */
static void print_type_suffix(mode_t mode, bool opt_F) {
    if (!opt_F) return;
    
    if (S_ISDIR(mode)) putchar('/');
    else if (S_ISLNK(mode)) putchar('@');
    else if (S_ISSOCK(mode)) putchar('=');
    else if (S_ISFIFO(mode)) putchar('|');
#ifdef S_ISWHT
    else if (S_ISWHT(mode)) putchar('%');
#endif
    else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) putchar('*');
}

/* Tính số lượng block (làm tròn lên) theo BLOCKSIZE hoặc cờ -k[cite: 1] */
static long long get_blocks(const FileEntry *e, const Options *opts) {
    long long bytes = (long long)e->sb.st_blocks * 512;
    long bs = (opts->opt_k) ? 1024 : opts->blocksize;
    return (bytes + bs - 1) / bs;
}

void print_entries(FileEntry **entries, size_t count, const Options *opts, bool is_dir_content) {
    if (count == 0) return;

    /* In tổng số block của thư mục nếu có các cờ -l, -n, -s[cite: 2] */
    if (is_dir_content && (opts->opt_l || opts->opt_n || opts->opt_s)) {
        long long total_blocks = 0;
        for (size_t i = 0; i < count; i++) {
            if (entries[i]->error == 0) {
                total_blocks += get_blocks(entries[i], opts);
            }
        }
        
        if (opts->opt_h) {
            char h_buf[32];
            format_human_size(total_blocks * (opts->opt_k ? 1024 : opts->blocksize), h_buf, sizeof(h_buf));
            printf("total %s\n", h_buf);
        } else {
            printf("total %lld\n", total_blocks);
        }
    }

    /* Lặp qua danh sách in từng entry */
    for (size_t i = 0; i < count; i++) {
        FileEntry *e = entries[i];
        
        if (e->error != 0) {
            fprintf(stderr, "ls: cannot access '%s'\n", e->name);
            continue;
        }

        /* [Cột 1] Inode nếu có -i[cite: 1] */
        if (opts->opt_i) {
            printf("%7llu ", (unsigned long long)e->sb.st_ino);
        }

        /* [Cột 2] Block size nếu có -s[cite: 1] */
        if (opts->opt_s) {
            long long blks = get_blocks(e, opts);
            if (opts->opt_h) {
                char h_buf[32];
                format_human_size(blks * (opts->opt_k ? 1024 : opts->blocksize), h_buf, sizeof(h_buf));
                printf("%4s ", h_buf);
            } else {
                printf("%4lld ", blks);
            }
        }

        /* [Các cột Long Format] nếu có -l hoặc -n[cite: 1, 2] */
        if (opts->opt_l || opts->opt_n) {
            char mode_str[11];
            mode_to_str(e->sb.st_mode, mode_str);
            printf("%s %2u ", mode_str, (unsigned int)e->sb.st_nlink); /* Mode và hard links[cite: 2] */

            /* In Owner và Group[cite: 2] */
            struct passwd *pw = opts->opt_n ? NULL : getpwuid(e->sb.st_uid);
            struct group *gr = opts->opt_n ? NULL : getgrgid(e->sb.st_gid);

            if (pw) printf("%-8s ", pw->pw_name);
            else printf("%-8u ", (unsigned int)e->sb.st_uid);

            if (gr) printf("%-8s ", gr->gr_name);
            else printf("%-8u ", (unsigned int)e->sb.st_gid);

            /* File size hoặc Device Numbers (character/block special)[cite: 2] */
            if (S_ISCHR(e->sb.st_mode) || S_ISBLK(e->sb.st_mode)) {
                printf("%3d, %3d ", major(e->sb.st_rdev), minor(e->sb.st_rdev));
            } else if (opts->opt_h) {
                char sz_buf[32];
                format_human_size(e->sb.st_size, sz_buf, sizeof(sz_buf));
                printf("%6s ", sz_buf);
            } else {
                printf("%8lld ", (long long)e->sb.st_size);
            }

            /* Định dạng thời gian: Tháng Ngày Giờ:Phút[cite: 2] */
            time_t t = opts->opt_c ? e->sb.st_ctime : (opts->opt_u ? e->sb.st_atime : e->sb.st_mtime);
            struct tm *tm_info = localtime(&t);
            char time_buf[64];
            strftime(time_buf, sizeof(time_buf), "%b %e %H:%M", tm_info);
            printf("%s ", time_buf);
        }

        /* In tên mục và hậu tố */
        print_sanitized_name(e->name, opts->opt_q);
        print_type_suffix(e->sb.st_mode, opts->opt_F);

        /* In đường dẫn đích của Symbolic Link[cite: 2] */
        if ((opts->opt_l || opts->opt_n) && e->link_target != NULL) {
            printf(" -> ");
            print_sanitized_name(e->link_target, opts->opt_q);
        }
        
        putchar('\n'); /* Xuống dòng sau mỗi entry[cite: 2] */
    }
}