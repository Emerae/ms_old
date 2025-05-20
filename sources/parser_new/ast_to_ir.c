/* sources/parser_new/ast_to_ir.c */
#include "minishell.h"
#include "ir.h"
#include "parser_new.h"

/* Compte le nombre de commandes dans la chaîne de pipes */
static size_t count_cmds(t_tree *tree)
{
	if (!tree)
		return (0);
	
	if (tree->info->type == PIPE)
		return (count_cmds(tree->left) + count_cmds(tree->right));
	
	return (1); /* Un nœud de commande */
}

/* Copie les redirections d'un nœud vers la structure IR */
static void copy_redirections(t_ir_cmd *cmd, t_tree *node)
{
	int has_input = 0;
	int has_output = 0;
	
	/* Initialiser les redirections */
	cmd->n_redir = 0;
	cmd->redir = NULL;
	
	/* Vérifier si le nœud a des redirections d'entrée/sortie */
	if (node->info->input != STDIN)
		has_input = 1;
	if (node->info->output != STDOUT)
		has_output = 1;
	
	if (!has_input && !has_output)
		return;
	
	/* Allouer l'espace pour les redirections */
	cmd->n_redir = has_input + has_output;
	cmd->redir = ft_calloc(cmd->n_redir, sizeof(t_ir_redir));
	if (!cmd->redir)
		exit(EXIT_FAILURE);
	
	/* Ajouter la redirection d'entrée si nécessaire */
	if (has_input)
	{
		cmd->redir[0].type = IR_REDIR_IN;
		cmd->redir[0].target = ft_strdup(node->info->file_error ? 
					node->info->file_error : "stdin");
		if (!cmd->redir[0].target)
			exit(EXIT_FAILURE);
	}
	
	/* Ajouter la redirection de sortie si nécessaire */
	if (has_output)
	{
		int idx = has_input ? 1 : 0;
		cmd->redir[idx].type = IR_REDIR_OUT;
		cmd->redir[idx].target = ft_strdup(node->info->file_error ?
					node->info->file_error : "stdout");
		if (!cmd->redir[idx].target)
			exit(EXIT_FAILURE);
	}
}

/* Remplir une commande IR à partir d'un nœud de l'arbre */
static void fill_cmd(t_ir_cmd *cmd, t_tree *node, t_list **envl)
{
	int i = 0;
	
	/* Copier les arguments */
	if (node->info->args)
	{
		/* Compter les arguments */
		while (node->info->args[i])
			i++;
		
		cmd->argv = malloc(sizeof(char *) * (i + 1));
		if (!cmd->argv)
			exit(EXIT_FAILURE);
		
		/* Copier chaque argument */
		i = 0;
		while (node->info->args[i])
		{
			cmd->argv[i] = ft_strdup(node->info->args[i]);
			if (!cmd->argv[i])
				exit(EXIT_FAILURE);
			i++;
		}
		cmd->argv[i] = NULL;
	}
	else
	{
		cmd->argv = malloc(sizeof(char *));
		if (!cmd->argv)
			exit(EXIT_FAILURE);
		cmd->argv[0] = NULL;
	}
	
	/* Copier les redirections */
	copy_redirections(cmd, node);
}

/* Ajouter une commande à la structure IR */
static void add_cmd_to_ir(t_ir_line *ir, t_tree *node, t_list **envl, int *idx)
{
	if (!node)
		return;
	
	if (node->info->type == PIPE)
	{
		add_cmd_to_ir(ir, node->left, envl, idx);
		add_cmd_to_ir(ir, node->right, envl, idx);
	}
	else
	{
		fill_cmd(&ir->cmd[*idx], node, envl);
		(*idx)++;
	}
}

/* Convertir l'arbre en structure IR */
t_ir_line *ast_to_ir(t_tree *root, t_list **envl)
{
	t_ir_line *ir;
	int idx = 0;
	
	if (!root)
		return (NULL);
	
	/* Allouer la structure IR */
	ir = malloc(sizeof(t_ir_line));
	if (!ir)
		exit(EXIT_FAILURE);
	
	/* Compter le nombre de commandes */
	ir->n_cmd = count_cmds(root);
	
	/* Allouer la mémoire pour les commandes */
	ir->cmd = ft_calloc(ir->n_cmd, sizeof(t_ir_cmd));
	if (!ir->cmd)
		exit(EXIT_FAILURE);
	
	/* Remplir la structure IR */
	add_cmd_to_ir(ir, root, envl, &idx);
	
	return (ir);
}
