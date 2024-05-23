#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_CMD_LEN 1024
#define MAX_PATHS 100
#define MAX_ARGS 100

// Function to read the .myshell file and set the environment variables
void load_env(char paths[MAX_PATHS][MAX_CMD_LEN], int *path_count) {
    int fd = open(".myshell", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open .myshell file");
        exit(1);
    }

    char buffer[MAX_CMD_LEN];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read .myshell file");
        close(fd);
        exit(1);
    }
    buffer[bytes_read] = '\0';

    close(fd);

    char *line = strtok(buffer, "\n");
    while (line != NULL) {
        if (strncmp(line, "PATH=", 5) == 0) {
            char *token = strtok(line + 5, ":");
            while (token != NULL && *path_count < MAX_PATHS) {
                strncpy(paths[*path_count], token, MAX_CMD_LEN - 1);
                paths[*path_count][MAX_CMD_LEN - 1] = '\0'; // Ensure null-termination
                (*path_count)++;
                token = strtok(NULL, ":");
            }
        }
        line = strtok(NULL, "\n");
    }
}

// Function to find the command in the given paths
int find_command(char *cmd, char paths[MAX_PATHS][MAX_CMD_LEN], int path_count, char *full_path) {
    for (int i = 0; i < path_count; i++) {
        snprintf(full_path, MAX_CMD_LEN, "%s/%s", paths[i], cmd);
        if (access(full_path, X_OK) == 0) {
            return 1; // Command found
        }
    }
    return 0; // Command not found
}

// Function to parse the input command into an array of arguments
void parse_command(char *cmd, char *argv[]) {
    int i = 0;
    argv[i] = strtok(cmd, " ");
    while (argv[i] != NULL) {
        argv[++i] = strtok(NULL, " ");
    }
}

int main() {
    char cmd[MAX_CMD_LEN];
    char paths[MAX_PATHS][MAX_CMD_LEN];
    int path_count = 0;

    // Load environment variables and paths from .myshell file
    load_env(paths, &path_count);

    // Ensure default paths are included if .myshell does not specify
    if (path_count == 0) {
        snprintf(paths[path_count++], MAX_CMD_LEN, "/bin");
        snprintf(paths[path_count++], MAX_CMD_LEN, "/usr/bin");
        snprintf(paths[path_count++], MAX_CMD_LEN, "/usr/local/bin");
    }

    while (1) {
        // Print shell prompt
        printf("myshell> ");
        fflush(stdout);

        // Read user input
        if (!fgets(cmd, sizeof(cmd), stdin)) {
            break;  // Exit on EOF (Ctrl+D)
        }

        // Remove newline character from the command
        cmd[strcspn(cmd, "\n")] = '\0';

        // Ignore empty commands
        if (cmd[0] == '\0') {
            continue;
        }

        // Parse the command into arguments
        char *argv[MAX_ARGS];
        parse_command(cmd, argv);

        // Check if the command exists in the specified paths
        char full_path[MAX_CMD_LEN];
        if (find_command(argv[0], paths, path_count, full_path)) {
            // Fork a child process
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                exit(1);
            }

            if (pid == 0) {
                // Child process: Execute the command
                execv(full_path, argv);
                // If execv returns, it must have failed
                perror("execv failed");
                exit(1);
            } else {
                // Parent process: Wait for the child to terminate
                int status;
                waitpid(pid, &status, 0);
            }
        } else {
            // Command not found in the specified paths
            printf("%s: command not found\n", argv[0]);
        }
    }

    return 0;
}

