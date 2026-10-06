#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

std::mutex mtx;
std::condition_variable cv;
bool inicio = false;
int contador = 0;

void worker(int id) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] {
            return inicio;
        });
    }

    try {
        {
            std::lock_guard<std::mutex> lock(mtx);
            ++contador;
            std::cout << "[thread " << id << "] contador=" << contador << '\n';
        }

        if (id % 2 != 0) {
            throw std::logic_error("id impar");
        }
    } catch (const std::logic_error& e) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[thread " << id << "] catch: " << e.what() << '\n';
    }
}

int main() {
    int n = 0;
    if (!(std::cin >> n) || n <= 0) {
        std::cerr << "N inválido\n";
        return 1;
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < n; ++i) {
        threads.emplace_back(worker, i);
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        inicio = true;
    }
    cv.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "contador final=" << contador << '\n';
    return 0;
}
