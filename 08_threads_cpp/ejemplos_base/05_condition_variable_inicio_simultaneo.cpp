#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void worker(int id) {
    // condition_variable::wait necesita unique_lock porque wait libera
    // temporalmente el mutex y lo vuelve a adquirir al despertar.
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [] {
        return ready;
    });

    std::cout << "thread " << id << " comenzó después de la señal global\n";
}

void go() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }

    // Despierta a todos los threads que esperan la condición.
    cv.notify_all();
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, i);
    }

    std::cout << "10 threads esperando condición...\n";
    go();

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
