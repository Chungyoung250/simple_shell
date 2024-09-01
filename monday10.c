#include "honore.h"

/**
 * execute_command - Runs a command by executing the specified binary file
 * @pathname: Absolute path to the executable binary
 * @hons: Pointer to the shell context structure
 *
 * Return: 0 on success, -1 on failure
 */
int execute_command(const char *pathname, shell_t *hons)
{
	int status;      /* Variable to hold the process exit status */
	pid_t pid;       /* Process ID for the forked process */

	pid = fork();
	if (pid == -1)   /* Error handling for fork failure */
	{
		perror("fork");
		return (-1);
	}

	if (pid == 0)    /* Child process block */
	{
		if (execve(pathname, hons->other_cmd, environ) == -1)
		{
			if (errno == EACCES)  /* Check for permission error */
			{
				fprintf(stderr, "%s: %lu: %s\n", hons->prog_name,
						++hons->comm_calc, strerror(errno));
				return (126);
			}
			perror("execve");     /* Error handling for execve failure */
			return (-1);
		}
	}
	else             /* Parent process block */
	{
		if (waitpid(pid, &status, 0) == -1)  /* Wait for child process to finish */
		{
			perror("wait");
			return (-1);
		}
		if (WIFEXITED(status))  /* Check if the child process exited normally */
		{
			return (WEXITSTATUS(status));  /* Return exit status of child process */
		}
	}

	return (0);  /* Return 0 on success */
}
