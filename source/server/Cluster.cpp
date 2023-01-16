/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 10:53:24 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/13 14:15:10 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"
#include <unistd.h>
#include "utils.hpp"
#include <sys/time.h>

using std::cout;
using std::cerr;
using std::endl;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Cluster::Cluster( char *file_path, char **env ) : _serverhastimeout(0) {

	LjohnSon	node = LjohnSon();
	std::string	tmp;

	this->env = env;
	_file.open(file_path); // open le fichier de config
	if (_file.is_open())
	{
		while (std::getline(_file, tmp)) // goobe le fichier dans une string
			_file_string.append(tmp);
		_file.close();
		node.parse( _file_string ); // fou le fichier dans le node
		_config.configure(node); // transforme le node en config lisible
	}
	else // pareil dans le cas ou le fichier de config n'as pas été trouvé
	{
		cout << "no file were found at : " << file_path << " switching to default config file" << endl;
		_file.open("./config/default_config/default.json");
		if (_file.is_open())
		{
			while (std::getline(_file, tmp))
				_file_string.append(tmp);
			_file.close();
			node.parse( _file_string );
			_config.configure(node);
		}
		else
		{
			cout << "default config file not found, the server cannot be launch without config file, end program" << endl;
			throw end_program();
		}
	}
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** ------------------------------- INIT_LAUNCH --------------------------------
*/

void	Cluster::init_launch() {

	if (!_config.getList().size())
	{
		cout << "no valide server object were find in the config file, end of program" << endl;
		throw end_program();
	}
	int	s;
	for (unsigned long i = 0; i < _config.getList().size(); i++) // rajoute les serveurs qui ont des pairs adress ip/ port unique
	{
		if (_miniservlist.size())
		{
			s = 0;
			for (unsigned long j = 0; j < _miniservlist.size(); j++)
				if (!_config.getList()[i].getIp().compare(_miniservlist[j].getIp()) && _config.getList()[i].getPort() == _miniservlist[j].getPort())
					s++;
			if (!s)
			{
				_miniservlist.push_back(Server());
				unsigned long	index = _miniservlist.size();
				_serversocketlist.push_back(_miniservlist[index - 1].launch(_config.getList()[i].getPort(), _config.getList()[i].getIp()));
			}
		}
		else
		{
			_miniservlist.push_back(Server());
			_serversocketlist.push_back(_miniservlist[0].launch(_config.getList()[i].getPort(), _config.getList()[i].getIp()));
		}
	}
	if (!_miniservlist.size())
	{
		cout << "no valide server object were find in the config file, end of program" << endl;
		throw end_program();
	}
	cout << "launch init done" << endl;
	cout << "serveur found : " << _miniservlist.size() << endl;
}

/*
** --------------------------------- LAUNCH ----------------------------------
*/

void	Cluster::launch( void ) { // launch the message listening and responding loop

	init_paul();
	while (!_serverhastimeout)
	{
		paul();
		talk();
		kick_client();
	}
	clean_socket();
}

/*
** ------------------------------- KICK_CLIENT --------------------------------
*/

void	Cluster::kick_client() {

	struct	timeval now;
	for (unsigned long i = 0; i < _clientList.size(); i++)
	{
		gettimeofday(&now, NULL);
		if (its_been_too_long(now, _clientList[i]))
			deconnectSocket(_clientList[i].getSocket());
	}
	return;
}

/*
** ---------------------------- ITS_BEEN_TOO_LONG -----------------------------
*/

int	Cluster::its_been_too_long( struct timeval const & now, Client const & client ) {

	return ( (now.tv_sec - client._start_time.tv_sec) * 1000000 + (now.tv_usec - client._start_time.tv_sec) > 100000 );
}

/*
** -------------------------------- INIT PAUL ---------------------------------
*/

void	Cluster::init_paul( void ) { // initialization for poll

	unsigned long	i;
	struct pollfd	tmppollfd;
	for (i = 0; i < _serversocketlist.size(); i++)
	{
		ft_memset(&tmppollfd, 0, sizeof(tmppollfd));	
		tmppollfd.fd = _serversocketlist[i];
		tmppollfd.events = POLLIN;
		_socketlist.push_back(tmppollfd);
	}
}

/*
** ------------------------------ PAUL METHODS --------------------------------
*/

void	Cluster::paul( void ) { // launching paul

	int	tmp = poll(_socketlist.data(), _socketlist.size(), TIMEOUT);
	if (tmp < 0)
	{
		clean_socket();
		throw Cluster::Exeption();
	}
	if (!tmp)
	{
		cout << "paul has time out, shutdown server" << endl;
		_serverhastimeout = 1;
	}
}

/*
** ------------------------------ TALK METHODS --------------------------------
*/

void	Cluster::talk( void ) { // accept a client or exanche message
	
	for (unsigned long i = 0; i < _socketlist.size(); i++)
	{
		if (_socketlist[i].fd > 0 && _socketlist[i].revents)
		{
			if ( _socketlist[i].revents != POLLIN && _socketlist[i].revents != POLLOUT && _socketlist[i].revents != (POLLIN | POLLOUT) && _socketlist[i].revents != 17 && _socketlist[i].revents != 21)
			{
				_serverhastimeout = 1; // erase the socket;
				cerr << "error revents = " << _socketlist[i].revents << endl;
				break;
			}
			if (is_a_server_socket(_socketlist[i].fd) != -1) // a changer par une function qui dit si la socket appartient a _serversocketlist and return witch it is
			{
				ft_accept(is_a_server_socket(_socketlist[i].fd)); // add client
			}
			else if ((_socketlist[i].revents & POLLIN) == POLLIN)
			{
				ft_recv(i);
			}
			else if ((_socketlist[i].revents & POLLOUT) == POLLOUT)
				ft_send(i);
		}
	}
}

/*
** --------------------------- IS_A_SERVER_SOCKET ----------------------------
*/

long long	Cluster::is_a_server_socket( int fd ) { // check if the socket is a listener socket

	long long	answer = -1;
	for (unsigned long	i = 0; i < _serversocketlist.size(); i++)
		if (_serversocketlist[i] == fd)
			answer = i;
	return (answer);
}

/*
** ----------------------------- ACCEPT METHODS -------------------------------
*/

void	Cluster::ft_accept( long long pos ) {

	bool			client_denied = 0;
	struct pollfd	tmppollfd;
	int				tmpsocket;
	size_t			it;
	int				serverPort;

	do
	{
		tmpsocket = accept(_serversocketlist[pos], NULL, NULL); // use the return to accapt with the good server
		if (tmpsocket < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				client_denied = 1;
				cerr << "error on client accept call" << strerror(errno) << endl;
			}
			break;
		}
		if (!client_denied)
		{
			ft_memset(&tmppollfd, 0, sizeof(tmppollfd));	
			tmppollfd.fd = tmpsocket;
			tmppollfd.events = POLLIN | POLLOUT;
			_socketlist.push_back(tmppollfd);
			it = findServer(_serversocketlist[pos]);
			serverPort = _miniservlist[it].getPort();
			_clientList.push_back(Client(tmppollfd.fd, _config, serverPort, env));
		}
	} while (tmpsocket != -1);
}

