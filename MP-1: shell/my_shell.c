#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BG_CMD 64

/* Splits the string by space and returns the array of tokens
 *
 */
char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++)
	{

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t')
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0)
			{
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else
		{
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}

void shiftLeft(char **tokens)
{
	for (int i = 0; i < MAX_NUM_TOKENS - 1; i++)
	{
		tokens[i] = tokens[i + 1];
	}
	tokens[MAX_NUM_TOKENS] = NULL;
}
pid_t foreground_pid;
pid_t shell_pid;
void sigint_handler(int signum)
{
	if (shell_pid == foreground_pid)
	{
		return;
	}
	if (kill(foreground_pid, SIGINT) == -1)
	{
		perror("kill");
	}
	printf("Shell: Interrupt signal received\n");
}
void sigint_handler_child(int signum)
{
	exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
	char line[MAX_INPUT_SIZE];
	pid_t backgroundTasks[MAX_BG_CMD];
	char **tokens;
	int status;
	int i;
	pid_t pid;
	foreground_pid = getpid();
	shell_pid = getpid();
	signal(SIGINT, sigint_handler);
	for (int i = 0; i < MAX_BG_CMD; i++)
	{
		backgroundTasks[i] = 0;
	}

	while (1)
	{
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		line[strlen(line)] = '\n';
		tokens = tokenize(line);
		for (int bg_tsk = 0; bg_tsk < MAX_BG_CMD; bg_tsk++)
		{
			pid_t pid_check = backgroundTasks[bg_tsk];
			if (pid_check == 0)
				continue;
			pid_t w = waitpid(pid_check, &status, WNOHANG);
			if (w == 0)
			{
				continue;
			}
			else
			{
				backgroundTasks[bg_tsk] = 0;
				printf("Shell: Background process %d finished\n", pid_check);
			}
		}
		if (tokens[0] == NULL)
		{
			continue;
		}
		else if (strcmp(tokens[0], "cd") == 0)
		{
			if (chdir(tokens[1]) != 0)
			{
				perror("chdir");
			}
		}
		else if (strcmp(tokens[0], "exit") == 0)
		{
			break;
		}
		// background process
		else if (strcmp(tokens[0], "&") == 0)
		{
			shiftLeft(tokens);
			printf("Job submitted successfully.\n");
			pid = fork();
			if (pid == 0)
			{
				signal(SIGINT, sigint_handler_child);
				// setting process to it's own process group
				if (setpgid(0, 0) == -1)
				{
					perror("setpgid");
					exit(EXIT_FAILURE);
				}
				sleep(10);
				execvp(tokens[0], tokens);
				printf("WARN: command not found\n");
				exit(EXIT_FAILURE);
			}
			else if (pid > 0)
			{
				for (int bg_tsk = 0; bg_tsk < MAX_BG_CMD; bg_tsk++)
				{
					if (backgroundTasks[bg_tsk] == 0)
					{
						backgroundTasks[bg_tsk] = pid;
						break;
					}
				}
			}
			else
			{
				printf("ERR: error in fork: SHELL\n");
			}
		}
		// foreground process
		else
		{
			pid = fork();
			if (pid == 0)
			{
				// handle ctrl+C signal
				signal(SIGINT, sigint_handler_child);
				// child
				foreground_pid = getpid();
				execvp(tokens[0], tokens);
				printf("WARN: command not found\n");
				exit(1);
			}
			else if (pid > 0)
			{
				// parent
				pid_t child_pid = waitpid(pid, &status, 0);
			}
			else
			{
				printf("ERR: error in fork: SHELL\n");
			}
		}
	}

	for (int p_id = 0; p_id < MAX_BG_CMD; p_id++)
	{
		pid_t pid_check = backgroundTasks[p_id];
		if (pid_check == 0)
			continue;
		if (kill(pid_check, SIGINT) == -1)
		{
			perror("kill");
		}
	}
	// Freeing the allocated memory
	for (i = 0; tokens[i] != NULL; i++)
	{
		free(tokens[i]);
	}
	free(tokens);
	return 0;
}
