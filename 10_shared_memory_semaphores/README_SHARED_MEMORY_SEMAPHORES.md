# Etapa 10 - Shared Memory + Semaphores (System V)

Esta etapa continúa la progresión del repositorio después de procesos, señales, pipes, message queues y threads.

## Idea central

- **Shared Memory**: varios procesos ven el mismo bloque de memoria.
- **Semaphore**: controla cuándo un proceso puede entrar a una sección crítica o cuándo debe esperar.
- **Juntos**: la memoria guarda el estado compartido y el semáforo evita carreras.

## Orden recomendado

1. `00_labs_usuario_revisados`: versiones comentadas de los laboratorios entregados por el usuario.
2. `01_shared_memory_basica`: servidor/cliente sin semáforo.
3. `02_semaphore_basico`: inicialización y exclusión mutua.
4. `03_shm_sem_dos_procesos`: primera combinación de ambos mecanismos.
5. `04_shm_sem_tres_procesos`: contador protegido con tres procesos totales.
6. `05_shm_sem_cinco_procesos`: cinco procesos totales compartiendo un contador.
7. `06_shm_sem_turnos_cinco_procesos`: cinco procesos en orden estricto usando un conjunto de semáforos.
8. `07_shm_sem_signal_inicio`: señal + shared memory + semaphore.
9. `08_shm_sem_fork_execv`: integración con fork/execv.

## Patrón que debes memorizar

```text
crear/obtener SHM -> shmat()
crear/obtener SEM -> semget()
inicializar SEM   -> semctl(... SETVAL ...)

P / wait:
    semop(... -1 ...)
    [SECCION CRITICA SOBRE SHM]
V / signal:
    semop(... +1 ...)

al final:
    shmdt()
    shmctl(... IPC_RMID ...)
    semctl(... IPC_RMID ...)
```

## Comandos de terminal

- `ipcs`: muestra todos los IPC System V visibles.
- `ipcs -m`: memoria compartida.
- `ipcs -s`: semáforos.
- `ipcs -q`: colas de mensajes.
- `ipcrm -m <shmid>`: elimina un segmento de memoria compartida.
- `ipcrm -s <semid>`: elimina un conjunto de semáforos.

Lee también `COMANDOS_IPC_Y_DEPURACION.txt`.
