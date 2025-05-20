#include "minishell.h"
#include "parser.h"
#include "structures.h"

static t_tree *new_exec_node(const t_ir_cmd *src)
{
    t_tree  *n;
    t_info  *info;
    size_t  i;

    /* Créer le nœud et son info */
    n = ft_calloc(1, sizeof(*n));
    if (!n)
        exit(EXIT_FAILURE);
    
    info = ft_calloc(1, sizeof(*info));
    if (!info)
    {
        free(n);
        exit(EXIT_FAILURE);
    }
    
    /* Configuration de base */
    info->type = CMD;
    info->input = STDIN;
    info->output = STDOUT;
    
    /* Copier les arguments */
    for (i = 0; src->argv && src->argv[i]; ++i)
        ;
    
    info->args = ft_calloc(i + 1, sizeof(char *));
    if (!info->args)
    {
        free(info);
        free(n);
        exit(EXIT_FAILURE);
    }
    
    for (i = 0; src->argv && src->argv[i]; ++i)
    {
        info->args[i] = ft_strdup(src->argv[i]);
        if (!info->args[i])
        {
            while (i > 0)
                free(info->args[--i]);
            free(info->args);
            free(info);
            free(n);
            exit(EXIT_FAILURE);
        }
    }
    
    /* Traiter les redirections */
    for (i = 0; i < src->n_redir; ++i)
    {
        if (src->redir[i].type == IR_REDIR_IN ||
            src->redir[i].type == IR_REDIR_HEREDOC)
        {
            /* Redirection d'entrée */
            if (info->input != STDIN)
                close(info->input);
            
            info->input = open(src->redir[i].target, O_RDONLY);
            if (info->input < 0)
            {
                info->file_error = ft_strdup(src->redir[i].target);
                info->err = errno;
            }
        }
        else
        {
            /* Redirection de sortie (normale ou append) */
            if (info->output != STDOUT)
                close(info->output);
            
            if (src->redir[i].type == IR_REDIR_APPEND)
                info->output = open(src->redir[i].target, 
                    O_WRONLY | O_CREAT | O_APPEND, 0644);
            else
                info->output = open(src->redir[i].target, 
                    O_WRONLY | O_CREAT | O_TRUNC, 0644);
            
            if (info->output < 0)
            {
                info->file_error = ft_strdup(src->redir[i].target);
                info->err = errno;
            }
        }
    }
    
    n->info = info;
    return (n);
}

t_tree *create_tree_from_ir(const t_ir_line *ir)
{
    size_t  i;
    t_tree  *root;
    t_tree  *prev;
    t_tree  *pipe_node;
    t_info  *pipe_info;

    if (ir->n_cmd == 0)
        return (NULL);

    root = new_exec_node(&ir->cmd[0]);
    prev = root;

    for (i = 1; i < ir->n_cmd; ++i)
    {
        /* Créer le nœud pipe */
        pipe_node = ft_calloc(1, sizeof(*pipe_node));
        if (!pipe_node)
            exit(EXIT_FAILURE);
        
        /* Configurer l'info du pipe */
        pipe_info = ft_calloc(1, sizeof(*pipe_info));
        if (!pipe_info)
        {
            free(pipe_node);
            exit(EXIT_FAILURE);
        }
        
        pipe_info->type = PIPE;  /* Utiliser la constante définie dans structures.h */
        pipe_node->info = pipe_info;
        
        /* Créer le nœud de la commande de droite */
        t_tree *right = new_exec_node(&ir->cmd[i]);
        
        /* Configurer les connexions */
        pipe_node->left = prev;
        pipe_node->right = right;
        prev = pipe_node;
    }
    
    return (prev);  /* le dernier pipe est la racine */
}
