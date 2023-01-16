/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiplex_serv.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 12:30:39 by jrinna            #+#    #+#             */
/*   Updated: 2022/11/15 10:11:25 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int	main( int ac, char **av ) {

	if (ac != 2)
		return (0);
	int	new_sd = -1;
	int	nfds = 1;
	int	current_size = 0;
	int	i;
	int	close_conn;
	char	buffer[80];
	int	len;
	int	compress_array = 0;
	int	j;
	/**
	 * @brief server socket
	 * 
	 */
	int	listen_sd = -1;
	int	timeout;
	int	rc = 1;
	int	on = 1;
	struct sockaddr_in	addr;
	/**
	 * @brief tableau de struct pollfd
	 * 
	 */
	struct pollfd	fds[200];
	int	end_server = 0;

	bzero(fds, sizeof(fds));
	fds[0].fd = listen_sd;
	fds[0].events = POLLIN;
	timeout = (1000 * 30);
	do
	{
		printf("Waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);
		if (rc < 0)
		{
			perror("poll() failed");
			break;
		}
		if (rc == 0)
		{
			printf("poll() timed out. End program.\n");
			break;
		}

		current_size = nfds;
		for (i = 0; i < current_size; i++)
		{
			if(fds[i].revents == 0)
				continue;
			if(fds[i].revents != POLLIN)
			{
				if (fds[i].revents != 17)
				{
					printf("Error! revents = %d\n", fds[i].revents);
					end_server = 1;
					break;
				}
			}
			if (fds[i].fd == listen_sd)
			{
				printf("Listening socket is readable\n");
				do
				{
					new_sd = accept(listen_sd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK) // a testeter avec le nombre max de client
						{
							perror("accept() failed");
							end_server = 1;
						}
						break;
					}
					printf("  New incoming connection - %d\n", new_sd);
					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;
				} while (new_sd != -1);
			}
			else
			{
			printf("  Descriptor %d is readable\n", fds[i].fd);
			close_conn = 0;
				do
				{
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("recv() failed");
							close_conn = 1;
						}
						break;
					}
					if (rc == 0)
					{
						printf("Connection closed\n");
						close_conn = 1;
						break;
					}
					len = rc;
					printf("%d bytes received\n", len);
					rc = send(fds[i].fd, buffer, len, 0);
					if (rc < 0)
					{
						perror("send() failed");
						close_conn = 1;
						break;
					}

				} while(1);
				if (close_conn)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = 1;
				}


			}
		}
		if (compress_array)
		{
			compress_array = 0;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for(j = i; j < nfds; j++)
					{
						fds[j].fd = fds[j+1].fd;
					}
					i--;
					nfds--;
				}
			}
		}

	} while (end_server == 0);
	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}
	