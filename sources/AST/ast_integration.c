#include "minishell.h"
#include "parser_new.h"
#include "structures.h"

static size_t count_pipes(const t_tree *n)
{
    if (!n)
        return (0);
    if (n->type == AST_PIPE)
        return (1 + count_pipes(n->left) + count_pipes(n->right));
    return (count_pipes(n->left) + count_pipes(n->right));
}

static void fill_ir_cmd(t_ir_cmd *dst, const t_tree *node, t_list **envl)
{
    size_t      i;
    t_list     *it;

    /* argv déjà sans quotes, on applique expansions et globbing ici */
    dst->argv = ast_expand_argv(node, envl);  /* implémentez/branchez */

    /* redirections dans l’ordre d’apparition */
    dst->n_redir = ft_lstsize(node->redir);
    dst->redir   = (dst->n_redir) ?
                   malloc(sizeof(t_ir_redir) * dst->n_redir) : NULL;
    if (dst->n_redir && !dst->redir)
        exit(EXIT_FAILURE);

    i  = 0;
    it = node->redir;
    while (it)
    {
        t_ast_redir *r = it->content;
        dst->redir[i].type = (r->type == RD_APPEND) ? IR_REDIR_APPEND :
                              (r->type == RD_OUT)   ? IR_REDIR_OUT    :
                              (r->type == RD_IN)    ? IR_REDIR_IN     :
                                                    IR_REDIR_HEREDOC;
        dst->redir[i].target = ft_strdup(r->word);
        if (!dst->redir[i].target)
            exit(EXIT_FAILURE);
        ++i;
        it = it->next;
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

    if (!root)
        return (NULL);

    n_pipes   = count_pipes(root);
    ir        = malloc(sizeof(*ir));
    if (!ir)
        exit(EXIT_FAILURE);
    ir->n_cmd = n_pipes + 1;
    ir->cmd   = ft_calloc(ir->n_cmd, sizeof(*ir->cmd));
    if (!ir->cmd)
        exit(EXIT_FAILURE);

    /* parcours gauche‑droite in‑order */
    idx = 0;
    sp  = 0;
    cur = root;
    while (cur || sp)
    {
        while (cur && cur->type == AST_PIPE)
        {
            stack[sp++] = cur->right;
            cur = cur->left;
        }
        if (cur)
        {
            fill_ir_cmd(&ir->cmd[idx++], cur, envl);
            cur = NULL;
        }
        if (sp)
            cur = stack[--sp];
    }
    return (ir);
}
