#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_COMMAND_ARGS 1024

volatile sig_atomic_t interrupted = 0;

void sigint_handler(int sig)
{
    interrupted = 1;
}
void get_commands_before_pipe(char **commands, char **cmd_pipe)
{
    int pipes_cnt = 0;
    while (strcmp(commands[pipes_cnt], "|"))
    {
        cmd_pipe[pipes_cnt] = commands[pipes_cnt];
        pipes_cnt++;
    }
    cmd_pipe[pipes_cnt] = NULL;
}
void get_commands_after_pipe(char **commands, int pipe_index)
{
    int i = 0;
    for (int t = pipe_index + 1; commands[t] != NULL; t++)
    {
        commands[i] = commands[t];
        i++;
    }
    commands[i] = NULL; // to end the array
}
int main()
{
    int i;
    char *token;
    char input[MAX_COMMAND_LENGTH];
    char *commands[MAX_COMMAND_ARGS];
    signal(SIGINT, sigint_handler);
    while (1)
    {
        printf("stshell > ");

        if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL)
        {
            if (errno == EINTR && interrupted)
            {
                printf("\n^C\n");
                interrupted = 0;
                continue;
            }
        }

        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "exit") == 0)
        {
            break;
        }

        i = 0;
        token = strtok(input, " ");
        while (token != NULL)
        {
            commands[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        commands[i] = NULL;

        if (commands[0] == NULL)
        {
            continue;
        }

        pid_t pid;
        int fd_pipe[2];
        char *cmd_pipe[MAX_COMMAND_ARGS] = {NULL};

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return 1;
        }

        if (pid == 0)
        {

            signal(SIGINT, sigint_handler);

            for (int j = 0; commands[j] != NULL; j++)
            {

                if (!strcmp(commands[j], ">"))
                {

                    char *filename = commands[j + 1];
                    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1)
                    {
                        perror("open");
                        return 1;
                    }
                    if (dup2(fd, STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        return 1;
                    }
                    close(fd);
                    commands[j] = NULL;
                    break;
                }
                else if (!strcmp(commands[j], ">>"))
                {

                    char *filename = commands[j + 1];
                    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd == -1)
                    {
                        perror("open");
                        return 1;
                    }
                    if (dup2(fd, STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        return 1;
                    }
                    close(fd);
                    commands[j] = NULL;
                    break;
                }
                else if (!strcmp(commands[j], "<"))
                {

                    char *filename = commands[j + 1];
                    int fd = open(filename, O_RDONLY);
                    if (fd == -1)
                    {
                        perror("open");
                        return 1;
                    }
                    if (dup2(fd, STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        return 1;
                    }
                    close(fd);
                    commands[j] = NULL;
                    break;
                }

                else if (!strcmp(commands[j], "|"))
                {
                    if (pipe(fd_pipe) < 0)
                    {
                        perror("pipe");
                        return 1;
                    }

                    pid_t pid2 = fork();
                    if (pid2 == -1)
                    {
                        perror("fork");
                        return 1;
                    }
                    if (pid2 == 0)
                    {

                        close(fd_pipe[0]);
                        get_commands_before_pipe(commands, cmd_pipe);

                        if (dup2(fd_pipe[1], STDOUT_FILENO) < 0)
                        {
                            perror("dup2");
                            return 1;
                        }

                        close(fd_pipe[1]);
                        if (execvp(cmd_pipe[0], cmd_pipe) < 0)
                        {
                            perror("execvp");
                            return 1;
                        }
                    }
                    else
                    {
                        get_commands_after_pipe(commands, j);
                        commands[i] = NULL;
                        close(fd_pipe[1]);

                        if (dup2(fd_pipe[0], STDIN_FILENO) < 0)
                        {
                            perror("dup2");
                            return 1;
                        }

                        close(fd_pipe[0]);
                        wait(0);
                        j = -1;
                    }
                }
            }
            if (execvp(commands[0], commands) < 0)
            {
                perror("execvp");
                return 1;
            }
        }
        wait(0);
    }
    return 0;
}
