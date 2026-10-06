#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mtx;
std::condition_variable cv;
int turno = 0;

void worker(int id) {
    std::unique_lock<std::mutex> lock(mtx);

    // El predicado evita continuar por un despertar espurio.
    // Solo el thread cuyo id coincide con turno puede avanzar.
    cv.wait(lock, [id] {
        return id == turno;
    });

    std::cout << "thread " << id << '\n';

    ++turno;

    // Despertamos a todos para que el siguiente id vuelva a evaluar el predicado.
    cv.notify_all();
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
