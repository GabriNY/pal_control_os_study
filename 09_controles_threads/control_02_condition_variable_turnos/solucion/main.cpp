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
    cv.wait(lock, [id] {
        return turno == id;
    });

    std::cout << "Proceso " << id << '\n';
    ++turno;

    lock.unlock();
    cv.notify_all();
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 6; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
