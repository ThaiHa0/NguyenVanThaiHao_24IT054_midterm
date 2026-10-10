#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

typedef struct {
    bool opt_A;     /* Liệt kê tất cả trừ . và .. */
    bool opt_a;     /* Bao gồm cả file ẩn */
    bool opt_c;     /* Dùng ctime */
    bool opt_d;     /* Liệt kê thư mục như file thường */
    bool opt_F;     /* Thêm ký tự định danh kiểu file */
    bool opt_f;     /* Không sắp xếp, tự động bật -a */
    bool opt_h;     /* Dung lượng human-readable */
    bool opt_i;     /* In số inode */
    bool opt_k;     /* Đơn vị block/dung lượng theo KB */
    bool opt_l;     /* Long listing format */
    bool opt_n;     /* Numeric UID/GID */
    bool opt_q;     /* Đổi ký tự không in được thành '?' */
    bool opt_R;     /* Đệ quy thư mục */
    bool opt_r;     /* Đảo ngược thứ tự sắp xếp */
    bool opt_S;     /* Sắp xếp theo dung lượng */
    bool opt_s;     /* In số blocks thực tế */
    bool opt_t;     /* Sắp xếp theo mốc thời gian */
    bool opt_u;     /* Dùng atime */
    bool opt_w;     /* In thô (raw) ký tự không in được */
    long blocksize; /* Giá trị từ biến môi trường BLOCKSIZE */
} Options;

void init_options(Options *opts);
int parse_options(int argc, char *argv[], Options *opts, int *optind_out);

#endif