#include "minishell.h"
#include "parser_new.h"

/**
 * @brief Convertit une liste de commandes du nouveau parseur en arbre pour l'exécuteur
 *
 * Cette fonction prend la sortie du nouveau parseur (liste chaînée de t_cmd)
 * et crée un arbre t_tree compatible avec l'exécuteur existant.
 *
 * @param cmds Liste chaînée de commandes du nouveau parseur
 * @param line Ligne de commande originale (pour compatibilité avec t_info)
 * @return t_tree* Arbre syntaxique pour l'exécuteur
 */
t_tree	*convert_cmdlist_to_tree(t_cmd *cmds, char *line)
{
	t_tree	*root;
	t_tree	*current;
	t_tree	*new_node;
	t_cmd	*cmd_current;
	int		pipe_count;

	/* Cas où il n'y a pas de commandes */
	if (!cmds)
		return (NULL);

	/* Si une seule commande (sans pipe), créer un nœud feuille unique */
	if (!cmds->next)
		return (create_single_cmd_node(cmds, line));

	/* Initialisation pour plusieurs commandes */
	root = NULL;
	current = NULL;
	cmd_current = cmds;
	pipe_count = 0;

	/* Parcourir la liste des commandes et construire l'arbre */
	while (cmd_current)
	{
		/* Créer un nœud pour la commande courante */
		new_node = create_cmd_node(cmd_current, line, pipe_count);
		
		if (!new_node)
		{
			free_tree(root);
			return (NULL);
		}

		/* Si premier nœud, l'affecter comme racine */
		if (!root)
		{
			root = new_node;
			current = root;
		}
		else
		{
			/* Sinon, ajouter comme fils droit du nœud précédent */
			if (current->info->type == PIPE)
			{
				current->right = new_node;
			}
			else
			{
				/* Créer un nœud de pipe entre les commandes */
				t_tree *pipe_node = create_pipe_node(line);
				
				if (!pipe_node)
				{
					free_tree(root);
					free_tree(new_node);
					return (NULL);
				}
				
				/* Connecter le nœud de pipe */
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
		}

		cmd_current = cmd_current->next;
		pipe_count++;
	}

	/* Finaliser l'arbre en définissant les entrées/sorties */
	setup_tree_io(root);

	return (root);
}

/**
 * @brief Crée un nœud d'arbre pour une commande unique
 *
 * @param cmd Structure de commande du nouveau parseur
 * @param line Ligne de commande originale
 * @return t_tree* Nœud d'arbre pour la commande
 */
t_tree	*create_single_cmd_node(t_cmd *cmd, char *line)
{
	t_tree	*node;
	t_info	*info;
	
	/* Initialiser le nœud de l'arbre */
	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
		
	/* Initialiser les informations de la commande */
	info = malloc(sizeof(t_info));
	if (!info)
	{
		free(node);
		return (NULL);
	}
	
	/* Remplir les champs de base */
	info->type = CMD;
	info->input = 0;  /* stdin par défaut */
	info->output = 1; /* stdout par défaut */
	info->line = line;
	info->args = copy_args(cmd->args);
	info->builtin = cmd->builtin_id;
	
	/* Gérer les redirections */
	apply_redirections_to_info(info, cmd->redirs);
	
	/* Finaliser le nœud */
	node->info = info;
	node->left = NULL;
	node->right = NULL;
	
	return (node);
}

/**
 * @brief Crée un nœud d'arbre pour une commande dans un pipeline
 *
 * @param cmd Structure de commande du nouveau parseur
 * @param line Ligne de commande originale
 * @param position Position dans le pipeline
 * @return t_tree* Nœud d'arbre pour la commande
 */
t_tree	*create_cmd_node(t_cmd *cmd, char *line, int position)
{
	t_tree	*node;
	t_info	*info;
	
	/* Similaire à create_single_cmd_node mais pour une commande dans un pipeline */
	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
		
	info = malloc(sizeof(t_info));
	if (!info)
	{
		free(node);
		return (NULL);
	}
	
	/* Remplir les champs */
	info->type = CMD;
	info->line = line;
	info->args = copy_args(cmd->args);
	info->builtin = cmd->builtin_id;
	
	/* Dans un pipeline, l'entrée/sortie sera configurée plus tard */
	info->input = 0;
	info->output = 1;
	
	/* Gérer les redirections */
	apply_redirections_to_info(info, cmd->redirs);
	
	/* Finaliser le nœud */
	node->info = info;
	node->left = NULL;
	node->right = NULL;
	
	return (node);
}

/**
 * @brief Crée un nœud de pipe pour l'arbre
 *
 * @param line Ligne de commande originale
 * @return t_tree* Nœud de pipe
 */
t_tree	*create_pipe_node(char *line)
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
	
	/* Configuration pour un nœud de pipe */
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
 * @brief Applique les redirections du nouveau parseur à la structure t_info
 *
 * @param info Structure d'information pour l'exécuteur
 * @param redirs Liste des redirections du nouveau parseur
 */
void	apply_redirections_to_info(t_info *info, t_redir *redirs)
{
	t_redir	*current;
	int		fd;
	
	current = redirs;
	while (current)
	{
		if (current->type == 0)  /* < (entrée) */
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
				/* Stocker l'erreur pour l'afficher plus tard */
				info->file_error = current->file;
				info->err = errno;
			}
		}
		else if (current->type == 1)  /* > (sortie) */
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
		else if (current->type == 2)  /* >> (ajout) */
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
		/* Pour le type 3 (<<), il faudrait implémenter la gestion des heredocs */
		
		current = current->next;
	}
}

/**
 * @brief Configure les entrées/sorties pour les commandes dans l'arbre
 *
 * Cette fonction parcourt l'arbre et configure les fds d'entrée/sortie
 * pour chaque commande en créant des pipes entre elles.
 *
 * @param root Racine de l'arbre
 */
void	setup_tree_io(t_tree *root)
{
	int	pipefd[2];
	
	if (!root)
		return;
		
	/* Si le nœud est un pipe */
	if (root->info->type == PIPE)
	{
		/* Créer un pipe */
		if (pipe(pipefd) == 0)
		{
			/* Commande gauche écrit dans le pipe */
			if (root->left && root->left->info->output == 1)
				root->left->info->output = pipefd[1];
			else
				close(pipefd[1]);
				
			/* Commande droite lit depuis le pipe */
			if (root->right && root->right->info->input == 0)
				root->right->info->input = pipefd[0];
			else
				close(pipefd[0]);
		}
		
		/* Configurer récursivement */
		setup_tree_io(root->right);
	}
}

/**
 * @brief Copie un tableau d'arguments
 *
 * @param args Tableau d'arguments à copier
 * @return char** Nouveau tableau d'arguments
 */
char	**copy_args(char **args)
{
	int		count;
	char	**new_args;
	int		i;
	
	/* Compter le nombre d'arguments */
	count = 0;
	while (args && args[count])
		count++;
		
	/* Allouer le tableau */
	new_args = malloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return (NULL);
		
	/* Copier chaque argument */
	i = 0;
	while (i < count)
	{
		new_args[i] = ft_strdup(args[i]);
		if (!new_args[i])
		{
			/* En cas d'erreur, libérer ce qui a été alloué */
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
