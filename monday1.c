#include "honore.h"

/**
 * pass_line - Processes a command line and handles it before execution.
 * @hons: Struct containing relevant shell data.
 *
 * Return: The exit code after processing.
 */
int pass_line(shell_t *hons)
{
	size_t start;

	/* Check for empty line or comments */
	if (*hons->linne == '\n' || *hons->linne == '#')
		return (0);

	/* Handle comments within the line */
	hons->linne = handle_comments(hons->linne);

	/* Tokenize the line based on newlines */
	hons->credential = _strtok(hons->linne, "\n");
	if (hons->credential == NULL)
	{
		fprintf(stderr, "Not enough system memory to continue\n");
		return (-1);
	}

	/* Iterate through each token */
	for (start = 0; hons->credential[start] != NULL; start++)
	{
		hons->token = hons->credential[start];

		/* Handle 'exit' command */
		if (!_strcmp(hons->credential[start], "exit") && 
			hons->credential[start + 1] == NULL)
		{
			handle_exit(hons, multi_free);
		}

		/* Tokenize based on semicolons or newlines */
		hons->commands = _strtok(hons->token, ";\n");
		if (hons->commands == NULL)
		{
			fprintf(stderr, "Memory allocation failed...\n");
			return (-1);
		}

		/* Execute commands */
		hons->terminate_code = pulse(hons);
		free_str(&hons->commands);
	}

	free_str(&hons->credential);
	return (hons->terminate_code);
}

/**
 * pulse - Processes and executes commands, accepting new prompts if necessary.
 * @hons: Struct containing relevant shell data.
 *
 * Return: The exit code.
 */
int pulse(shell_t *hons)
{
	ssize_t i, offset;
	char *cur_cmd = NULL, *operator = NULL;
	char *next_cmd = NULL, *temp_next_cmd = NULL;

	/* Iterate through each command */
	for (i = 0; hons->commands[i] != NULL; i++)
	{
		operator = get_operator(hons->commands[i]);

		/* Handle logical operators */
		if (operator != NULL)
		{
			offset = strcspn(hons->commands[i], operator);
			cur_cmd = strndup(hons->commands[i], offset);
			if (cur_cmd == NULL)
				return (0);
			hons->other_cmd = _strtok(cur_cmd, NULL);
			safe_free(cur_cmd);
			if (hons->other_cmd == NULL)
				return (0);
			hons->other_cmd = handle_variables(hons);
			pulse_helper(hons, i);

			temp_next_cmd = _strdup(&hons->commands[i][offset + 2]);
			safe_free(next_cmd);
			safe_free(hons->commands[i]);

			/* Determine whether to execute the next command based on the operator */
			if ((!_strcmp(operator, "&&") && hons->terminate_code == 0) ||
				(!_strcmp(operator, "||") && hons->terminate_code != 0))
			{
				hons->commands[i] = temp_next_cmd;
				pulse(hons);
				next_cmd = temp_next_cmd;
			}
			else
				safe_free(temp_next_cmd);
		}
		else
			pulse_and_tambuka(hons, i);
	}
	return (hons->terminate_code);
}

/**
 * pulse_and_tambuka - Processes each line individually and executes the command.
 * @hons: Struct containing relevant shell data.
 * @index: The current index of the command.
 *
 * Return: The exit code.
 */
int pulse_and_tambuka(shell_t *hons, size_t index)
{
	/* Tokenize the current command */
	hons->other_cmd = _strtok(hons->commands[index], NULL);
	if (hons->other_cmd == NULL)
	{
		return (0);
	}

	/* Handle variable substitution within the command */
	hons->other_cmd = handle_variables(hons);
	if (hons->other_cmd[0] != NULL && hons->other_cmd != NULL)
		pulse_helper(hons, index);
	else
		free_str(&hons->other_cmd);

	safe_free(hons->commands[index]);
	return (hons->terminate_code);
}

/**
 * pulse_helper - Provides additional parsing and command execution.
 * @hons: Struct containing relevant shell data.
 * @index: The current index of the command.
 */
void pulse_helper(shell_t *hons, size_t index)
{
	char *alias_value;

	/* Handle alias commands */
	if (!_strcmp(hons->other_cmd[0], "alias") ||
			!_strcmp(hons->other_cmd[0], "unalias"))
	{
		hons->terminate_code = handle_alias(&hons->aliyases, hons->commands[index]);
		free_str(&hons->other_cmd);
		return;
	}

	/* Retrieve and process alias values */
	alias_value = get_alias(hons->aliyases, hons->other_cmd[0]);
	if (alias_value != NULL)
	{
		build_alias_cmd(&hons->other_cmd, alias_value);
		safe_free(alias_value);
	}

	/* Handle built-in commands */
	hons->terminate_code = handle_builtin(hons);
	if (hons->terminate_code != NOT_BUILTIN)
	{
		free_str(&hons->other_cmd);
		return;
	}

	/* Handle commands in the PATH or direct execution */
	if (hons->path_list != NULL && !_strchr(hons->other_cmd[0], '/'))
	{
		hons->terminate_code = handle_with_path(hons);
		if (hons->terminate_code == -1)
			hons->terminate_code = print_cmd_not_found(hons);
	}
	else
	{
		if (access(hons->other_cmd[0], X_OK) == 0 &&
				_strchr(hons->other_cmd[0], '/'))
			hons->terminate_code = execute_command(hons->other_cmd[0], hons);
		else
			hons->terminate_code = print_cmd_not_found(hons);
	}
	free_str(&hons->other_cmd);
}

/**
 * print_cmd_not_found - Prints an error message when a command is not found.
 * @hons: Struct containing relevant shell data.
 *
 * Return: Command not found error code.
 */
int print_cmd_not_found(shell_t *hons)
{
	dprintf(STDERR_FILENO, "%s: %lu: %s: not found\n", hons->prog_name,
			hons->comm_calc, hons->other_cmd[0]);

	return (CMD_NOT_FOUND);
}

