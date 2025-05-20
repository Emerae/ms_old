/* sources/execute/ir_driver.c */
#include "minishell.h"
#include "ir.h"

/* Crée un nœud d'arbre pour l'exécuteur */
static t_tree *create_node_from_cmd(t_ir_cmd *cmd, char *line)
{
	t_tree *node;
	t_info *info;
	
	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
	
	info = malloc(sizeof(t_info));
	if (!info)
	{
		free(node);
		return (NULL);
	}
	
	/* Initialiser l'info */
	info->type = CMD;
	info->input = STDIN;
	info->output = STDOUT;
	info->line = line;
	info->args = NULL;
	info->argv = NULL;
	info->seps = NULL;
	info->spaces = NULL;
	info->err = 0;
	info->file_error = NULL;
	
	/* Copier les arguments */
	info->args = copy_args(cmd->argv);
	if (!info->args)
	{
		free(info);
		free(node);
		return (NULL);
	}
	
	/* Traiter les redirections */
	for (size_t i = 0; i < cmd->n_redir; i++)
	{
		if (cmd->redir[i].type == IR_REDIR_IN)
		{
			info->input = open(cmd->redir[i].target, O_RDONLY);
			if (info->input < 0)
			{
				info->file_error = cmd->redir[i].target;
				info->err = errno;
			}
		}
		else if (cmd->redir[i].type == IR_REDIR_OUT)
		{
			info->output = open(cmd->redir[i].target, 
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (info->output < 0)
			{
				info->file_error = cmd->redir[i].target;
				info->err = errno;
			}
		}
		else if (cmd->redir[i].type == IR_REDIR_APPEND)
		{
			info->output = open(cmd->redir[i].target,
				O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (info->output < 0)
			{
				info->file_error = cmd->redir[i].target;
				info->err = errno;
			}
		}
	}
	
	node->info = info;
	node->left = NULL;
	node->right = NULL;
	
	return (node);
}

/* Crée un arbre à partir de la structure IR */
static t_tree *create_tree_from_ir(const t_ir_line *ir, char *line)
{
	t_tree *root = NULL;
	t_tree *current = NULL;
	
	if (ir->n_cmd == 0)
		return (NULL);
	
	/* Créer le premier nœud */
	root = create_node_from_cmd(&ir->cmd[0], line);
	if (!root)
		return (NULL);
	
	current = root;
	
	/* Ajouter les commandes restantes comme nœuds PIPE */
	for (size_t i = 1; i < ir->n_cmd; i++)
	{
		t_tree *pipe_node = malloc(sizeof(t_tree));
		t_tree *cmd_node = create_node_from_cmd(&ir->cmd[i], line);
		t_info *pipe_info;
		
		if (!pipe_node || !cmd_node)
		{
			if (pipe_node)
				free(pipe_node);
			if (cmd_node)
				free_tree(cmd_node);
			free_tree(root);
			return (NULL);
		}
		
		pipe_info = malloc(sizeof(t_info));
		if (!pipe_info)
		{
			free(pipe_node);
			free_tree(cmd_node);
			free_tree(root);
			return (NULL);
		}
		
		/* Initialiser l'info du nœud pipe */
		pipe_info->type = PIPE;
		pipe_info->line = line;
		pipe_info->args = NULL;
		pipe_info->argv = NULL;
		pipe_info->input = 0;
		pipe_info->output = 1;
		
		pipe_node->info = pipe_info;
		pipe_node->left = current;
		pipe_node->right = cmd_node;
		
		current = pipe_node;
	}
	
	return (current);
}

/* Exécute une structure IR */
int exec_ir(const t_ir_line *ir, t_list **envl)
{
	t_tree *tree;
	int status;
	int err;
	
	if (!ir)
		return (ERROR);
	
	/* Créer l'arbre */
	tree = create_tree_from_ir(ir, "command line");
	if (!tree)
		return (ERROR);
	
	/* Préparer les pipes */
	if (pipe_recursive(tree, NULL, envl))
	{
		free_tree(tree);
		print_error(NULL, NULL, TOOMANY, NULL);
		return (TOOMANY);
	}
	
	/* Exécuter l'arbre */
	err = execute_recursive(tree, NULL, envl);
	
	if (g_signal != 6)
		g_signal = 5;
	
	/* Attendre la fin des processus */
	while (wait(&status) > 0)
		;
	
	free_tree(tree);
	
	return (return_value(status, err));
}

/* Libérer la mémoire de la structure IR */
void free_ir(t_ir_line *ir)
{
	if (!ir)
		return;
	
	for (size_t i = 0; i < ir->n_cmd; i++)
	{
		/* Libérer les arguments */
		if (ir->cmd[i].argv)
		{
			for (int j = 0; ir->cmd[i].argv[j]; j++)
				free(ir->cmd[i].argv[j]);
			free(ir->cmd[i].argv);
		}
		
		/* Libérer les redirections */
		if (ir->cmd[i].redir)
		{
			for (size_t j = 0; j < ir->cmd[i].n_redir; j++)
				free(ir->cmd[i].redir[j].target);
			free(ir->cmd[i].redir);
		}
	}
	
	free(ir->cmd);
	free(ir);
}