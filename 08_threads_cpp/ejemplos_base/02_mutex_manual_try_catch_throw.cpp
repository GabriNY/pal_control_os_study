#include <chrono>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

std::mutex mtx;

// Demuestra el uso MANUAL de mutex junto con excepciones.
// Es importante liberar el mutex también cuando ocurre una excepción.
void worker(int id, int segundos) {
    std::this_thread::sleep_for(std::chrono::seconds(segundos));

    mtx.lock();
    try {
        std::cout << "[thread " << id << "] entró a la sección crítica\n";

        // Se genera intencionalmente un error lógico para practicar throw/catch.
        if (id % 2 != 0) {
            throw std::logic_error("id impar: error de ejemplo");
        }

        std::cout << "[thread " << id << "] operación correcta\n";
        mtx.unlock();
    } catch (const std::logic_error& e) {
        std::cout << "[thread " << id << "] excepción capturada: " << e.what() << '\n';

        // Con lock() manual debemos recordar desbloquear también en el catch.
        // Si se omite, otros threads podrían quedarse bloqueados para siempre.
        mtx.unlock();
    }
}

int main() {
    std::thread t1(worker, 1, 1);
    std::thread t2(worker, 2, 1);
    std::thread t3(worker, 3, 1);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
