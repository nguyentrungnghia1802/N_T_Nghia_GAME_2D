#include <iostream>
#include <fstream>

int main() {
    std::ifstream inputFile("input.txt"); // Thay "input.txt" bằng đường dẫn đến file cần xử lý
    std::ofstream outputFile("map.txt"); // Thay "output.txt" bằng tên file kết quả đầu ra

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    char ch;
    while (inputFile.get(ch)) {
        if (ch == ',') {
            outputFile << ' '; // Thay dấu phẩy bằng dấu cách
        } else {
            outputFile << ch;
        }
    }

    inputFile.close();
    outputFile.close();

    std::cout << "Finish!\n";

    return 0;
}

