#include "minishell.h"
#include "parser.h"
#include "structures.h"

static t_tree *new_exec_node(const t_ir_cmd *src)
{
    t_tree *n;
    size_t  i;

    n = ft_calloc(1, sizeof(*n));
    if (!n)
        exit(EXIT_FAILURE);

    /* args */
    for (i = 0; src->argv && src->argv[i]; ++i)
        ;
    n->args = ft_calloc(i + 1, sizeof(char *));
    if (!n->args)
        exit(EXIT_FAILURE);
    for (i = 0; src->argv && src->argv[i]; ++i)
    {
        n->args[i] = ft_strdup(src->argv[i]);
        if (!n->args[i])
            exit(EXIT_FAILURE);
    }

    /* redirections */
    for (i = 0; i < src->n_redir; ++i)
    {
        if (src->redir[i].type == IR_REDIR_IN ||
            src->redir[i].type == IR_REDIR_HEREDOC)
            n->infile = ft_strdup(src->redir[i].target);
        else
        {
            n->outfile = ft_strdup(src->redir[i].target);
            n->append  = (src->redir[i].type == IR_REDIR_APPEND);
        }
    }
    return (n);
}

t_tree *create_tree_from_ir(const t_ir_line *ir)
{
    size_t  i;
    t_tree *root;
    t_tree *prev;

    if (ir->n_cmd == 0)
        return (NULL);

    root = new_exec_node(&ir->cmd[0]);
    prev = root;

    for (i = 1; i < ir->n_cmd; ++i)
    {
        t_tree *pipe_node = ft_calloc(1, sizeof(*pipe_node));
        t_tree *right     = new_exec_node(&ir->cmd[i]);

        if (!pipe_node)
            exit(EXIT_FAILURE);
        pipe_node->type  = NODE_PIPE;
        pipe_node->left  = prev;
        pipe_node->right = right;
        prev             = pipe_node;
    }
    return (prev);      /* le dernier pipe est la racine */
}
