#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

std::mutex mtx;
std::condition_variable cv_finalizados;
int finalizados = 0;
constexpr int TOTAL = 8;

void worker(int id) {
    try {
        if (id % 3 == 0) {
            throw std::logic_error("id múltiplo de 3");
        }

        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[worker " << id << "] trabajo correcto\n";
    } catch (const std::logic_error& e) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[worker " << id << "] catch: " << e.what() << '\n';
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        ++finalizados;
    }
    cv_finalizados.notify_one();
}

int main() {
    for (int i = 0; i < TOTAL; ++i) {
        std::thread(worker, i).detach();
    }

    std::unique_lock<std::mutex> lock(mtx);
    cv_finalizados.wait(lock, [] {
        return finalizados == TOTAL;
    });

    std::cout << "main: los " << finalizados << " detached workers terminaron\n";
    return 0;
}
