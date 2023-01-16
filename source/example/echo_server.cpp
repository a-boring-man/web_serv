/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/02 10:21:24 by jrinna            #+#    #+#             */
/*   Updated: 2022/11/03 14:37:52 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define MAX_CLIENT	2
#define MAX_DATA	1024

int	main( int ac, char **av ) {

	(void)ac;
	struct	sockaddr_in server;
	struct	sockaddr_in	client;
	int					sock;
	int					client_sock;
	socklen_t			sockaddr_len = sizeof(struct sockaddr_in);
	int					data_len;
	char				data[MAX_DATA];

	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		printf("un problem de socket\n");
		exit(-1);
	}
	printf("socket created\n");
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(av[1]));
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 8);

	if ( bind(sock, (struct sockaddr *)&server, sockaddr_len) == -1 )
	{
		printf("un problem de bind\n");
		exit (-1);
	}
	printf("binding done\n");
	if ( listen(sock, MAX_CLIENT) == -1 )
	{
		printf("un problem de listen\n");
		exit(-1);
	}
	printf("listening\n");
	while (1)
	{
		if ( (client_sock = accept(sock, (struct sockaddr *)&client, &sockaddr_len)) == -1 )
		{
			printf("un problem d'accept");
			exit(-1);
		}
		printf("a new client just connected from port no : %d with the ip : %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));
		data_len = 1;

		while (data_len)
		{
			printf("starting reading loop\n");
			data_len = recv(client_sock, data, MAX_DATA, 0);
			if (data_len)
			{
				send(client_sock, data, data_len, 0);
				data[data_len] = '\0';
				printf("send message : -%s-\n", data);
			}
		}
		printf("client disconnected\n");
		close(client_sock);
	}
	
}