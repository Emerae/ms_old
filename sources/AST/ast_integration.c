#include "minishell.h"
#include "parser.h"
#include "structures.h"

/* Fonction utilitaire pour copier les arguments */
static char **copy_args_from_tree(const t_tree *node)
{
	char	**args;
	int		i;
	int		count;

	if (!node || !node->info || !node->info->args)
		return (NULL);
	
	/* Compter les arguments */
	count = 0;
	while (node->info->args[count])
		count++;
	
	/* Allouer le tableau */
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	
	/* Copier chaque argument */
	i = 0;
	while (i < count)
	{
		args[i] = ft_strdup(node->info->args[i]);
		if (!args[i])
		{
			while (--i >= 0)
				free(args[i]);
			free(args);
			return (NULL);
		}
		i++;
	}
	args[count] = NULL;
	
	return (args);
}

static size_t count_pipes(const t_tree *n)
{
	if (!n)
		return (0);
	if (n->info && n->info->type == PIPE)
		return (1 + count_pipes(n->left) + count_pipes(n->right));
	return (count_pipes(n->left) + count_pipes(n->right));
}

static void fill_ir_cmd(t_ir_cmd *dst, const t_tree *node)
{
	size_t i;
	
	/* Copier les arguments */
	dst->argv = copy_args_from_tree(node);
	if (!dst->argv)
		exit(EXIT_FAILURE);
	
	/* Gérer les redirections */
	dst->n_redir = 0;
	dst->redir = NULL;
	
	/* Vérifier si le nœud a des redirections d'entrée/sortie */
	if (node->info->input != STDIN || node->info->output != STDOUT)
	{
		dst->n_redir = ((node->info->input != STDIN) ? 1 : 0) + 
					   ((node->info->output != STDOUT) ? 1 : 0);
		dst->redir = malloc(sizeof(t_ir_redir) * dst->n_redir);
		if (!dst->redir)
			exit(EXIT_FAILURE);
		
		i = 0;
		/* Ajouter redirection d'entrée si nécessaire */
		if (node->info->input != STDIN)
		{
			dst->redir[i].type = IR_REDIR_IN;
			dst->redir[i].target = ft_strdup(node->info->file_error ? 
							node->info->file_error : "stdin");
			if (!dst->redir[i].target)
				exit(EXIT_FAILURE);
			i++;
		}
		
		/* Ajouter redirection de sortie si nécessaire */
		if (node->info->output != STDOUT)
		{
			/* Déterminer le type de redirection de sortie */
			/* Ne pas utiliser de ternaire (interdit par la norme) */
			if (i < dst->n_redir)
			{
				dst->redir[i].type = IR_REDIR_OUT; /* Par défaut mode normal */
				dst->redir[i].target = ft_strdup(node->info->file_error ? 
								node->info->file_error : "stdout");
				if (!dst->redir[i].target)
					exit(EXIT_FAILURE);
			}
		}
	}
}

t_ir_line *ast_to_ir(t_tree *root, t_list **envl)
{
	size_t      n_pipes;
	t_ir_line  *ir;
	size_t      idx;
	t_tree      *stack[512];
	int         sp;
	t_tree      *cur;
	
	(void)envl; /* Marquer envl comme utilisé pour éviter l'avertissement */

	if (!root)
		return (NULL);

	n_pipes = count_pipes(root);
	ir = malloc(sizeof(*ir));
	if (!ir)
		exit(EXIT_FAILURE);
	ir->n_cmd = n_pipes + 1;
	ir->cmd = ft_calloc(ir->n_cmd, sizeof(*ir->cmd));
	if (!ir->cmd)
		exit(EXIT_FAILURE);

	/* parcours gauche‑droite in‑order */
	idx = 0;
	sp = 0;
	cur = root;
	while (cur || sp)
	{
		while (cur && cur->info && cur->info->type == PIPE)
		{
			stack[sp++] = cur->right;
			cur = cur->left;
		}
		if (cur)
		{
			fill_ir_cmd(&ir->cmd[idx++], cur);
			cur = NULL;
		}
		if (sp)
			cur = stack[--sp];
	}
	return (ir);
}
