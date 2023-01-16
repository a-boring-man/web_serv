/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 15:42:51 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 10:35:06 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <netinet/in.h> // pour sockaddr struct
# include <iostream>

# define MAX_PENDING_CLIENT 200

using std::string;

class Server
{

	public:

		Server();

		class	Exeption : public std::exception {
			public:
				virtual const char* what() const throw(){return (strerror(errno));} 
		};

		int		getSocket( void ) const;
		int		const	&getPort()	const;
		int		launch( int port, string ip );
		void	ft_setsockopt( void );
		void	setPort(int const &i);
		void	setIp(string const &s);
		void	init_structaddrserv( void );
		void	ft_fcntl( int socket_fd, int command, int arg );
		void	ft_socket( int family, int socket_type, int protocol );
		void	ft_bind( struct sockaddr *addr, int size_of_addr_struct );
		void	ft_listen( int socket_fd, int max_number_of_pending_connection );
		string	const	&getIp()	const;
		
	private:

		string				_ip;
		int					_port;
		int					_serversocket;
		struct sockaddr_in	_structaddrserv;
		
};
