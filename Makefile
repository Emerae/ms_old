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
PARSER_NEW_SRCS = $(shell find $(PARSER_NEW_DIR) -name "*.c" ! -name "main.c") ./sources/parser_new/parse_command_new.c
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
			parse/count.c \
			parse/env.c \
			parse/parse.c \
			parse/redir.c \
			parse/state.c \
			parse/tree.c \
			parse/node.c \
			parse/trim.c \
			parse/useful.c \
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