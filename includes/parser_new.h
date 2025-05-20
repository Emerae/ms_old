#ifndef PARSER_NEW_H
#define PARSER_NEW_H

#include "prser.h"
#include "libftstruct.h"

/* Wrapper to expose high-level entry points */
t_cmd    *parse_command_new(char *line, t_list *env, int *status);

#endif /* PARSER_NEW_H */