/*
** ------------------------------ RECV METHODS --------------------------------
*/

void	Cluster::ft_recv( int fd_nbr ) {

	int		data_len = 0;
	size_t	it = 0;
	std::string	buf = "";

	for (size_t i = 0; i < 1024; i++)
		_tmprecvbuffer[i] = 0;
	data_len = recv(_socketlist[fd_nbr].fd, _tmprecvbuffer, sizeof(_tmprecvbuffer), 0);
	if (data_len <= 0) // si deconnection
	{
		deconnectSocket(fd_nbr);
		return;
	}
	buf.append(_tmprecvbuffer, data_len);
	cout << "message was : -" << buf  << "-" << endl;
	it = findClient(_socketlist[fd_nbr].fd);
	_clientList[it].getRequest().parse(buf);
}

/*
** ------------------------------- FIND_CLIENT --------------------------------
*/

size_t	Cluster::findClient( int toFind ) const
{
	for (size_t i = 0; i < _clientList.size(); i++)
		if (_clientList[i].getSocket() == toFind)
			return (i);
	return (-1);
}

/*
** ------------------------------- FIND_SERVER --------------------------------
*/

size_t	Cluster::findServer( int toFind ) const
{
	for (size_t i = 0; i < _miniservlist.size(); i++)
		if (_miniservlist[i].getSocket() == toFind)
			return (i);
	return (-1);
}

/*
** ----------------------------- DISCONNECT_SOCKET -----------------------------
*/

void	Cluster::deconnectSocket( int fd_nbr )
{
	_clientList.erase(_clientList.begin() + findClient(_socketlist[fd_nbr].fd)); // potentiellement un segfault ou exepection si pas de client
	close(_socketlist[fd_nbr].fd);
	_socketlist[fd_nbr].fd = -1;
	_socketlist.erase(_socketlist.begin() + fd_nbr);
	// std::cout << "recv: " << strerror(errno) << std::endl;
}

/*
** ----------------------------------- SEND ------------------------------------
*/

void	Cluster::ft_send( int fd_nbr ) {

	size_t	it = 0;
	int		data_len = 0;
	//cerr << "fd_nbr : " << fd_nbr << endl;
	it = findClient(_socketlist[fd_nbr].fd);
	if (_clientList[it].getRequest().getFinishParsing() == true)
	{	
		_clientList[it].createResponse();
		if (_clientList[it].getReady())
		{
			long	toSend = _clientList[it].getToSend();
			const char	*tmp = _clientList[it].getResponse().c_str();
			//cerr << "THE RESPONSE SEND IS : -" << tmp << "-" << endl;
			data_len = send(_socketlist[fd_nbr].fd, tmp, toSend, 0);
			if (toSend - data_len == 0)
			{
				_clientList[it].clear_client();
			}
			else
				_clientList[it].updateResponse(data_len);
		}
		if (data_len < 0) // si deconnection
			deconnectSocket(fd_nbr);
	}
}

/*
** ------------------------------- CLEAN METHODS --------------------------------
*/

void	Cluster::clean_socket( void ) {

	for(unsigned long i = 0; i < _socketlist.size(); i++)
	{
		if (_socketlist[i].fd >= 0)
			close(_socketlist[i].fd);
	}
	throw Cluster::end_program();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */