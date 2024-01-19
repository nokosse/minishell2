/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kvisouth <kvisouth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 16:32:39 by kvisouth          #+#    #+#             */
/*   Updated: 2024/01/19 11:21:38 by kvisouth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// Exécute une commande individuelle.
int	exec_cmd( t_mini *shell, t_cmd *cmd)
{
	int		i;
	int		j;
	int		k;
	char	*ret;
	char	**f;

	j = 0;
	i = -1;
	f = ft_split(cmd->str, ' ');
	f[0] = ft_strjoin("/",f[0]);
	while (shell->env[++i] != NULL && shell->env)
	{
		ret = ft_substr(shell->env[i], 0, 5);
		if (ft_same_str(ret, "PATH=", 5) == 1)
		{
			j = 5;

			while (shell->env[i][j] && shell->env[i][++j] != '\0' && shell->env)
			{
				k = j;
				while (shell->env[i][j] != ':' && shell->env[i][j] && shell->env)
					j++;
				ret = ft_strjoin_free1(ft_substr(shell->env[i], k, ((j) - k)), f[0]);

				if (shell->env[i][j] == ':')
					if (access(ret, F_OK) == 0)
						execve(ret, f, shell->env);

			}
		}
	}
	return 0;
}

char	*ft_strjoin_free1(char const *s1, char const *s2)
{
	char	*dest;
	size_t	len;
	int		i;
	int		j;
	int		k;

	i = 0;
	j = 0;
	k = 0;
	len = (ft_strlen(s1) + ft_strlen(s2));
	dest = malloc(len + 1 * sizeof(char));
	if (!dest)
		return (NULL);
	while (s1 && s1[i] && dest)
		dest[k++] = s1[i++];
	while (s2 && s2[j] && dest)
		dest[k++] = s2[j++];
	dest[k] = '\0';
	free((void *)s1);
	return (dest);
}

int	ft_same_str(char *str1, char *str2, size_t n)
{
	int	i;

	i = -1;
	while ((++i < (int)n) && str1 && str2)
	{
		if (str1[i] != str2[i])
		{
			free(str1);
			return (0);
		}
	}
	free(str1);
	return (1);
}


int executor(t_mini *shell)
{
    int pipe_fds[2 * shell->nb_pipes];
    int i = 0;
	pid_t pid;

    // Initialisation des pipes si nécessaire
    for (i = 0; i < shell->nb_pipes; i++) {
        if (pipe(pipe_fds + i * 2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    t_cmd *current_cmd = shell->cmd;
    i = 0;
    while (current_cmd != NULL) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Processus enfant
            if (i != 0) { // Si ce n'est pas la première commande
                dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < shell->nb_pipes * 2) { // Si ce n'est pas la dernière commande
                dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO);
            }

            // Fermeture des descripteurs de fichier de pipe dans le processus enfant
            for (int j = 0; j < 2 * shell->nb_pipes; j++) {
                close(pipe_fds[j]);
            }

            exec_cmd(shell, current_cmd);
            exit(EXIT_FAILURE); // Si exec_cmd retourne
        }

        current_cmd = current_cmd->next;
        i++;
    }

    // Fermeture des descripteurs de fichier de pipe dans le processus parent
    for (i = 0; i < 2 * shell->nb_pipes; i++) {
        close(pipe_fds[i]);
    }

    // Attente des processus enfants
    while ((pid = wait(NULL)) > 0);

    return 1;
}

// int	executor(t_mini *shell)
// {
// 	int pid;
// 	pid = fork();
// 	if (shell->nb_pipes == 0)
// 	{
		
// 		if (pid == 0)
// 		{
// 			if (!exec_cmd(shell, shell->cmd))
// 			{
// 				perror("execve");
// 				exit(EXIT_FAILURE);
// 			}
// 		}
// 	}

// 	else
// 	{
// 		t_cmd *current_cmd = shell->cmd;
// 		while (current_cmd != NULL)
// 		{
// 			pid_t pid = fork();
// 			if (pid == -1)
// 			{
// 				// Gestion d'erreur de fork
// 				perror("fork");
// 			}
// 			else if (pid == 0)
// 			{
// 				// Processus enfant
// 				if (!exec_cmd(shell, current_cmd))
// 				{
// 					perror("execve");
// 					exit(EXIT_FAILURE);
// 				}
// 			}
// 			else
// 			{
// 				// Processus parent
// 				int status;
// 				waitpid(pid, &status, 0);
// 				// Vous pouvez traiter le statut de sortie ici si nécessaire
// 			}

// 			current_cmd = current_cmd->next;
// 		}
		
// 	}
// 	return 1;
// }
