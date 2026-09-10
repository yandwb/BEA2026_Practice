# BEA2026 Practice - CAN Communication & LCD Dashboard

Đây là bài thực hành giao tiếp CAN trên vi điều khiển STM32F4 (Board OpenX05R-C), bao gồm chức năng điều khiển màn hình LCD ILI9341 qua giao thức SPI.

## 🌟 Chức năng chính
- **Giao tiếp CAN (Node 1 & Node 2)**: 
  - Node 2 truyền bản tin `0x0A2` theo chu kỳ 20ms chứa Value0 (tăng liên tục) và Value1.
  - Node 1 nhận `0x0A2`, tính tổng `Value0 + Value1`, sau đó truyền lại bản tin `0x012` mỗi 50ms kèm mã lỗi CRC.
  - Node 2 nhận `0x012`, kiểm tra mã CRC (theo chuẩn SAE J1850) và phản hồi bằng cách chớp nháy đèn LED.
- **Màn hình LCD Dashboard (ILI9341)**:
  - Hiển thị theo thời gian thực (Real-time) dữ liệu `RX` và `TX` của cả 2 Node.
  - Cập nhật liên tục trạng thái check mã `CRC: OK / FAIL`.
  - Terminal log: Ghi log lịch sử truyền nhận CAN dạng cuộn (scrolling) giống terminal.
  - Xử lý triệt để lỗi SPI re-entrancy (xung đột tài nguyên SPI giữa ISR và Main loop).
- **Log UART (Hercules)**: Gửi log của các bản tin TX qua UART3 để theo dõi trên máy tính.

## 🛠 Cấu trúc thư mục
- `01_Diagnostic/`: Tài liệu và mã nguồn cho chức năng chẩn đoán.
- `02_Communication/`: Tài liệu giao tiếp CAN.
- `03_BEA_BasicConfiguredSourceCode/`: Mã nguồn chính (C/C++) chứa code STM32 (CubeIDE).

## 🚀 Hướng dẫn sử dụng
1. Build toàn bộ project trong **STM32CubeIDE**.
2. Nạp code (Flash) xuống Board STM32.
3. Chú ý cấu hình phần cứng:
   - LCD: `PB6` (đèn nền).
   - CAN2: đã được mapping sang `PB12/PB13` (RX/TX) để tránh xung đột chân đèn nền LCD.
4. Mở phần mềm Hercules trên máy tính (chọn COM tương ứng, baudrate 115200) để xem log UART TX.

## 📝 Thông tin tác giả
- **Nhóm**: BOSCH - Nhóm 11
- **Github**: [@yandwb](https://github.com/yandwb)
