# Simulacro de control práctico

## Cómo usar esto

Intenta cada enunciado sin mirar las carpetas. Si te trabas, identifica qué bloques necesitas y recién después revisa el ejemplo asociado.

## Ejercicio 1: Signal + FIFO

Un programa debe esperar SIGUSR1. Al recibirla, abre /tmp/ex1, lee una línea y la imprime. Debe seguir activo para otra señal.

Referencia después de intentarlo: `02_pairs/02_signal_fifo`.

## Ejercicio 2: Signal + MQ

SIGINT debe enviar type 1 y SIGUSR1 type 2 a una cola. Un receptor imprime ambos.

Referencia después de intentarlo: `06_lab_style`.

## Ejercicio 3: Pipe + fork

El padre pregunta un texto con fgets y lo manda al hijo por pipe. El hijo convierte el texto a mayúsculas.

Referencia después de intentarlo: `02_pairs/16_pipe_fork`.

## Ejercicio 4: Pipe + dup2

Conecta un pipe a STDIN y usa fgets(stdin) en vez de read(fd[0]).

Referencia después de intentarlo: `02_pairs/17_pipe_dup2`.

## Ejercicio 5: fork + execv

El hijo debe ejecutar otro programa pasando dos argumentos; el padre espera.

Referencia después de intentarlo: `02_pairs/20_fork_execv`.

## Ejercicio 6: dup2 + execv

Redirige stdout a resultado.txt y después ejecuta /bin/echo.

Referencia después de intentarlo: `02_pairs/21_dup2_execv`.

## Ejercicio 7: FIFO + dup2 + execv

El programa abre un FIFO, lo convierte en stdin y ejecuta un lector externo.

Referencia después de intentarlo: `03_triples/07_fifo_dup2_execv`.

## Ejercicio 8: Signal + fork + execv

SIGUSR1 debe provocar la creación de un hijo que ejecuta otro programa.

Referencia después de intentarlo: `03_triples/03_signal_fork_execv`.

## Ejercicio 9: Signal + pipe + fork + dup2

El padre solo escribe al pipe después de una señal; el hijo lee mediante stdin.

Referencia después de intentarlo: `04_quads/02_signal_pipe_fork_dup2`.

## Ejercicio 10: Pipe + fork + dup2 + execv

Reconstruye desde cero el patrón programa1/programa2 de tu ejemplo.

Referencia después de intentarlo: `04_quads/03_pipe_fork_dup2_execv`.

## Ejercicio 11: FIFO + MQ

Lee texto desde FIFO y reenvíalo como message type 5.

Referencia después de intentarlo: `02_pairs/07_msgq_fifo`.

## Ejercicio 12: MQ + fork + execv

El hijo espera un mensaje y, al recibirlo, ejecuta un programa pasando el texto por argv.

Referencia después de intentarlo: `03_triples/08_msgq_fork_execv`.

## Ejercicio 13: Descriptor heredado

En vez de dup2, pasa fd[0] como argv y verifica FD_CLOEXEC con fcntl.

Referencia después de intentarlo: `02_pairs/18_pipe_execv`.

## Ejercicio 14: Cadena de laboratorio

Signal 2 lee FIFO; Signal 10 envía el último texto por MQ type 3. Un proceso intermedio lo reenvía type 4.

Referencia después de intentarlo: `03_triples/01_signal_fifo_msgq`.

## Ejercicio 15: Todos

Signal -> FIFO -> pipe -> fork -> dup2 -> execv -> MQ -> receptor.

Referencia después de intentarlo: `05_all_topics`.

## Variaciones rápidas que puede pedir el profesor

- Cambiar SIGUSR1 por SIGINT.
- Cambiar message type 3 por 4.
- Hacer que el hijo responda al padre por un segundo pipe.
- Crear dos hijos, uno lector y otro consumidor de MQ.
- Sustituir read(fd,...) por dup2(fd,0)+fgets(stdin,...).
- Pasar el descriptor por argv en vez de dup2.
- Activar FD_CLOEXEC y explicar por qué el descriptor deja de funcionar después de execv.
- Añadir unlink() del FIFO o msgctl(..., IPC_RMID, ...).
- Ejecutar un programa distinto según la señal recibida.
- Añadir mensajes de debugging con PID, PPID y números de descriptor.
