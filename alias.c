#include "honore.h"

/**
 * add_alias - Adds or updates an alias in the linked list.
 * @aliases: Pointer to the list containing aliases.
 * @name: Name of the alias to add or update.
 * @value: Value associated with the alias name.
 *
 * Return: Address of the newly added alias node,
 * or the address of the existing alias if it was updated.
 */
alias_t *add_alias(alias_t **aliases, const char *name, const char *value)
{
	alias_t *current, *alias;

	alias = malloc(sizeof(alias_t));
	if (alias == NULL)
		return (NULL);

	alias->name = _strdup(name);
	alias->value = _strdup(value);
	alias->next = NULL;

	if (*aliases == NULL)
	{
		(*aliases) = alias;
		return (*aliases);
	}

	current = *aliases;
	while (current != NULL)
	{
		if (!_strcmp(name, current->name))
		{
			safe_free(current->value);
			current->value = _strdup(value);
			safe_free(alias->name);
			safe_free(alias->value);
			safe_free(alias);

			return (current);
		}
		current = current->next;
	}

	current = *aliases;
	while (current->next != NULL)
	{
		current = current->next;
	}
	current->next = alias;

	return (alias);
}

/**
 * print_aliases - Prints all aliases in the list.
 * @aliases: Pointer to the list of aliases.
 */
void print_aliases(const alias_t *aliases)
{
	if (aliases == NULL)
		return;

	while (aliases != NULL)
	{
		printf("%s='%s'\n", aliases->name, aliases->value);
		aliases = aliases->next;
	}
}

/**
 * unalias - Removes specified aliases from the list.
 * @aliases: Pointer to the list of aliases.
 * @command: Null-terminated array of alias names to remove.
 *
 * Return: 0 if an alias was successfully removed, otherwise 1.
 */
int unalias(alias_t **aliases, char *command)
{
	alias_t *current, *prev;
	size_t i;
	char **names;
	int exit_code = 1; /* Assume failure by default */

	names = _strtok(command, NULL);
	if (names == NULL)
		return (-1);

	current = *aliases;
	for (i = 1; names[i] != NULL; i++)
	{
		while (current != NULL)
		{
			if (!_strcmp(current->name, names[i]))
			{
				if (current == *aliases)
					*aliases = (*aliases)->next;
				else
					prev->next = current->next;
				multi_free("ss", current->name, current->value);
				safe_free(current);
				exit_code = 0;
				break;
			}
			prev = current;
			current = current->next;
		}
		current = *aliases; /* Reset pointer to the start of the list */

		if (exit_code != 0)
		{
			fprintf(stderr, "unalias: %s not found\n", names[i]);
			exit_code = 1;
		}
		safe_free(names[i]);
	}
	free_str(&names);
	return (exit_code);
}

/**
 * print_alias - Prints the alias with the specified name.
 * @aliases: Pointer to the list of aliases.
 * @name: Name of the alias to print.
 *
 * Return: 0 if the alias was found, otherwise 1.
 */
int print_alias(const alias_t *aliases, const char *name)
{
	if (aliases == NULL)
	{
		fprintf(stderr, "alias: %s not found\n", name);
		return (1); /* List is empty, can't search */
	}

	while (aliases != NULL)
	{
		if (!_strcmp(aliases->name, name))
		{
			printf("%s='%s'\n", aliases->name, aliases->value);
			return (0);
		}
		aliases = aliases->next;
	}

	/* Reached the end of the list and didn't find the alias */
	fprintf(stderr, "alias: %s not found\n", name);
	return (1);
}

/**
 * get_alias - Retrieves the value of the specified alias.
 * @aliases: Pointer to the list of aliases.
 * @name: Name of the alias to retrieve.
 *
 * Return: Value of the alias if found, otherwise NULL.
 */
char *get_alias(alias_t *aliases, const char *name)
{
	char *value, *chained_value;
	alias_t *current;

	if (aliases == NULL || name == NULL)
		return (NULL); /* List is empty or name is invalid */

	current = aliases;

	while (current != NULL)
	{
		if (!_strcmp(current->name, name))
		{
			value = _strdup(current->value);
			if (value == NULL)
				return (NULL);
			if (!_strcmp(current->name, current->value))
				return (value);

			/* Check for aliases chained to other aliases */
			chained_value = get_alias(aliases, value);
			if (chained_value != NULL)
			{
				safe_free(value);
				return (chained_value);
			}

			return (value); /* Not a chained value, return as is */
		}
		current = current->next;
	}

	return (NULL);
}

