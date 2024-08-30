#include "honore.h"

/**
 * _getline - Reads input from a file descriptor into a buffer.
 * @str_buff: Pointer to the buffer storing the read line.
 * @num: Pointer to the variable holding the number of bytes read.
 * @file_disc: File descriptor to read from.
 *
 * Return: The number of bytes read, or -1 on error.
 */
ssize_t _getline(char **str_buff, size_t *num, int file_disc)
{
	ssize_t num_reading;
	size_t Summ_read, buffer_size = BUFF_SIZE;

	/* Allocate memory if the buffer is NULL */
	if (*str_buff == NULL)
	{
		*str_buff = malloc(sizeof(char) * (buffer_size + 1));
		if (*str_buff == NULL)
			return (-1);
	}

	num_reading = Summ_read = 0;

	/* Read from file descriptor into the buffer */
	while ((num_reading = read(file_disc, *str_buff + Summ_read, BUFF_SIZE)) > 0)
	{
		Summ_read += num_reading;

		/* Expand buffer if necessary */
		if (Summ_read >= buffer_size)
		{
			buffer_size *= 2;
			*str_buff = _realloc(*str_buff, Summ_read, buffer_size);
			if (*str_buff == NULL)
				return (-1);
			*num = Summ_read;
		}

		/* Check for newline character */
		if (Summ_read && (*str_buff)[Summ_read - 1] == '\n')
		{
			(*str_buff)[Summ_read] = '\0';
			*num = Summ_read;
			return (Summ_read);
		}
	}

	/* Handle read errors */
	if (num_reading == -1)
	{
		safe_free(*str_buff);
		return (-1);
	}

	/* Free buffer if no bytes were read */
	if (Summ_read == 0)
		safe_free(*str_buff);

	return (Summ_read);
}

