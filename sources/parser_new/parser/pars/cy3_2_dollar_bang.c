#include "../prser.h"

void	cy3_handle_dollar_bang5(t_dollar_bang *sdb, t_input *current, int j)
{
	sdb->k = j + 1;
	while (current->input[sdb->k])
	{
		sdb->new_input[sdb->p] = current->input[sdb->k];
		sdb->new_type[sdb->p] = current->input_type[sdb->k];
		sdb->new_num[sdb->p] = current->input_num[sdb->k];
		sdb->k = sdb->k + 1;
		sdb->p = sdb->p + 1;
	}
}

void	cy3_handle_dollar_bang3(t_dollar_bang *sdb, t_input *current, int i)
{
	sdb->p = 0;
	sdb->k = 0;
	while (sdb->k < i)
	{
		sdb->new_input[sdb->p] = current->input[sdb->k];
		sdb->new_type[sdb->p] = current->input_type[sdb->k];
		sdb->new_num[sdb->p] = current->input_num[sdb->k];
		sdb->p = sdb->p + 1;
		sdb->k = sdb->k + 1;
	}
	sdb->k = 0;
	while (sdb->k < sdb->vlen)
	{
		sdb->new_input[sdb->p] = sdb->last_env[sdb->k];
		sdb->new_type[sdb->p] = '5';
		sdb->new_num[sdb->p] = current->input_num[i];
		sdb->k = sdb->k + 1;
		sdb->p = sdb->p + 1;
	}
}

void	cy3_handle_dollar_bang2(t_dollar_bang *sdb,
								t_input *current, int i, int j)
{
	sdb->vlen = cy_strlen(sdb->last_env);
	sdb->lold = cy_strlen(current->input);
	sdb->replaced_len = j - i + 1;
	sdb->new_input = malloc(sdb->lold - sdb->replaced_len + sdb->vlen + 1);
	sdb->new_type = malloc(sdb->lold - sdb->replaced_len + sdb->vlen + 1);
	sdb->new_num = malloc(sdb->lold - sdb->replaced_len + sdb->vlen + 1);
}

int	cy3_handle_dollar_bang1(t_dollar_bang *sdb, char **env)
{
	sdb->last_env = NULL;
	sdb->e = 0;
	while (env[sdb->e])
	{
		if (!env[sdb->e + 1])
		{
			sdb->last_env = env[sdb->e];
			break ;
		}
		sdb->e = sdb->e + 1;
	}
	if (!sdb->last_env)
		return (0);
	return (1);
}

int	cy3_handle_dollar_bang(t_input *current, int i, int j, char **env)
{
	t_dollar_bang	sdb;

	if (!cy3_handle_dollar_bang1(&sdb, env))
		return (-2);
	cy3_handle_dollar_bang2(&sdb, current, i, j);
	if (!sdb.new_input || !sdb.new_type || !sdb.new_num)
		return (-1);
	cy3_handle_dollar_bang3(&sdb, current, i);
	cy3_handle_dollar_bang5(&sdb, current, j);
	sdb.new_input[sdb.p] = '\0';
	sdb.new_type[sdb.p] = '\0';
	sdb.new_num[sdb.p] = '\0';
	free(current->input);
	free(current->input_type);
	free(current->input_num);
	current->input = sdb.new_input;
	current->input_type = sdb.new_type;
	current->input_num = sdb.new_num;
	return (i - 1 + sdb.vlen);
}
