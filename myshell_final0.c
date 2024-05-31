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
	if(sig == SIGINT){
		printf("\n종료하시려면 'Ctrl+|'(QUIT)을 사용하세요.\n");	
	}
}

int main() {
    char line[MAX], *pathPtr[MAX_PATH];
    char path[100][MAX];
    char buf[MAX];

    int count = 0;
    int fd;

    // .myshell 파일 읽고 pathPtr 설정
    fd = open(".myshell", O_RDONLY);
    if (fd < 0) {
        perror(".myshell을 읽을 수 없습니다.");
        exit(1);
    }

    int size = read(fd, buf, sizeof(buf) - 1);
    if (size < 0) {
        perror("읽기 실패");
        close(fd);
        exit(1);
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


    // Ctrl+C 무시
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
                perror("디렉토리를 변경할 수 없습니다");
            }
            continue;
        }

        // 'gcc' 명령어 처리
        if (strncmp(line, "gcc ", 4) == 0) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork 실패");
                exit(1);
            }
            if (pid == 0) {
                char *argv[256];
                int i = 0;
                argv[i] = strtok(line, " ");
                while (argv[i] != NULL) {
                    argv[++i] = strtok(NULL, " ");
                }
                execvp("gcc", argv);
                perror("gcc 실행 실패");
                exit(1);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
            continue;
        }

        // './a.out' 명령어 처리
        if (strcmp(line, "./a.out") == 0) {
            if (access("./a.out", X_OK) != 0) {
                perror("a.out 실행 파일을 찾을 수 없습니다!!!");
                continue;
            }
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork 실패");
                exit(1);
            }
            if (pid == 0) {
                char *argv[] = {"./a.out", NULL};
                execv("./a.out", argv);
                perror("./a.out 실행 실패");
                exit(1);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
            continue;
        }

        char *argv[256];
        int i = 0;
        argv[i] = strtok(line, " ");
        while (argv[i] != NULL) {
            argv[++i] = strtok(NULL, " ");
        }

        char full_path[256];
        int found = 0;
        for (int j = 0; j < count; j++) {
            snprintf(full_path, sizeof(full_path), "%s/%s", pathPtr[j], argv[0]);
            if (access(full_path, X_OK) == 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            int pid = fork();
            if (pid == 0) {
                execv(full_path, argv);
                perror("execv 실패");
                exit(1);
            } else if (pid > 0) {
                int status;
                if (wait(&status) == -1) {
                    perror("wait 실패");
                }
            } else {
                perror("fork 실패");
            }
        } else {
            printf("Command not found\n");
        }
    }

    return 0;
}