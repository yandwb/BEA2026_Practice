# BEA2026 Practice - CAN Communication, Diagnostics & LCD Dashboard

Đây là bài thực hành tổng hợp trên vi điều khiển STM32F4 (Board OpenX05R-C), bao gồm 2 phần chính: Giao tiếp CAN / Hiển thị LCD và Hệ thống chẩn đoán (Diagnostics UDS).

## 1. Chức năng Giao tiếp CAN và Màn hình LCD
- Giao tiếp CAN (Node 1 & Node 2): 
  - Node 2 truyền bản tin 0x0A2 theo chu kỳ 20ms chứa Value0 (tăng liên tục) và Value1.
  - Node 1 nhận 0x0A2, tính tổng Value0 + Value1, sau đó truyền lại bản tin 0x012 mỗi 50ms kèm mã lỗi CRC.
  - Node 2 nhận 0x012, kiểm tra mã CRC (theo chuẩn SAE J1850) và phản hồi bằng cách chớp nháy đèn LED.
- Màn hình LCD Dashboard (ILI9341):
  - Hiển thị theo thời gian thực (Real-time) dữ liệu RX và TX của cả 2 Node.
  - Cập nhật liên tục trạng thái check mã CRC: OK / FAIL.
  - Terminal log: Ghi log lịch sử truyền nhận CAN dạng cuộn (scrolling).
  - Xử lý triệt để lỗi SPI re-entrancy (xung đột tài nguyên SPI giữa ISR và Main loop).
- Log UART (Hercules): Gửi log của các bản tin TX qua UART3 để theo dõi trên máy tính.

## 2. Hệ thống Chẩn đoán (Diagnostics - UDS)
- Phần mềm trên máy tính (Python): BEA2026_Diag_Checker hỗ trợ giao diện UI để thực hiện các chức năng chẩn đoán thông qua UART.
- Hỗ trợ các dịch vụ UDS (Unified Diagnostic Services):
  - Read Data By Identifier (0x22).
  - Write Data By Identifier (0x2E).
  - Security Access (0x27).
- Kiểm tra và giám sát việc phản hồi các Request Diagnostic từ vi điều khiển.

## 3. Cấu trúc thư mục
- 01_Diagnostic/: Tài liệu và mã nguồn phần mềm chẩn đoán UDS (Python).
- 02_Communication/: Tài liệu giao tiếp CAN.
- 03_BEA_BasicConfiguredSourceCode/: Mã nguồn chính (C/C++) chứa code STM32 (CubeIDE).

## 4. Hướng dẫn sử dụng
1. Build toàn bộ project 03_BEA_BasicConfiguredSourceCode trong phần mềm STM32CubeIDE.
2. Nạp code (Flash) xuống Board STM32.
3. Chú ý cấu hình phần cứng:
   - LCD: PB6 (đèn nền).
   - CAN2: đã được mapping sang PB12/PB13 (RX/TX) để tránh xung đột chân đèn nền LCD.
4. Mở phần mềm Hercules trên máy tính (chọn COM tương ứng, baudrate 115200) để xem log UART TX.
5. Để sử dụng chức năng chẩn đoán, chạy phần mềm BEA2026_Diag_Checker trong thư mục 01_Diagnostic.

## 5. Thông tin tác giả
- Nhóm: BOSCH - Nhóm 11
- Github: https://github.com/yandwb
