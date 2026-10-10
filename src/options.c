#include "options.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void init_options(Options *opts) {
    opts->opt_A = false;
    opts->opt_a = false;
    opts->opt_c = false;
    opts->opt_d = false;
    opts->opt_F = false;
    opts->opt_f = false;
    opts->opt_h = false;
    opts->opt_i = false;
    opts->opt_k = false;
    opts->opt_l = false;
    opts->opt_n = false;
    opts->opt_R = false;
    opts->opt_r = false;
    opts->opt_S = false;
    opts->opt_s = false;
    opts->opt_t = false;
    opts->opt_u = false;

    /* Terminal mặc định bật -q, xuất ra pipe/file mặc định bật -w */
    if (isatty(STDOUT_FILENO)) {
        opts->opt_q = true;
        opts->opt_w = false;
    } else {
        opts->opt_q = false;
        opts->opt_w = true;
    }

    /* Đọc biến môi trường BLOCKSIZE, mặc định chuẩn BSD là 512 bytes */
    char *bs = getenv("BLOCKSIZE");
    if (bs != NULL) {
        opts->blocksize = atol(bs);
        if (opts->blocksize <= 0) {
            opts->blocksize = 512;
        }
    } else {
        opts->blocksize = 512;
    }
}

int parse_options(int argc, char *argv[], Options *opts, int *optind_out) {
    int ch;
    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
            case 'A':
                opts->opt_A = true;
                break;
            case 'a':
                opts->opt_a = true;
                break;
            case 'c':
                opts->opt_c = true;
                opts->opt_u = false; /* -c và -u ghi đè lẫn nhau */
                break;
            case 'd':
                opts->opt_d = true;
                opts->opt_R = false; /* -d và -R ghi đè lẫn nhau */
                break;
            case 'F':
                opts->opt_F = true;
                break;
            case 'f':
                opts->opt_f = true;
                opts->opt_a = true; /* -f tắt sắp xếp và tự động kích hoạt -a */
                break;
            case 'h':
                opts->opt_h = true;
                opts->opt_k = false; /* -h và -k ghi đè lẫn nhau */
                break;
            case 'i':
                opts->opt_i = true;
                break;
            case 'k':
                opts->opt_k = true;
                opts->opt_h = false; /* -k và -h ghi đè lẫn nhau */
                break;
            case 'l':
                opts->opt_l = true;
                opts->opt_n = false; /* -l và -n ghi đè lẫn nhau */
                break;
            case 'n':
                opts->opt_n = true;
                opts->opt_l = false; /* -n và -l ghi đè lẫn nhau */
                break;
            case 'q':
                opts->opt_q = true;
                opts->opt_w = false; /* -q và -w ghi đè lẫn nhau */
                break;
            case 'R':
                opts->opt_R = true;
                opts->opt_d = false; /* -R và -d ghi đè lẫn nhau */
                break;
            case 'r':
                opts->opt_r = true;
                break;
            case 'S':
                opts->opt_S = true;
                break;
            case 's':
                opts->opt_s = true;
                break;
            case 't':
                opts->opt_t = true;
                break;
            case 'u':
                opts->opt_u = true;
                opts->opt_c = false; /* -u và -c ghi đè lẫn nhau */
                break;
            case 'w':
                opts->opt_w = true;
                opts->opt_q = false; /* -w và -q ghi đè lẫn nhau */
                break;
            default:
                return -1;
        }
    }
    *optind_out = optind;
    return 0;
}