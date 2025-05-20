#ifndef PARSER_NEW_H
# define PARSER_NEW_H

# include "prser.h"
# include "libftstruct.h"
# include "structures.h"  /* Ajoutez cette ligne pour importer t_tree */

/* Wrapper to expose high-level entry points */
t_tree  *parse_command_new(char *line, t_list *env, int *status);

/* Nouvelle fonction pour la conversion */
t_tree *convert_cmdlist_to_tree(t_cmd *cmds, char *line);

/* Fonction pour libérer la liste de commandes */
void   free_cmd_list(t_cmd *cmd);

char	**copy_args(char **args);

void	apply_redirections_to_info(t_info *info, t_redir *redirs);



#endif /* PARSER_NEW_H */