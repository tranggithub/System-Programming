def extract_payload(file_path, output_path):
    try:
        with open(file_path, "rb") as file:
            data = file.read()

        # Tìm kiếm signature MZ (0x4d, 0x5a)
        mz_offset = data.find(b"\x4d\x5a")  # Signature của file PE
        if mz_offset == -1:
            print("Không tìm thấy header MZ trong file.")
            return

        print(f"Header MZ được tìm thấy tại offset: {mz_offset}")

        # Trích xuất payload từ vị trí MZ
        payload_start = mz_offset
        payload_length = 27648 # Số byte payload cần trích xuất (ví dụ)
        payload = data[payload_start:payload_start + payload_length]

        # Lưu payload vào file mới
        with open(output_path, "wb") as output_file:
            output_file.write(payload)

        print(f"Payload đã được trích xuất và lưu vào {output_path}")

        # In payload dưới dạng hex
        print("Payload (hex):")
        formatted_payload = ", ".join(f"0x{byte:02x}" for byte in payload)
        print(formatted_payload)

    except Exception as e:
        print(f"Lỗi: {e}")


# Sử dụng hàm với file input và output
file_path = "C:\\Windows\\System32\\calc.exe"       # Đường dẫn tới file PE gốc
output_path = "payload_output.bin"  # Đường dẫn để lưu payload
extract_payload(file_path, output_path)
