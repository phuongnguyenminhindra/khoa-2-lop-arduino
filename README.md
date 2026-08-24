# 🔐 Khóa An Toàn 2 Lớp Arduino

Project 30 ngày - Khóa 2 lớp đổi được mật khẩu ngay trên mạch, lưu vĩnh viễn.

### ✨ Tính năng
- **Lớp 1 - Biến trở (A0):** Phải xoay đúng vùng bí mật (400-600)
- **Lớp 2 - Mật khẩu (D4,D5,D7):** Bấm đúng thứ tự A->B->C, mỗi bước <2s
- **Đổi MK không cần code:** Giữ nút A 3s = đổi vùng biến trở, Giữ C 3s = đổi thứ tự A-B-C
- **Chống trộm:** Sai 3 lần -> Khóa 30s, đèn đỏ + còi hú nhấp nháy 100ms
- **Chống gian lận:** `docNut()` phát hiện bấm 2 nút cùng lúc -> tính là sai
- **Nhớ vĩnh viễn:** Lưu EEPROM, cúp điện không mất

### 🔧 Đấu dây
- LED Đỏ: D2, Xanh: D3
- Nút A: D4, B: D5, C: D7 (INPUT_PULLUP)
- Còi qua IC NOT: D6 (LOW = kêu)
- Biến trở: A0

### 🚀 Demo
1. Xoay biến trở đúng -> xanh chớp
2. Bấm A->B->C -> xanh sáng 1s + tít -> Mở khóa!

Làm bởi Phương Nguyễn - Arduino 30 Days Challenge
