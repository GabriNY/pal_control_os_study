#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

std::mutex mtx;

void validar(int value) {
    if (value % 2 != 0) {
        throw std::runtime_error("solo se aceptan pares");
    }
    std::cout << value << " aceptado\n";
}

void worker(int id) {
    try {
        std::lock_guard<std::mutex> lock(mtx);
        validar(id);
    } catch (const std::runtime_error& e) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << id << " rechazado: " << e.what() << '\n';
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 1; i <= 10; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
