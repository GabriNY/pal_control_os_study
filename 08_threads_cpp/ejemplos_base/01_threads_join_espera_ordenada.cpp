#include <chrono>
#include <iostream>
#include <thread>

// Cada hilo ejecuta esta función de forma concurrente.
// El parámetro n identifica al hilo y también indica cuántos segundos espera.
void pause_thread(int n) {
    std::this_thread::sleep_for(std::chrono::seconds(n));
    std::cout << "[hilo " << n << "] terminó su pausa de " << n << " s\n";
}

int main() {
    std::cout << "Creando 3 threads...\n";

    // Cada std::thread comienza a ejecutar pause_thread inmediatamente.
    std::thread t1(pause_thread, 1);
    std::thread t2(pause_thread, 2);
    std::thread t3(pause_thread, 3);

    std::cout << "Threads creados. main esperará con join().\n";

    // join() bloquea al hilo principal hasta que el hilo indicado termina.
    t1.join();
    t2.join();
    t3.join();

    std::cout << "Todos los threads finalizaron.\n";
    return 0;
}
