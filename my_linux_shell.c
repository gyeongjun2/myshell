#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX 256
#define MAX_PATH 256

void sigint_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n종료하시려면 'Ctrl+|'(QUIT)을 사용하세요.\n");
    }
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


int main() {
    char line[MAX], *pathPtr[MAX_PATH];
    char path[128][MAX];
    char buf[MAX];

    int count = 0;
    int fd;

    fd = open(".myshell", O_RDONLY);
    if (fd < 0) {
        error_handling("cant read '.myshell'");
    }

    int size = read(fd, buf, sizeof(buf) - 1);
    if (size < 0) {
        error_handling("read() error");
    }

    buf[size] = '\0';
    close(fd);

    char *ptr = strtok(buf, "\n");
    while (ptr != NULL) {
        if (strncmp(ptr, "PATH=", 5) == 0) {
            char *token = strtok(ptr + 5, ":");
            while (token != NULL && count < MAX_PATH) {
                strncpy(path[count], token, 255);
                path[count][255] = '\0';
                pathPtr[count] = path[count];
                count++;
                token = strtok(NULL, ":");
            }
        }
        ptr = strtok(NULL, "\n");
    }

    
    signal(SIGINT, sigint_handler);

    while (1) {
        printf("%% ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        // 'cd' 명령어 처리
        if (strncmp(line, "cd ", 3) == 0) {
            char *dir = line + 3;
            if (chdir(dir) != 0) {
                perror("cant change directory");
            }
            continue;
        }

        // 'gcc' 명령어 처리
        if (strncmp(line, "gcc ", 4) == 0) {
            char *argv[MAX];
            int i = 0;
            argv[i] = strtok(line, " ");
            while (argv[i] != NULL) {
                argv[++i] = strtok(NULL, " ");
            }
            argv[i] = NULL;

            int pid = fork();
            if (pid < 0) {
                error_handling("fork() error");
            }
            if (pid == 0) {
                execvp("gcc", argv);
                error_handling("execvp() error");
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
            continue;
        }

        // './a.exe' 명령어 처리
        if (strcmp(line, "./a.exe") == 0) {
            if (access("./a.exe", F_OK) != 0) {
                perror("cant find a.exe");
                continue;
            }
            int pid = fork();
            if (pid < 0) {
                error_handling("fork() error");
            }
            if (pid == 0) {
                char *argv[] = {"./a.exe", NULL};
                execv("./a.exe", argv);
                error_handling("execv() error");
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
            continue;
        }

        char *argv[MAX];
        int i = 0;
        argv[i] = strtok(line, " ");
        while (argv[i] != NULL) {
            argv[++i] = strtok(NULL, " ");
        }
        argv[i] = NULL;

        char full_path[MAX];
        int found = 0;
        for (int j = 0; j < count; j++) {
            snprintf(full_path, sizeof(full_path), "%s/%s", pathPtr[j], argv[0]);
            if (access(full_path, F_OK) == 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            int pid = fork();
            if (pid == 0) {
                execv(full_path, argv);
                error_handling("execv() error");
            } else if (pid > 0) {
                int status;
                if (wait(&status) == -1) {
                    perror("wait(): ");
                }
            } else {
                perror("fork(): ");
            }
        } else {
            printf("Command not found\n");
        }
    }

    return 0;
}
