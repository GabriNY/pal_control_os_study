#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

volatile sig_atomic_t enviar = 0;

void handler(int) {
    enviar = 1;
}

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(fd[1]);
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(1);
        }
        close(fd[0]);

        char* args[] = {const_cast<char*>("./detached_worker"), nullptr};
        execv(args[0], args);
        perror("execv");
        _exit(1);
    }

    close(fd[0]);

    struct sigaction sa {};
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, nullptr);

    std::printf("PID coordinador: %d\n", static_cast<int>(getpid()));
    std::printf("Esperando SIGUSR1...\n");
    std::fflush(stdout);

    while (!enviar) {
        pause();
    }

    const char mensaje[] = "5\n";
    if (write(fd[1], mensaje, std::strlen(mensaje)) == -1) {
        perror("write");
    }
    close(fd[1]);

    waitpid(pid, nullptr, 0);
    return 0;
}
