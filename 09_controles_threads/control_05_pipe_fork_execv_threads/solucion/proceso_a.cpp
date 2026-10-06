#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

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

        char* args[] = {const_cast<char*>("./worker_threads"), nullptr};
        execv(args[0], args);
        perror("execv");
        _exit(1);
    }

    close(fd[0]);
    const char mensaje[] = "6\n";
    if (write(fd[1], mensaje, std::strlen(mensaje)) == -1) {
        perror("write");
    }
    close(fd[1]);

    waitpid(pid, nullptr, 0);
    return 0;
}
