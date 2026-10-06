#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

struct Cuenta {
    explicit Cuenta(int saldo_inicial) : saldo(saldo_inicial) {}

    int saldo;
    std::mutex mtx;
};

Cuenta cuenta_a(100);
Cuenta cuenta_b(100);
std::mutex output_mtx;

void transferir(Cuenta& origen, Cuenta& destino, int cantidad, const char* nombre) {
    try {
        std::lock(origen.mtx, destino.mtx);
        std::lock_guard<std::mutex> lock_origen(origen.mtx, std::adopt_lock);
        std::lock_guard<std::mutex> lock_destino(destino.mtx, std::adopt_lock);

        if (origen.saldo < cantidad) {
            throw std::runtime_error("saldo insuficiente");
        }

        origen.saldo -= cantidad;
        destino.saldo += cantidad;

        std::lock_guard<std::mutex> out(output_mtx);
        std::cout << nombre << ": transferencia correcta\n";
    } catch (const std::runtime_error& e) {
        std::lock_guard<std::mutex> out(output_mtx);
        std::cout << nombre << ": " << e.what() << '\n';
    }
}

int main() {
    std::thread t1(transferir, std::ref(cuenta_a), std::ref(cuenta_b), 30, "A->B");
    std::thread t2(transferir, std::ref(cuenta_b), std::ref(cuenta_a), 20, "B->A");

    t1.join();
    t2.join();

    std::cout << "Saldo A=" << cuenta_a.saldo << " Saldo B=" << cuenta_b.saldo << '\n';
    return 0;
}
