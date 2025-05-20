#ifndef IR_H
# define IR_H

# include <stddef.h>
# include "structures.h"  /* Pour t_tree */
# include "libftfull.h"   /* Pour t_list */

typedef enum e_ir_redir_type
{
    IR_REDIR_IN,
    IR_REDIR_OUT,
    IR_REDIR_APPEND,
    IR_REDIR_HEREDOC
} t_ir_redir_type;

typedef struct s_ir_redir
{
    t_ir_redir_type type;    /* type de redirection */
    char           *target;  /* fichier ou delimiter (malloc) */
} t_ir_redir;

typedef struct s_ir_cmd
{
    char       **argv;       /* NULL-terminated, déjà expansé */
    t_ir_redir  *redir;      /* tableau dynamique de redirections */
    size_t       n_redir;    /* nombre de redirections */
} t_ir_cmd;

typedef struct s_ir_line
{
    t_ir_cmd    *cmd;        /* tableau de commandes */
    size_t       n_cmd;      /* nombre de commandes */
} t_ir_line;

/* Fonctions pour manipuler la structure IR */
t_ir_line *ast_to_ir(t_tree *root, t_list **envl);
int        exec_ir(const t_ir_line *ir, t_list **envl);
void       free_ir(t_ir_line *ir);
t_tree    *create_tree_from_ir(const t_ir_line *ir);

#endif