#include "parser.h"

/**
 * @brief Libère une liste de commandes
 *
 * @param cmd Liste de commandes à libérer
 */
void	free_cmd_list(t_cmd *cmd)
{
	cy0_free_cmd_list(cmd);
}
