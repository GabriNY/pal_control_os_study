CONTROLES SIMULADOS - SISTEMAS OPERATIVOS 2026-II
=================================================

Estos controles se han construido siguiendo la estructura del control de referencia:
- un proceso padre principal;
- jerarquía creada con fork();
- un clon que se transforma mediante execv();
- señales como eventos de activación;
- dos o más mecanismos IPC;
- obligación de explicar el diagrama de procesos y el flujo de los datos.

Orden sugerido de práctica:
1) control_01: réplica estructural del control modelo.
2) control_02: intercambia los destinos de Pipe y Message Queue.
3) control_03: reemplaza el paso del descriptor por argv con dup2 + STDIN.
4) control_04: integra Signal + FIFO + Message Queue + Pipe + fork + dup2 + execv.
5) control_05: añade confirmación/ACK y una señal generada por otro proceso.
6) control_06: enfatiza herencia de descriptores y FD_CLOEXEC.

Intenta resolver primero solo ENUNCIADO.txt. Luego compara con la carpeta solucion/.
