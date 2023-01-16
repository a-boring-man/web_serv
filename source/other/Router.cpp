/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/05 11:05:55 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/10 16:21:05 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

using std::cout;
using std::cerr;
using std::endl;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Router::Router( Config const &conf, Request const &header, int port, std::string ext ) : _conf(conf), _header(header), _port(port), _ext(ext) {

	choose();
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Router const & i )
{
	o << "Config : -" << i.getConfig() << "-" << endl << "Header : -"
	<< i.getRequest() << "-" << endl << "Location : -"
	<< i.getLocation() << "-" << endl << "ConfigServer : -"
	<< i.getConfigserver() << "-" << endl;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** --------------------------------- CHOOSE ----------------------------------
*/

void	Router::choose() {

	setConfigserver();
	setLocation();
	return;
}

/*
** --------------------------------- SET_CONFIG_SERVER ----------------------------------
*/

void	Router::setConfigserver() {

	std::vector<ConfigServer>	selection;
	add_matching_port( selection );
	if (selection.size() == 1)
	{
		_configserver = selection[0];
		return;
	}
	erase_nonmatching_servername( selection );
	_configserver = selection[0];
	return;
}

/*
** --------------------------------- ADD_MATCHING_PORT ----------------------------------
*/

void	Router::add_matching_port( std::vector<ConfigServer> &portselection ) {

	//ConfigServer	*tmp;
	for (unsigned long i = 0; i < _conf.getList().size(); i++)
	{
		if (_conf.getList()[i].getPort() == _port)
		{
			//tmp = _conf.getList()[i];
			portselection.push_back(_conf.getList()[i]);
		}
	}
	return;
}

/*
** --------------------------------- ERASE_NON_MATCHING_SERVERNAME ----------------------------------
*/

void	Router::erase_nonmatching_servername( std::vector<ConfigServer> &portselection ) {

	std::vector<ConfigServer>	tmp;
	for (unsigned long i = 0; i < portselection.size(); i++)
	{
		if (portselection[i]._server_names_has_been_set)
		{
			int	s = 0;
			for (unsigned long j = 0; j < portselection[i].getServername().size(); j++)
				if (is_there_this_header_option("Host") && !portselection[i].getServername()[j].compare(_header.getHost()))
					s++;
			if (s)
			{
				tmp.push_back(portselection[i]);
				break;
			}
		}
	}
	if (!tmp.size())
	{
		for (unsigned long i = 0; i < portselection.size(); i++)
			if (i)
				portselection.erase(portselection.begin() + i);
		return;
	}
	else
	{
		portselection.clear();
		portselection.push_back(tmp[0]);
		return;
	}
	return;
}

/*
** --------------------------------- SET_LOCATION ----------------------------------
*/

void	Router::setLocation() {

	if (_configserver.getLocation().size() == 1)
	{
		_location = _configserver.getLocation()[0];
		return;
	}
	for (unsigned long i = 0; i < _configserver.getLocation().size(); i++)
	{
		//cerr << "------------------boucle du match extention-----------" << endl;
		if (!_configserver.getLocation()[i].getType().compare(_ext)) // check if CGI can be applied ?
		{
			//cerr << "------------------if du match extention-----------" << endl;
			_location = _configserver.getLocation()[i];
			return;
		}
	}
	for (unsigned long i = 0; i < _configserver.getLocation().size(); i++)
	{
		//cerr << "------------------boucle du match parfait-----------" << _configserver.getLocation().size() << endl;
		if (!_configserver.getLocation()[i].getType().compare(_header.getPath())) // check for perfect match ?
		{
			//cerr << "------------------if du match parfait-----------" << endl;
			_location = _configserver.getLocation()[i];
			return;
		}
	}
	unsigned long	j = 0;
	unsigned long	diff = 0;
	//cerr << "------------------just avant-----------" << endl;
	for (unsigned long i = _configserver.getLocation().size(); i > 0; i--) // check for the longest match
	{
		//cerr << "------------------boucle du match imparfait-----------" << endl;
		if (_header.getPath().find(_configserver.getLocation()[i - 1].getType()) != _header.getPath().npos && diff < _header.getPath().npos - _header.getPath().find(_configserver.getLocation()[i - 1].getType()))
		{
			//cerr << "------------------if du match imparfait-----------" << endl;
			diff = _header.getPath().npos - _header.getPath().find(_configserver.getLocation()[i - 1].getType());
			j = i - 1;
		}
	}
	_location = _configserver.getLocation()[j];
	return;
}

/*
** --------------------------------- IS_THERE_THIS_HEADER_OPTION ----------------------------------
*/

bool	Router::is_there_this_header_option(std::string ho) const {

	for (unsigned long i = 0; i < _header.getHeaderOptionSet().size(); i++)
		if (!_header.getHeaderOptionSet()[i].compare(ho))
			return (1);
	return (0);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

Location const &Router::getLocation()	const {

	return (_location);
}

ConfigServer const &Router::getConfigserver()	const {

	return (_configserver);
}

Config const	&Router::getConfig()	const {

	return (_conf);
}

Request const	&Router::getRequest()	const {

	return (_header);
}

int	Router::getPort() const {

	return (_port);
}

/* ************************************************************************** */