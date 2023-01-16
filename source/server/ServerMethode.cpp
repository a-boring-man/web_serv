/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMethode.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 13:41:31 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 12:58:42 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <unistd.h> // pour close
#include <fcntl.h> // pour fcntl
#include <arpa/inet.h> // pour les htons
#include "utils.hpp" // pour le memset de la struct du server

using std::cout;
using std::string;
using std::endl;

/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** ------------------------------ QUICK METHODS -------------------------------
*/

/*
** --------------------------------- FT_SOCKET ----------------------------------
*/

void	Server::ft_socket( int family, int socket_type, int protocol ) {

	int	tmp = socket(family, socket_type, protocol);
	if (tmp < 0)
		throw Server::Exeption();
	_serversocket = tmp;
}

/*
** --------------------------------- FT_SETSOCKETOPT ----------------------------------
*/

void	Server::ft_setsockopt( void ) {

	int	sockoptenabler = 1;
	int	tmp = setsockopt(_serversocket, SOL_SOCKET, SO_REUSEADDR, (char *)&sockoptenabler, sizeof(sockoptenabler));
	if (tmp < 0)
	{
		close(_serversocket);
		throw Server::Exeption();
	}
}

/*
** --------------------------------- FT_FCNTL ----------------------------------
*/

void	Server::ft_fcntl( int socket_fd, int command, int arg ) {

	int	tmp = fcntl(socket_fd, command, arg);
	if (tmp < 0)
	{
		close(_serversocket);
		throw Server::Exeption();
	}
}

/*
** --------------------------------- INIT_STRUCT_ADDR_SERV ----------------------------------
*/

void	Server::init_structaddrserv( void ) { // function a modifier pour adress ip avec les info du parsing

	ft_memset(&_structaddrserv, 0, sizeof(_structaddrserv));
	_structaddrserv.sin_family = AF_INET;
	_structaddrserv.sin_port = htons(_port);
	_structaddrserv.sin_addr.s_addr = inet_addr(_ip.c_str());
	//_structaddrserv.sin_addr.s_addr = INADDR_ANY;
}

/*
** --------------------------------- FT_BIND ----------------------------------
*/

void	Server::ft_bind( struct sockaddr *addr, int size_of_addr_struct ) {

	int	tmp = bind(_serversocket, addr, size_of_addr_struct);
	if (tmp < 0)
	{
		close(_serversocket);
		throw Server::Exeption();
	}
}

/*
** --------------------------------- FT_LISTEN ----------------------------------
*/

void	Server::ft_listen( int socket_fd, int max_number_of_pending_connection ) {

	int	tmp = listen(socket_fd, max_number_of_pending_connection);
	if (tmp < 0)
	{
		close(_serversocket);
		throw Server::Exeption();
	}
}

/*
** --------------------------------- LAUNCH ----------------------------------
*/

int	Server::launch( const int port, const string ip ) {

	setPort(port);
	setIp(ip);
	ft_socket(AF_INET, SOCK_STREAM, 0);
	ft_setsockopt();
	ft_fcntl(_serversocket, F_SETFL, O_NONBLOCK);
	init_structaddrserv();
	int	tmp = bind(_serversocket, (struct sockaddr *)&_structaddrserv, sizeof(_structaddrserv));
	if (tmp < 0)
	{
		cout << "server socket problem : " << _serversocket << endl;
		close(_serversocket);
		throw Server::Exeption();
	}
	else
	{
		ft_listen( _serversocket, MAX_PENDING_CLIENT );
		return (_serversocket);
	}
}
