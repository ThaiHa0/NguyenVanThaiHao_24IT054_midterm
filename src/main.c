#include "options.h"
#include "traverse.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    Options opts;
    
    /* 1. Khởi tạo giá trị mặc định cho cấu trúc Options */
    init_options(&opts);

    /* 2. Phân tích cờ lệnh qua getopt */
    int optind_out = 1;
    if (parse_options(argc, argv, &opts, &optind_out) != 0) {
        fprintf(stderr, "Usage: ls [-AacdFfhiklnqRrSstuw] [file...]\n");
        return 1; /* Thoát >0 nếu có lỗi xảy ra[cite: 3] */
    }

    /* 3. Lấy danh sách các operands (đường dẫn truyền vào) bỏ qua các cờ lệnh */
    int operand_count = argc - optind_out;
    char **operands = &argv[optind_out];

    /* 4. Giao nhiệm vụ cho khối duyệt cây thư mục */
    process_operands(operand_count, operands, &opts);

    return 0; /* Thoát 0 khi thành công[cite: 3] */
}