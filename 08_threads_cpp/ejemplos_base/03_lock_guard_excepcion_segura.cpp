#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

std::mutex mtx;

void print_even(int value) {
    if (value % 2 == 0) {
        std::cout << value << " es par\n";
        return;
    }

    throw std::logic_error("el valor no es par");
}

void worker(int id) {
    try {
        // lock_guard aplica RAII:
        // bloquea mtx al construirse y lo libera automáticamente al salir del bloque,
        // incluso si print_even() lanza una excepción.
        std::lock_guard<std::mutex> lock(mtx);
        print_even(id);
    } catch (const std::logic_error& e) {
        // El mutex ya fue liberado al destruirse lock_guard durante el desenrollado.
        std::lock_guard<std::mutex> output_lock(mtx);
        std::cout << "[thread " << id << "] excepción capturada: " << e.what() << '\n';
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
