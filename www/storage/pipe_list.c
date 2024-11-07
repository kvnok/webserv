/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   pipe_list.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <marvin@42.fr>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 17:34:23 by jvorstma      #+#    #+#                 */
/*   Updated: 2023/11/13 10:25:39 by ibehluli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_count_w_red(char *cmd, int *w)
{
	int	i;

	i = -1;
	while (cmd && cmd[++i])
	{
		if (cmd[i] == '"' || cmd[i] == '\'')
		{
			i = ft_f_next_quote(cmd, cmd[i], i);
			if (i == -2)
				return (1);
		}
		else if (cmd[i] == '<' || cmd[i] == '>')
		{
			if (i > 0 && cmd[i -1] && !ft_strchr(" 	<>", cmd[i - 1]))
				(*w)++;
			while (cmd[i] && cmd[i] == cmd[i + 1]
				&& (cmd[i] == '<' || cmd[i] == '>'))
				i++;
			if (cmd[i + 1] && cmd[i + 1] != ' ' && cmd[i + 1] != '\t')
				(*w)++;
		}
	}
	return (0);
}

static int	ft_count_w_space(char *cmd, int	*w)
{
	int	i;

	i = 0;
	while (cmd && cmd[i])
	{
		if (cmd[i] && cmd[i] != ' ' && cmd[i] != '\t')
		{
			(*w)++;
			while (cmd[i] && cmd[i] != ' ' && cmd[i] != '\t')
			{
				if (cmd[i] == '"' || cmd[i] == '\'')
					i = ft_f_next_quote(cmd, cmd[i], i);
				else if (cmd[i] == '$' && cmd[i + 1] && cmd[i + 1] == '{')
					i = ft_get_var_brac(cmd, i);
				if (i == -2)
					return (1);
				i++;
			}
		}
		while (cmd[i] && (cmd[i] == ' ' || cmd[i] == '\t'))
			i++;
	}
	return (0);
}

static char	*ft_get_word(char *s, int *j, int b)
{
	if (!s)
		return (NULL);
	while (s[*j] && (s[*j] == ' ' || s[*j] == '\t'))
		(*j)++;
	b = *j;
	if (s[*j] == '<' || s[*j] == '>')
	{
		while (s[*j] && s[*j] == s[b])
			(*j)++;
		return (ft_substr(s, b, (*j - b)));
	}
	while (s[*j])
	{
		if (s[*j] == '"' || s[*j] == '\'')
			*j = ft_f_next_quote(s, s[*j], *j);
		else if (s[*j] == '<' || s[*j] == '>')
			return (ft_substr(s, b, (*j - b)));
		else if (s[*j] == '$' && s[*j + 1] && s[*j + 1] == '{')
			*j = ft_get_var_brac(s, *j);
		else if (s[*j] == ' ' || s[*j] == '\t')
			return (ft_substr(s, b, (*j - b)));
		if ((*j)++ == -2)
			return (NULL);
	}
	return (ft_substr(s, b, (*j - b)));
}

static char	**ft_input_split_words(char *cmd)
{
	int		i;
	int		j;
	int		w;
	char	**str;

	if (!cmd)
		return (NULL);
	w = 0;
	if (ft_count_w_space(cmd, &w) || ft_count_w_red(cmd, &w))
		return (NULL);
	str = (char **)malloc((w + 1) * sizeof(char *));
	if (!str)
		return (NULL);
	i = 0;
	j = 0;
	while (i < w && cmd[j])
	{
		str[i] = ft_get_word(cmd, &j, j);
		if (!str[i])
			return (ft_free_double_array(str), NULL);
		i++;
	}
	str[i] = NULL;
	return (str);
}

void	ft_make_pipe_list(char *buf, t_main *main, int p)
{
	t_tokens	*new_token;
	t_tokens	*current;

	if (!main)
		return ;
	new_token = (t_tokens *)malloc(sizeof(t_tokens));
	if (!new_token)
		return ;
	new_token->input_pipe = ft_strtrim(buf, " 	");
	if (p == main->num_p)
		new_token->redirection = ft_strdup("end");
	else
		new_token->redirection = ft_strdup("|");
	new_token->parameters = ft_input_split_words(buf);
	new_token->file = NULL;
	new_token->next_token = NULL;
	if (!main->token)
		main->token = new_token;
	else
	{
		current = main->token;
		while (current->next_token)
			current = current->next_token;
		current->next_token = new_token;
	}
}
