/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/01 15:33:45 by ccartet           #+#    #+#             */
/*   Updated: 2023/08/03 12:10:34 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"
# include "Config.hpp"
# include "ConfigServer.hpp"
# include "Location.hpp"
# include "Router.hpp"
# include <sys/types.h>
# include <dirent.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/time.h>
# include <string>
# include <string.h>
# include <unistd.h>

using std::string;

class Client {

	public:

		Client( int socket_fd, Config & config, int port, char **env );
		Client( Client const & other );
		~Client();

		Client &		operator=( Client const & rhs );

		void	createResponse( void );
		void	updateResponse( int dataLen );
		bool	file_exist( string s );
		void	construct_error_page( string s );
		void	ship_page( void );

		int					getSocket( void ) const;
		int					getServerport( void ) const;
		Request&			getRequest( void );
		Location const &	getLocation( void ) const;
		std::string const &	getStatusCode( void ) const;
		std::string const &	getFullPath( void ) const;
		std::string const &	getBody( void ) const;
		std::string const &	getFileType( void ) const;
		std::string const &	getResponse( void ) const;
		long				getToSend( void ) const;
		bool				getReady( void ) const;
		void				clear_client( void );

		struct timeval						_start_time;
		char								**_env;
		char								**_env_malloc;

	private:

		int									_socket; // socket du client
		int									_serverPort; // sockect du listenner qui a recus le client
		Request								_request; // request du client RESET
		Config								_config; // la config de base
		ConfigServer						_configServer; // sera set ou pas sur la config specifique du virtual server approprier RESET
		Location							_location; // pareille les deux si la request etais bien formatter RESET
		std::map<std::string, std::string>	_errors;
		std::map<std::string, std::string>	_types;
		std::string							_statusCode; // le status code a renvoyer RESET
		std::string							_fullPath; // root + uri RESET
		std::string							_body; // le body a renvoyer au client RESET
		std::string							_fileType; // type du contenu à renvoyer RESET
		std::string							_response; // le text a renvoyer au client donc header + body RESET
		long								_toSend; // la longueur du text a send RESET
		bool								_responseReady; // esque la reponse est prete a etre envoyer RESET

		int							getFile( std::string const & fullPath );
		int							toCreate( std::string const & fullPath, std::string const & body );
		bool						checkPath( std::string const & fullPath) const;
		bool						checkMethod( std::string const & method ) const;
		bool						isDirectory( std::string const & fullPath ) const;
		void						initMapType( void );
		void						initMapError( void );
		void						methodGet( std::string fullPath );
		void						dispatchMethod( std::string path );
		void						methodPost( std::string const & fullPath );
		void						methodDelete( std::string const & fullPath );
		void						generateIndex( std::string const & fullPath );
		void						map_to_env( std::map<int, std::string> envm );
		void						initiate_cgi_env( std::string const & fullPath );
		void						create_good_error_page( const std::runtime_error& e );
		std::string					setExtension( void ) const;
		std::string					call_cgi( std::string const & fullPath );
		std::string					do_the_cgi( std::string const & fullPath );
		std::string					setFileType( std::string const & filePath ) const;
		std::string const &			getMessageStatus( void ) const;
		std::map<int, std::string>	env_to_map() const;
		
};

#endif