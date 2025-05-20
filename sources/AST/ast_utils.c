#include "minishell.h"
#include "parser.h"
#include "structures.h"


void    free_ir(t_ir_line *ir)
{
    size_t i;
    size_t j;

    if (!ir)
        return ;
    i = 0;
    while (i < ir->n_cmd)
    {
        /* argv */
        j = 0;
        while (ir->cmd[i].argv && ir->cmd[i].argv[j])
            free(ir->cmd[i].argv[j++]);
        free(ir->cmd[i].argv);
        /* redirections */
        j = 0;
        while (j < ir->cmd[i].n_redir)
            free(ir->cmd[i].redir[j++].target);
        free(ir->cmd[i].redir);
        ++i;
    }
    free(ir->cmd);
    free(ir);
}
