#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
int finalizados = 0;
int total = 0;

void worker(int id) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[detached " << id << "] trabajo terminado\n";
        ++finalizados;
    }
    cv.notify_one();
}

int main() {
    if (!(std::cin >> total) || total <= 0) {
        std::cerr << "N inválido\n";
        return 1;
    }

    for (int i = 0; i < total; ++i) {
        std::thread(worker, i).detach();
    }

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] {
        return finalizados == total;
    });

    std::cout << "Todos los detached workers finalizaron: " << finalizados << '\n';
    return 0;
}
