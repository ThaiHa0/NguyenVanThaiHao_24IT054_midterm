# 🗂️ UNIX File Listing Utility · System Programming Midterm
> Trình mô phỏng tiện ích lệnh `ls(1)` bằng ngôn ngữ C trên hệ điều hành NetBSD 10.1.

---

## 🧭 Mục lục
- [📖 Tổng quan](#-tổng-quan)
- [✨ Tính năng & Cờ lệnh](#-tính-năng--cờ-lệnh)
- [🧰 Công nghệ & Môi trường](#-công-nghệ--môi-trường)
- [📁 Cấu trúc dự án](#-cấu-trúc-dự-án)
- [⚙️ Biên dịch](#️-biên-dịch)
- [🚀 Cách sử dụng & Kiểm thử](#-cách-sử-dụng--kiểm-thử)
- [🔗 Kho mã nguồn](#-kho-mã-nguồn)

---

## 📖 Tổng quan
Dự án giữa kỳ môn **Lập trình hệ thống** thực hiện việc tái tạo lại lệnh `ls` của UNIX/NetBSD từ đầu bằng ngôn ngữ C (chuẩn C99). Dự án tương tác trực tiếp với Kernel và Hệ thống tệp (File System) thông qua các System Calls / APIs POSIX như `opendir()`, `readdir()`, `lstat()`, `readlink()`, `getpwuid()`, `getgrgid()`.

**Cú pháp:**
```bash
./ls [-AacdFfhiklnqRrSstuw] [file ...]