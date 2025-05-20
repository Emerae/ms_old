#include "parser_new.h"
#include "libftfull.h"

/*
** Simple wrapper converting readline input into t_cmd* using the new parser.
** It also detects forbidden characters such as ';' and '\\' outside quotes
** and returns NULL while setting *status = 258 (bash syntax error code).
*/

static int contains_forbidden_char(t_input *head)
{
    t_input *tmp = head;
    while (tmp)
    {
        if (tmp->type == 2 && (tmp->input[0] == ';' || tmp->input[0] == '\\') && tmp->input[1] == '\0')
            return (1);
        tmp = tmp->next;
    }
    return (0);
}

t_cmd    *parse_command_new(char *line, t_list *env, int *status)
{
    t_input *head = NULL;
    *status = 0;
    if (!line || !*line)
        return (NULL);
    head = cy1_make_list(line);
    if (!head)
    {
        *status = 258;
        return (NULL);
    }
    /* Expansion */
    if (cy3_substi_check(&head, (char **)env))
    {
        cy0_free_input_list(head);
        *status = 258;
        return (NULL);
    }
    /* Syntax validation */
    if (cy4_1wrong_char(head) || contains_forbidden_char(head))
    {
        cy0_free_input_list(head);
        *status = 258;
        return (NULL);
    }
    t_cmd *cmds = cy2_convert_cmd(head);
    cy0_free_input_list(head);
    if (!cmds)
        *status = 258;
    return (cmds);
}
