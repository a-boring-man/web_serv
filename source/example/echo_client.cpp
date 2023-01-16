/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_client.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/02 11:43:12 by jrinna            #+#    #+#             */
/*   Updated: 2022/12/16 11:24:30 by jrinna           ###   ########lyon.fr   */
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
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#define BUFFER	1024

int	main(int ac, char **av) {

	(void)ac;
	struct sockaddr_in	remote_server;
	int					sock;
	char				input[BUFFER];
	char				output[BUFFER];
	int					len;

	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		printf("socket problem\n");
		exit(-1);
	}
	printf("socked created\n");
	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(atoi(av[2]));
	remote_server.sin_addr.s_addr = inet_addr(av[1]);
	bzero(&remote_server.sin_zero, 8);

	if ( (connect(sock, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in))) == -1 )
	{
		printf("connect problem\n");
		exit(-1);
	}
	printf("connection done\n");

		dprintf(1, "test1\n");int trtertert;
	while (1)
	{
		dprintf(1, "avant input\n");
		std::ifstream f(av[3]); //taking file as inputstream
		std::string str;
		if(f) {
			std::ostringstream ss;
			ss << f.rdbuf(); // reading data
			str = ss.str();
		}
		//fgets(input, BUFFER, stdin);
		dprintf(1, "apres input/ avant send: -%s-\n", str.c_str());
		trtertert = send(sock, str.c_str(), str.size(), 0);
		dprintf(1, "apres send/ avant recv %d\n", trtertert);

		len =  recv(sock, output, BUFFER, 0);
		dprintf(1, "apres recv %d\n", len);
		output[len] = '\0';
		dprintf(1, "-%s-\n", output);
	}
	close(sock);
}