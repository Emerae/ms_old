CC			= clang -Wall -Wextra -Werror
RM			= rm -rf
NAME		= minishell 
INCS_DIR	= ./includes/
MAIN_INC	= -I$(INCS_DIR) $(PARSER_NEW_INC)

LIB_DIR		= ./libft/
LIB_INC		= -I$(LIB_DIR)includes/
LIB_NAME	= $(LIB_DIR)libft.a

# New parser integration
PARSER_NEW_DIR = ./sources/parser_new/parser
PARSER_NEW_INC = -I$(PARSER_NEW_DIR)

INCS		= $(addprefix $(INCS_DIR), minishell.h structures.h)

SRCS		=  $(addprefix sources/, \
			execute/args.c \
			execute/types.c \
			execute/fork.c \
			execute/command.c \
			execute/execute.c \
			execute/file.c \
			execute/useful.c \
			execute/pipe.c \
			parser_new/parser/cyutil/cy_memset.c \
			parser_new/parser/cyutil/cy_strchr.c \
			parser_new/parser/cyutil/cy_strcmp.c \
			parser_new/parser/cyutil/cy_strdup.c \
			parser_new/parser/cyutil/cy_strlcat.c \
			parser_new/parser/cyutil/cy_strlcpy.c \
			parser_new/parser/cyutil/cy_strlen.c \
			parser_new/parser/cyutil/cy_true_strdup.c \
			parser_new/parser/pars/cy0_check_char.c \
			parser_new/parser/pars/cy0_freeer.c \
			parser_new/parser/pars/cy0_init_env.c \
			parser_new/parser/pars/cy1_1_remove_space_nodes.c \
			parser_new/parser/pars/cy1_env.c \
			parser_new/parser/pars/cy1_input_list.c \
			parser_new/parser/pars/cy1_input_list1.c \
			parser_new/parser/pars/cy1_input_list2.c \
			parser_new/parser/pars/cy2_1_fill_builtin.c \
			parser_new/parser/pars/cy2_2_fill_redir.c \
			parser_new/parser/pars/cy2_2_fill_redir2.c \
			parser_new/parser/pars/cy2_3_free_first_node.c \
			parser_new/parser/pars/cy2_convert_cmd.c \
			parser_new/parser/pars/cy2_convert_cmd2.c \
			parser_new/parser/pars/cy2_convert_cmd3.c \
			parser_new/parser/pars/cy3_2_dollar.c \
			parser_new/parser/pars/cy3_2_dollar2.c \
			parser_new/parser/pars/cy3_2_dollar_alone.c \
			parser_new/parser/pars/cy3_2_dollar_bang.c \
			parser_new/parser/pars/cy3_2_dollar_braces.c \
			parser_new/parser/pars/cy3_2_dollar_braces2.c \
			parser_new/parser/pars/cy3_2_dollar_braces3.c \
			parser_new/parser/pars/cy3_2_dollar_word.c \
			parser_new/parser/pars/cy3_2_dollar_word2.c \
			parser_new/parser/pars/cy3_subti_check.c \
			parser_new/parser/pars/cy3_subti_fuse.c \
			parser_new/parser/pars/cy4_1wrong_char.c \
			parser_new/parser/pars/cy4_2wrong_redir.c \
			parser_new/parser/pars/cy4_3wrong_pipe.c \
			parser_new/parser/pars/cy4_4wrong_redir_log.c \
			parser_new/parser/pars/cy4_5wrong_pipe_log.c \
			parser_new/parser/pars/utils.c \
			parser_new/parse_command_new.c \
			parser_new/glue_cmd.c \
			parser_new/glue_tree.c \
			parser_new/env.c \
			parser_new/tree.c \
			parser_new/redir.c \
			parser_new/node.c \
			utilities/errors.c \
			utilities/free.c \
			utilities/prompt.c \
			utilities/reader.c \
			utilities/reader_useful.c \
			builtin/cd.c \
			builtin/echo.c \
			builtin/env.c \
			builtin/exit.c \
			builtin/export.c \
			builtin/export_sorted.c \
			builtin/pwd.c \
			builtin/unset.c \
			AST/ast_integration.c \
			AST/ast_utils.c \
			AST/create_tree.c \
			environment/declare.c \
			environment/env.c \
			environment/replace.c \
			environment/update.c \
			environment/useful.c \
			expand/join.c \
			expand/main.c \
			expand/expansion.c \
			expand/useful.c \
			main.c )
SRCS += $(PARSER_NEW_SRCS)
 
OBJS		= $(SRCS:.c=.o)

%.o			: %.c
			@$(CC) $(MAIN_INC) $(LIB_INC) -c $< -o $@

all			: $(NAME)

$(NAME)		: $(OBJS) $(INCS)
			@make --silent -C $(LIB_DIR)
			@$(CC) $(OBJS) $(LIB_NAME) -L$(LIB_DIR) $(LIB_INC) $(MAIN_INC) -lreadline -o $(NAME)
			@echo "minishell compiled"

clean:
			@$(MAKE) clean --silent -C $(LIB_DIR)
			@$(RM) $(OBJS)

fclean		: clean
			@$(MAKE) fclean --silent -C $(LIB_DIR)
			@$(RM) $(NAME)

re			: fclean all

.PHONY		: all clean fclean re