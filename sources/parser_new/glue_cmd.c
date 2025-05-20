#include "parser.h"
#include "minishell.h"



char	**copy_args(char **args)
{
	int		count;
	char	**new_args;
	int		i;
	
	count = 0;
	while (args && args[count])
		count++;
	new_args = malloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_args[i] = ft_strdup(args[i]);
		if (!new_args[i])
		{
			while (--i >= 0)
				free(new_args[i]);
			free(new_args);
			return (NULL);
		}
		i++;
	}
	new_args[count] = NULL;
	return (new_args);
}

void	apply_redirections_to_info(t_info *info, t_redir *redirs)
{
	t_redir	*current;
	int		fd;
	
	current = redirs;
	while (current)
	{
		if (current->type == 0)
		{
			fd = open(current->file, O_RDONLY);
			if (fd != -1)
			{
				if (info->input != 0)
					close(info->input);
				info->input = fd;
			}
			else
			{
				info->file_error = current->file;
				info->err = errno;
			}
		}
		else if (current->type == 1)
		{
			fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd != -1)
			{
				if (info->output != 1)
					close(info->output);
				info->output = fd;
			}
			else
			{
				info->file_error = current->file;
				info->err = errno;
			}
		}
		else if (current->type == 2)
		{
			fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd != -1)
			{
				if (info->output != 1)
					close(info->output);
				info->output = fd;
			}
			else
			{
				info->file_error = current->file;
				info->err = errno;
			}
		}
		current = current->next;
	}
}


/**
 * @brief Libère une liste de commandes
 *
 * @param cmd Liste de commandes à libérer
 */
void	free_cmd_list(t_cmd *cmd)
{
	cy0_free_cmd_list(cmd);
}