/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/02 15:21:19 by ccartet           #+#    #+#             */
/*   Updated: 2023/01/11 11:49:43 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */


#include "Client.hpp"
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

using std::cout;
using std::cerr;
using std::endl;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client( int socket_fd, Config & config, int port, char **env ) : 

	_env( env ),
	_env_malloc( NULL ),
    _socket( socket_fd ),
	_serverPort( port ),
    _config( config ),
	_fullPath( "" ),
	_body( "" ),
	_fileType( "" ),
	_response( "" ),
	_toSend( 0 ),
    _responseReady( false ) {

	gettimeofday(&_start_time, NULL);
	initMapError();
	initMapType();
}

Client::Client( Client const & other ) : 

	_env( other._env ),
	_env_malloc( NULL ),
	_socket( other._socket ),
	_serverPort( other._serverPort),
    _config( other._config ),
	_fullPath( "" ),
	_body( "" ),
	_fileType( "" ),
	_response( "" ),
	_toSend( 0 ),
    _responseReady( false ) {

	gettimeofday(&_start_time, NULL);
	initMapError();
	initMapType();
	*this = other;
}

Client::~Client() {

	if (_env_malloc)
	{
		for (size_t i = 0; _env_malloc[i]; i++)
			delete _env_malloc[i];
		delete _env_malloc;
	}
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &	Client::operator=( Client const & rhs )
{
	if ( this != &rhs )
	{
		_start_time.tv_sec=rhs._start_time.tv_sec;
		_start_time.tv_usec=rhs._start_time.tv_usec;
		_socket = rhs._socket;
		_serverPort = rhs._serverPort;
		_request = rhs._request;
		_config = rhs._config;
		_configServer = rhs._configServer;
		_location = rhs._location;
		_statusCode = rhs._statusCode;
		_fullPath = rhs._fullPath;
		_body = rhs._body;
		_fileType = rhs._fileType;
		_response = rhs._response;
		_toSend = rhs._toSend;
		_responseReady = rhs._responseReady;
		_env = rhs._env;
		if (_env_malloc)
		{
			for (size_t i = 0; _env_malloc[i]; i++)
			{
				delete _env_malloc[i];
			}
			delete _env_malloc;
			_env_malloc = NULL;
			if (rhs._env_malloc)
			{
				unsigned long	size = 0;
				for (size = 0; rhs._env_malloc[size]; size++){}//
				char	**env = new char*[size + 1];
				env[size] = NULL;
				for (unsigned long i = 0; i < size; i++)
				{
					int	small_size = 0;
					for (small_size = 0; rhs._env_malloc[i][small_size]; small_size++){}//
					env[i] = new char[small_size + 1];
					std::strcpy(env[i], rhs._env_malloc[i]);//
				}
				_env_malloc = env;
			}
		}
	}
	return *this;
}

/*
** ----------------------------- ERROR PAGE CONSTRUCTION ------------------------------
*/

void	Client::construct_error_page( string s ) { // construit une error page avec l'error dans string s

	//cerr << "constructing error page" << s << endl;
	_body += "<div id=\"main\">";
	_body += "	<div class=\"fof\">";
	_body += "		<h1>Error ";
	_body += s;
	_body += "</h1>";
	_body += "	</div>";
	_body += "</div>";
	_fileType = "text/html";
}

/*
** ---------------------------- DISPATCH METHOD -----------------------------
*/

void	Client::dispatchMethod( std::string path )
{
	std::string	meth = _request.getMethod();
	std::string	fullPath = "";

	if (checkMethod(meth) == false)
		throw std::runtime_error("405");
	if (_location.root_has_been_set)
	{
		unsigned long	test = path.find('/', 1);
		//cerr << "le path : -" << path << "-" << "type -" << _location.getType() << "-\n";
		if (test != std::string::npos)
			path.replace(0, test, std::string(""));
		else if (path.compare(_location.getType()) == 0)
			path.replace(0, _location.getType().size(), std::string(""));
		//std::string tmp = path.replace(0, _location.getType().size(), std::string(""));
		fullPath = _location.getRoot() + path;
		//cerr << "fulle path : -" << fullPath << "-\n";
	}
	else 
		fullPath = _configServer.getRoot() + path;
	//cerr << "FullPath-" << fullPath << "-" << endl;
	if (meth == "GET")
		methodGet(fullPath);
	else if (meth == "DELETE")
		methodDelete(fullPath);
	else
		methodPost(fullPath);
}

/*
** ---------------------------- CHECK -----------------------------
*/

bool	Client::checkMethod( std::string const & method ) const
{
	std::vector<std::string>    methAllowed = _location.getMethode();
    std::vector<std::string>::iterator	ite = methAllowed.end();
	std::vector<std::string>::iterator	itb;
	
    for (itb = methAllowed.begin(); itb != ite; itb++)
    	if (method == *itb)
			break;
	if (itb == ite)
		return false;
	return true;
}

bool	Client::checkPath( std::string const & fullPath ) const // return true if the path is a directory that can be acces, throw an error if not accesible or false if it's a file
{
	struct stat	buf;
	int			ret = 0;

	ret = stat(fullPath.c_str(), &buf);
	if (ret == -1 && errno == EACCES) // no access
		throw std::runtime_error("403");
	if (ret == -1 && errno == ENOENT) // not found
	{
		//std::cerr << "ERROR: checkPath" << fullPath << std::endl;
		throw std::runtime_error("404");
	}
	if (ret != -1 && S_ISDIR(buf.st_mode)) // is a directory
		return true;
	return false; // is not a directory
}

bool	Client::isDirectory( std::string const & fullPath ) const {
	
	struct stat	buf;
	int			ret = 0;

	ret = stat(fullPath.c_str(), &buf);
	if (ret != -1 && S_ISDIR(buf.st_mode)) // is a directory
		return true;
	return false; // is not a directory
}

/*
** ---------------------------- UTILS -----------------------------
*/

bool	Client::file_exist( string s ) {

	return ( access( s.c_str(), F_OK ) != -1 );
}

std::string const &	Client::getMessageStatus( void ) const
{
	return _errors.at(_statusCode);
}

void	Client::updateResponse( int dataLen )
{
	_response.erase(0, dataLen);
	_toSend -= dataLen;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int  Client::getSocket( void ) const
{
    return _socket;
}

int  Client::getServerport( void ) const
{
    return _serverPort;
}

Request &    Client::getRequest( void )
{
    return _request;
}

Location const &	Client::getLocation( void ) const
{
	return _location;
}

std::string const &	Client::getStatusCode( void ) const
{
	return _statusCode;
}

std::string const &	Client::getFullPath( void ) const
{
	return _fullPath;
}
std::string const &	Client::getBody( void ) const
{
	return _body;
}
std::string const &	Client::getFileType( void ) const
{
	return _fileType;
}

std::string const &	Client::getResponse( void ) const
{
	return _response;
}

long	Client::getToSend( void ) const
{
	return _toSend;
}

bool    Client::getReady( void ) const
{
    return _responseReady;
}