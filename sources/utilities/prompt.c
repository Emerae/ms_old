
#include "minishell.h"

void	prompt(void)
{
	char	cwd[SIZE_PATH];

	getcwd(cwd, SIZE_PATH);
	ft_putstr("\033[1;36mmini@shell \033[0;36m");
	ft_putstr(cwd);
	ft_putstr("\033[0m$ ");
}

void	header(void)
{
	ft_putstr("\033[1;36m  __  __ _       _     _          _ _\n");
	ft_putstr(" |  \\/  (_)_ __ (_)___| |__   ___| | |\n");
	ft_putstr(" | |\\/| | | '_ \\| / __| '_ \\ / _ \\ | |\n");
	ft_putstr(" | |  | | | | | | \\__ \\ | | |  __/ | |\n");
	ft_putstr(" |_|  |_|_|_| |_|_|___/_| |_|\\___|_|_|\033[0m\n\n");
}
