#include "parser.h"
#include "libftfull.h"

/**
 * @brief Parse une ligne de commande avec le nouveau parseur et la convertit en format pour l'exécuteur
 *
 * @param line Ligne de commande à analyser
 * @param env Liste des variables d'environnement
 * @param status Pointeur pour stocker le statut
 * @return t_tree* Arbre syntaxique pour l'exécuteur
 */
t_tree	*parse_command_new(char *line, t_list *env, int *status)
{
	t_input	*head;
	t_cmd	*cmds;
	t_tree	*tree;
	
	*status = 0;
	if (!line || !*line)
		return (NULL);
		
	/* Utiliser le nouveau parseur */
	head = cy1_make_list(line);
	if (!head)
	{
		*status = 258;
		return (NULL);
	}
	
	/* Effectuer les étapes du parseur */
	if (cy3_substi_check(&head, (char **)env))
	{
		cy0_free_input_list(head);
		*status = 258;
		return (NULL);
	}
	
	/* Valider la syntaxe */
	if (cy4_1wrong_char(head))
	{
		cy0_free_input_list(head);
		*status = 258;
		return (NULL);
	}
	
	/* Convertir en liste de commandes */
	cmds = cy2_convert_cmd(head);
	cy0_free_input_list(head);
	
	if (!cmds)
	{
		*status = 258;
		return (NULL);
	}
	
	/* Convertir la liste de commandes en arbre */
	tree = convert_cmdlist_to_tree(cmds, line);
	
	/* Libérer la liste de commandes */
	free_cmd_list(cmds);
	
	if (!tree)
		*status = 258;
		
	return (tree);
}
