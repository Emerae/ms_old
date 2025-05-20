#include "parser_new.h"
#include "minishell.h"

/**
 * @brief Crée un nœud d'arbre pour une commande
 *
 * @param cmd Structure de commande du nouveau parseur
 * @param line Ligne de commande originale
 * @return t_tree* Nœud créé
 */
static t_tree	*create_cmd_node(t_cmd *cmd, char *line)
{
	t_tree	*node;
	t_info	*info;
	
	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
	info = malloc(sizeof(t_info));
	if (!info)
	{
		free(node);
		return (NULL);
	}
	info->type = CMD;
	info->input = 0;
	info->output = 1;
	info->line = line;
	info->args = copy_args(cmd->args);
	info->builtin = cmd->builtin_id;
	apply_redirections_to_info(info, cmd->redirs);
	node->info = info;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

/**
 * @brief Crée un nœud de type pipe
 *
 * @param line Ligne de commande originale
 * @return t_tree* Nœud créé
 */
static t_tree	*create_pipe_node(char *line)
{
	t_tree	*node;
	t_info	*info;
	
	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
	info = malloc(sizeof(t_info));
	if (!info)
	{
		free(node);
		return (NULL);
	}
	info->type = PIPE;
	info->line = line;
	info->args = NULL;
	info->builtin = 0;
	node->info = info;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

/**
 * @brief Configure les pipes pour les nœuds de l'arbre
 *
 * @param root Racine de l'arbre
 */
static void	setup_tree_io(t_tree *root)
{
	int	pipefd[2];
	
	if (!root)
		return;
	if (root->info->type == PIPE)
	{
		if (pipe(pipefd) == 0)
		{
			if (root->left && root->left->info->output == 1)
				root->left->info->output = pipefd[1];
			else
				close(pipefd[1]);
			if (root->right && root->right->info->input == 0)
				root->right->info->input = pipefd[0];
			else
				close(pipefd[0]);
		}
		setup_tree_io(root->right);
	}
}

/**
 * @brief Convertit une liste de commandes en arbre syntaxique
 *
 * @param cmds Liste de commandes
 * @param line Ligne de commande originale
 * @return t_tree* Arbre créé
 */
t_tree	*convert_cmdlist_to_tree(t_cmd *cmds, char *line)
{
	t_tree	*root;
	t_tree	*current;
	t_tree	*new_node;
	t_cmd	*cmd_current;
	
	if (!cmds)
		return (NULL);
	if (!cmds->next)
		return (create_cmd_node(cmds, line));
	
	root = NULL;
	current = NULL;
	cmd_current = cmds;
	
	while (cmd_current)
	{
		new_node = create_cmd_node(cmd_current, line);
		if (!new_node)
		{
			free_tree(root);
			return (NULL);
		}
		
		if (!root)
		{
			root = new_node;
			current = root;
		}
		else
		{
			t_tree *pipe_node = create_pipe_node(line);
			if (!pipe_node)
			{
				free_tree(root);
				free_tree(new_node);
				return (NULL);
			}
			
			if (!root->right)
			{
				root->right = pipe_node;
				pipe_node->left = root;
			}
			else
			{
				current->right = pipe_node;
				pipe_node->left = current;
			}
			
			pipe_node->right = new_node;
			current = pipe_node;
		}
		
		cmd_current = cmd_current->next;
	}
	
	setup_tree_io(root);
	return (root);
}
