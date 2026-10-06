#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> cola;
bool fin = false;

void productor() {
    for (int value = 1; value <= 5; ++value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            cola.push(value);
            std::cout << "[productor] -> " << value << '\n';
        }
        cv.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        fin = true;
    }
    cv.notify_one();
}

void consumidor() {
    try {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] {
                return !cola.empty() || fin;
            });

            if (cola.empty() && fin) {
                break;
            }

            int value = cola.front();
            cola.pop();
            lock.unlock();

            if (value < 0) {
                throw std::runtime_error("dato negativo");
            }

            std::cout << "[consumidor] <- " << value << '\n';
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "[consumidor] excepción: " << e.what() << '\n';
    }
}

int main() {
    std::thread p(productor);
    std::thread c(consumidor);

    p.join();
    c.join();
    return 0;
}
