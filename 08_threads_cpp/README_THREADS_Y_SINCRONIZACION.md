# Threads y sincronización - material de control

Los archivos originales p1.c ... p6.c usan características de C++, por lo que se renombran a `.cpp`.

## Correspondencia

- p1.c -> `01_threads_join_espera_ordenada.cpp`
- p2.c -> `02_mutex_manual_try_catch_throw.cpp`
- p3.c -> `03_lock_guard_excepcion_segura.cpp`
- p4.c -> `04_threads_detach_ciclo_vida.cpp`
- p5.c -> `05_condition_variable_inicio_simultaneo.cpp`
- p6.c -> `06_condition_variable_turnos_ordenados.cpp`

## Conceptos

- `join()`: el hilo llamador espera la terminación de otro hilo.
- `detach()`: el thread continúa independiente; ya no puede hacerse `join()` sobre ese objeto.
- `std::mutex`: exclusión mutua de una sección crítica.
- `lock()/unlock()`: control manual del mutex; exige liberar el mutex en todos los caminos.
- `try/catch`: manejo de excepciones C++.
- `throw`: genera una excepción.
- `std::lock_guard`: RAII; libera el mutex automáticamente al abandonar el bloque, incluso por excepción.
- `std::condition_variable`: permite dormir threads hasta que cambie una condición compartida.
- `std::unique_lock`: lock flexible requerido por `condition_variable::wait`.

### Sobre deadlock y lock_guard

`lock_guard` evita el error frecuente de olvidar `unlock()` cuando hay un único mutex y también es seguro ante excepciones. No significa que cualquier programa con varios mutex quede automáticamente libre de deadlock; para varios locks deben respetarse órdenes consistentes o usarse mecanismos como `std::lock`/`std::scoped_lock`.

## Ejemplo adicional: prevención de deadlock

`07_prevencion_deadlock_std_lock_lock_guard.cpp` usa `std::lock` para adquirir dos mutex de forma coordinada y dos `lock_guard` con `std::adopt_lock` para aplicar RAII a ambos locks.
