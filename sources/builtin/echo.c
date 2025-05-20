
#include "minishell.h"
/**
 * @file echo.c
 * @brief Implementation of the echo built-in command
 * 
 * This file contains functions to implement the echo built-in command
 * for the minishell project. It supports the '-n' option to suppress
 * the trailing newline.
 */

/**
 * @brief Prints arguments with spaces between them
 * 
 * Prints all arguments from the array starting at index i, with a space
 * between each argument. If option is 0, appends a newline character
 * at the end of the output.
 * 
 * @param args Array of strings to print
 * @param option Flag to control newline output (1: no newline, 0: with newline)
 * @param i Starting index in args array
 */

/**
 * @brief Processes echo command options
 * 
 * Checks for the presence of the '-n' option in the echo command arguments.
 * Returns the index of the first non-option argument.
 * 
 * @param cmd Structure containing command information
 * @return Index of the first non-option argument in the args array
 */

/**
 * @brief Implements the echo built-in command
 * 
 * Executes the echo command with support for the '-n' option, which suppresses
 * the trailing newline. Creates a child process to handle the command execution
 * and properly redirects input/output as needed.
 * 
 * @param cmd Structure containing command information including arguments
 * @param envl Pointer to the environment variables list (unused in this function)
 * @return SUCCESS (0) if the command executed successfully, or an error code
 */

static void	print_args(char **args, int option, int i)
{
	while (args[i] && args[i + 1])
	{
		ft_putstr(args[i]);
		ft_putstr(" ");
		i++;
	}
	if (args[i])
		ft_putstr(args[i]);
	if (!option)
		ft_putstr("\n");
}

static int	option_echo(t_info *cmd)
{
	int	i;

	i = 1;
	while ((cmd->args)[i] && (ft_strcmp((cmd->args)[i], "-n") == 0))
		i++;
	return (i);
}

int			ft_echo(t_info *cmd, t_list **envl)
{
	int		i;
	int		pid;

	(void)envl;
	i = option_echo(cmd);
	pid = fork();
	if (pid == -1)
		return (error_msg(FORK_FAIL));
	else if (pid == 0)
	{
		change_stdin_stdout(cmd);
		print_args(cmd->argv + cmd->offset, (i > 1), i);
		exit(SUCCESS);
	}
	return (SUCCESS);
}
