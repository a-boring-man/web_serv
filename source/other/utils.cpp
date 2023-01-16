/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 11:17:09 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/10 14:11:13 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

void	*ft_memset(void *block, int c, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		*(((unsigned char *)(block)) + i) = (unsigned char)c;
		i++;
	}
	return (block);
}

long	ft_min(long a, long b)
{
	if (a < b)
		return (a);
	return (b);
}
