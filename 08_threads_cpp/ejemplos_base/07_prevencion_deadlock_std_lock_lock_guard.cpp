#include <iostream>
#include <mutex>
#include <thread>

std::mutex mutex_a;
std::mutex mutex_b;
int recurso_a = 100;
int recurso_b = 200;

void transferencia_a_b(int cantidad) {
    // std::lock adquiere ambos mutex evitando que este thread quede con uno
    // mientras espera indefinidamente por el otro.
    std::lock(mutex_a, mutex_b);

    // adopt_lock indica que los mutex YA fueron adquiridos por std::lock.
    // lock_guard se encargará de liberarlos automáticamente al salir del bloque.
    std::lock_guard<std::mutex> lock_a(mutex_a, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(mutex_b, std::adopt_lock);

    recurso_a -= cantidad;
    recurso_b += cantidad;
}

void transferencia_b_a(int cantidad) {
    // Aunque la operación lógica vaya de B hacia A, adquirimos los dos mutex
    // con std::lock en una única operación coordinada para evitar deadlock.
    std::lock(mutex_a, mutex_b);
    std::lock_guard<std::mutex> lock_a(mutex_a, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(mutex_b, std::adopt_lock);

    recurso_b -= cantidad;
    recurso_a += cantidad;
}

int main() {
    std::thread t1(transferencia_a_b, 10);
    std::thread t2(transferencia_b_a, 20);

    t1.join();
    t2.join();

    std::cout << "recurso_a=" << recurso_a << '\n';
    std::cout << "recurso_b=" << recurso_b << '\n';
    return 0;
}
