#include "minishell.h"
#include "parser.h"
#include "structures.h"

int exec_ir(const t_ir_line *ir, t_list **envl)
{
    int     err;
    int     status;
    t_tree *tree;

    tree = create_tree_from_ir(ir);
    if (!tree)
        return (0);

    err = execute_pipeline(tree, envl);

    while (wait(&status) > 0)
        ;
    return (return_value(status, err));
}

