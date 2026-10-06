#include <chrono>
#include <iostream>
#include <thread>

void worker_detached(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200 + id * 100));
    std::cout << "[detached " << id << "] terminó\n";
}

int main() {
    std::cout << "Creando 5 threads detached...\n";

    for (int i = 0; i < 5; ++i) {
        // detach() separa el thread del objeto std::thread.
        // Después de detach() ya no podemos hacer join() sobre ese objeto.
        std::thread(worker_detached, i).detach();
    }

    std::cout << "main continúa sin hacer join().\n";

    // Esta espera solo se usa para demostrar que los threads detached necesitan
    // que el proceso siga vivo. No es una sincronización robusta para producción.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "main termina.\n";
    return 0;
}
