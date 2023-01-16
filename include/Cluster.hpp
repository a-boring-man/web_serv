/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 10:53:33 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/13 13:45:07 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "LjohnSon.hpp" // pour le fichier de config
# include <fstream> // pour le fichier de config
# include "Server.hpp" // pour les miniserver
# include <vector> // pour les vector
# include <poll.h> // pour la struct pollfd
# include "Config.hpp"
# include "Client.hpp"

#define	TIMEOUT 2500*1000

class Cluster
{

	public:

		class	end_program : public std::exception {
			public:
				virtual const char* what() const throw(){return ("the end");} 
		};
		class	Exeption : public std::exception {
			public:
				virtual const char* what() const throw(){return (strerror(errno));} 
		};

		Cluster( char *file_path, char **env);

		void		init_launch();
		void		launch();
		void		init_paul( void );
		void		paul( void );
		void		talk( void );
		long long	is_a_server_socket( int fd );
		void		ft_accept( long long );
		void		ft_recv( int fd_nbr );
		void		ft_send( int fd_nbr );
		size_t		findClient( int toFind ) const;
		size_t		findServer( int toFind ) const;
		void		deconnectSocket( int fd_nbr );
		void		clean_socket( void );
		void		kick_client( void );
		int			its_been_too_long( struct timeval const & now, Client const & client );
		char		**env;

	private:

		char						_tmprecvbuffer[1024];
		bool						_serverhastimeout;
		Config						_config;
		std::string					_file_string;
		std::ifstream				_file;
		std::vector<int>			_serversocketlist;
		std::vector<Client>			_clientList;
		std::vector<Server>			_miniservlist;
		std::vector<struct pollfd>	_socketlist;

};

//std::ostream &			operator<<( std::ostream & o, Cluster const & i );
