#include "parser.h"
#include "minishell.h"

/**
 * @brief Traite une redirection d'entrée
 *
 * @param info Structure d'information de l'exécuteur
 * @param current Redirection courante
 */
static void	handle_input_redirection(t_info *info, t_redir *current)
{
	int	fd;
	
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

/**
 * @brief Traite une redirection de sortie
 *
 * @param info Structure d'information de l'exécuteur
 * @param current Redirection courante
 */
static void	handle_output_redirection(t_info *info, t_redir *current)
{
	int	fd;
	
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

/**
 * @brief Traite une redirection d'ajout
 *
 * @param info Structure d'information de l'exécuteur
 * @param current Redirection courante
 */
static void	handle_append_redirection(t_info *info, t_redir *current)
{
	int	fd;
	
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

/**
 * @brief Choisit le handler de redirection approprié
 *
 * @param info Structure d'information de l'exécuteur
 * @param current Redirection courante
 */
static void	handle_redirection(t_info *info, t_redir *current)
{
	if (current->type == 0)
		handle_input_redirection(info, current);
	else if (current->type == 1)
		handle_output_redirection(info, current);
	else if (current->type == 2)
		handle_append_redirection(info, current);
}

/**
 * @brief Applique les redirections à une info d'exécuteur
 *
 * @param info Structure d'information de l'exécuteur
 * @param redirs Liste des redirections
 */
void	apply_redirections_to_info(t_info *info, t_redir *redirs)
{
	t_redir	*current;
	
	current = redirs;
	while (current)
	{
		handle_redirection(info, current);
		current = current->next;
	}
}
