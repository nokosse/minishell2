/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_ins_spaces.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kvisouth <kvisouth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 18:12:35 by kvisouth          #+#    #+#             */
/*   Updated: 2024/01/19 16:15:42 by kvisouth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

/*
Skip from the actual double quote to the next one. (It's built for 'count_len')
Increment i.
*/
void	skip_dq_count_len(char *str, int *i, int *nb)
{
	int	j;

	j = 0;
	if (str[*i] == '\"')
	{
		j = *i;
		j++;
		while (str[j] && str[j] != '\"')
			j++;
		if (str[j] == '\"')
		{
			*i = j;
			(*nb)++;
		}
	}
}

/*
Skip from the actual simple quote to the next one. (It's built for 'count_len')
Increment i.
*/
void	skip_sq_count_len(char *str, int *i, int *nb)
{
	int	j;

	j = 0;
	if (str[*i] == '\'')
	{
		j = *i;
		j++;
		while (str[j] && str[j] != '\'')
			j++;
		if (str[j] == '\'')
		{
			*i = j;
			(*nb)++;
		}
	}
}

/*
Used to count the number of characters in the new command line to malloc it.
*/
int	count_len(char *str)
{
	int	i;
	int	nb;

	i = 0;
	nb = 0;
	if (!str)
		return (0);
	while (str[i])
	{
		skip_dq_count_len(str, &i, &nb);
		skip_sq_count_len(str, &i, &nb);
		if (str[i] == '<' || str[i] == '>' || str[i] == '|')
		{
			if ((i > 0) && (str[i - 1] != ' ' && str[i - 1] != '<'
					&& str[i - 1] != '>' && str[i - 1] != '|'))
				nb++;
			if (str[i + 1] != ' ' && str[i + 1] != '<'
				&& str[i + 1] != '>')
				nb++;
		}
		i++;
	}
	return (nb + ft_strlen(str));
}

/*
This function will insert spaces where it's needed.
It will insert in two cases :
1. if the char BEFORE the current is NOT a SPACE, LEFT1, RIGHT1 or PIPE
2. if the CURRENT char is NOT a SPACE, LEFT1, RIGHT1 or PIPE 
*/
void	handle_spaces(int *i, int *j, char **new, char *cmd)
{
	if ((*i > 0) && (cmd[*i - 1] != ' ' && cmd[*i - 1] != '<'
			&& cmd[*i - 1] != '>' && cmd[*i - 1] != '|'))
	{
		(*new)[*j] = ' ';
		(*j)++;
	}
	(*new)[*j] = cmd[*i];
	(*j)++;
	(*i)++;
	if (cmd[*i] != ' ' && cmd[*i] != '<' && cmd[*i] != '>')
	{
		(*new)[*j] = ' ';
		(*j)++;
	}
}

/*
This function will insert spaces between the tokens.
To make 'ls>out' become 'ls > out' for example.
*/
int	insert_spaces(t_mini *shell)
{
	char	*new;
	char	*cmd;
	int		i;
	int		j;

	i = 0;
	j = 0;
	cmd = shell->cmdline;
	new = ft_calloc(count_len(cmd) + 1, sizeof(char));
	if (!new)
		return (0);
	while (cmd[i])
	{
		skip_double_quotes_2strings(&cmd, &new, &i, &j);
		skip_simple_quotes_2strings(&cmd, &new, &i, &j);
		if (cmd[i] == '<' || cmd[i] == '>' || cmd[i] == '|')
			handle_spaces(&i, &j, &new, cmd);
		else
			new[j++] = cmd[i++];
		if (i >= (int)ft_strlen(cmd))
			break ;
	}
	free(cmd);
	shell->cmdline = new;
	return (1);
}
