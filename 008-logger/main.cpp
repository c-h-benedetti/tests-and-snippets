#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdio>
#include <filesystem>
#include <chrono>

std::string currentDateTime() {
    const int BUFFER_SIZE = 20; // YYYY-MM-DD_HH-MM-SS\0
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);

    char buffer[BUFFER_SIZE];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d_%02d-%02d-%02d",
        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);

    return std::string(buffer);
}

enum class Error : int {
    OK,
    ERROR_TYPE_1,
    ERROR_TYPE_2,
    ERROR_TYPE_3,
    ERROR_TYPE_4
};

class LogManager {

    std::vector<std::pair<Error, std::string>> errors;
    const size_t MAX_BUFFER_SIZE = 100;
    std::ofstream logFile;

private:

    void dumpToStream(std::ostream& o) const {
        for (const auto& [error, message] : errors) {
            o << message << " (" << static_cast<int>(error) << ")" << std::endl;
        }
    }

public:

    inline std::pair<Error, std::string> last_error() const { return this->errors.back(); }

    LogManager(const std::string& filename = "") {
        std::filesystem::path logPath;

        if (filename.size() == 0) { logPath = "./logs" / std::filesystem::path("logs_" + currentDateTime() + ".txt"); }
        else                      { logPath = std::filesystem::path(filename); }

        logFile.open(logPath, std::ios::out);

        if (!logFile.is_open()) {
            std::cerr << "Erreur lors de l'ouverture du fichier de log." << std::endl;
        }
    }

    ~LogManager() {
        writeToFile();
        logFile.close();
    }

    void set_error(Error err, std::string msg = "") {
        msg = "[" + currentDateTime() + "] - " + msg;
        errors.push_back({err, msg});
        if (errors.size() >= MAX_BUFFER_SIZE) {
            writeToFile();
        }
    }

    void writeToFile() {
        if (!logFile.is_open()) return;
        this->dumpToStream(logFile);
        errors.clear();
    }

    inline void printErrors() const { this->dumpToStream(std::cerr); }
};

int main() {

    LogManager log;
    constexpr size_t nLogs = 20000;

    std::string messages[5] = {
        "Ca va",
        "Message de petite taille",
        "Ligne de taille un peu plus importante mais toujours assez rapide à écrire dans un fichier. Ca ne devrait pas être dramatique.",
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi convallis tincidunt nunc a condimentum. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Donec vestibulum fringilla purus vitae accumsan. In eu aliquam nisl. In porttitor sed nisl sit amet fringilla.",
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi convallis tincidunt nunc a condimentum. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Donec vestibulum fringilla purus vitae accumsan. In eu aliquam nisl. In porttitor sed nisl sit amet fringilla. Pellentesque molestie cursus eros vel ultrices. Vivamus posuere in ipsum at porttitor. Curabitur erat risus, dictum at massa quis, scelerisque volutpat purus. Nulla suscipit tortor sit amet tempor interdum. Praesent fringilla blandit rhoncus. Nunc aliquam quis orci non imperdiet. Nunc imperdiet diam nulla, nec hendrerit nibh pretium quis. Fusce ullamcorper nulla vel nibh gravida, vel semper enim vehicula. Sed sit amet pharetra dolor, et volutpat urna. Nullam ullamcorper sapien commodo mattis pharetra. Maecenas congue sit amet risus ac imperdiet."
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0 ; i < nLogs ; i++) {
        log.set_error(static_cast<Error>(i%5), messages[i%5]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Total: " << duration.count() << " µs" << std::endl;
    std::cout << ">>> " << (duration.count() / (double)nLogs) << " line/µs" << std::endl;

    return 0;
}
