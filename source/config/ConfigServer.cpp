/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 14:42:26 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 11:28:27 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"

using std::endl;
using std::string;
using std::cout;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConfigServer::ConfigServer() : _server_names_has_been_set(0), _error_page_has_been_set(0) {

	return;
}

ConfigServer::ConfigServer(LjohnSon node) : _server_names_has_been_set(0), _error_page_has_been_set(0), _imput(node), _ip("0.0.0.0"), _port(80), _root("./"), _clientmaxbodysize(10000000) {

	return;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, ConfigServer const & i )
{
	if (i._server_names_has_been_set)
		for (unsigned long j = 0; j < i.getServername().size(); j++)
			o << "Servername : " << j << " : " << i.getServername()[j] << endl;
	o << "Root : " << i.getRoot() << endl;
	std::map<string, string>::const_iterator	it;
	if (i._error_page_has_been_set)
		for (it = i.getErrorpage().begin(); it != i.getErrorpage().end(); it++)
			o << "key : " << it->first << ", " << "value : " << it->second << endl;
	o << "client max body size : " << i.getClientmaxbodysize() << endl;
	o << "ip : " << i.getIp() << endl;
	o << "port : " << i.getPort() << endl;
	for(unsigned long j = 0; j < i.getLocation().size(); j++)
		o << i.getLocation()[j] << endl;
	return o;
}

ConfigServer &				ConfigServer::operator=( ConfigServer const & rhs )
{
	if ( this != &rhs )
	{
		_imput = rhs.getImput();
		_server_names_has_been_set = rhs._server_names_has_been_set;
		if (_server_names_has_been_set)
			_server_names = rhs.getServername();
		_ip = rhs.getIp();
		_port = rhs.getPort();
		_error_page_has_been_set = rhs._error_page_has_been_set;
		if (_error_page_has_been_set)
			_error_page = rhs.getErrorpage();
		_root = rhs.getRoot();
		_clientmaxbodysize = rhs.getClientmaxbodysize();
		_location = rhs.getLocation();
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** -------------------------------- SET_INPUT ---------------------------------
*/

void	ConfigServer::setImput() {

	if (_imput.getType() != OBJECT)
		throw ParsingInfo();
	if (_imput.getObject().find("server_name") != _imput.getObject().end())
		setServername(_imput.getObject().find("server_name")->second);
	if (_imput.getObject().find("root") != _imput.getObject().end())
		setRoot(_imput.getObject().find("root")->second);
	if (_imput.getObject().find("error_page") != _imput.getObject().end())
		setErrorpage(_imput.getObject().find("error_page")->second);
	if (_imput.getObject().find("client_max_body_size") != _imput.getObject().end())
		setClientmaxbodysize(_imput.getObject().find("client_max_body_size")->second);
	if (_imput.getObject().find("location") != _imput.getObject().end())
		setLocation(_imput.getObject().find("location")->second);
	if (_imput.getObject().find("host") != _imput.getObject().end())
		setIp(_imput.getObject().find("host")->second);
	if (_imput.getObject().find("port") != _imput.getObject().end())
		setPort(_imput.getObject().find("port")->second);
	return;
}

/*
** -------------------------------- SET_ROOT ---------------------------------
*/

void	ConfigServer::setRoot(LjohnSon node) {

	if (node.getType() != STRING)
		throw ParsingInfo();
	_root = node.getString();
	// if (_root.find_last_of('/') == _root.size() - 1)
	// 	_root.erase(_root.end() - 1);
	return;
}

/*
** ----------------------------- SET_SERVER_NAME -----------------------------
*/

void	ConfigServer::setServername(LjohnSon node) {

	if (node.getType() != ARRAY)
		throw ParsingInfo();
	for (unsigned long	i = 0; i < node.getArray().size() && node.getArray()[i].getType() == STRING; i++)
		_server_names.push_back(node.getArray()[i].getString());
	_server_names_has_been_set = 1;
	return;
}

/*
** ----------------------------- SET_ERROR_PAGE ------------------------------
*/

void	ConfigServer::setErrorpage(LjohnSon node) {

	std::map<string, LjohnSon>::const_iterator	it;
	if (node.getType() != OBJECT)
		throw ParsingInfo();
	for (it = node.getObject().begin(); it != node.getObject().end(); it++)
	{
		if (it->second.getType() == STRING)
		{
			_error_page.insert(std::pair<string, string>(it->first, it->second.getString()));		
			_error_page_has_been_set = 1;
		}
	}
	return;
}

/*
** ----------------------- SET_CLIENT_MAX_BODY_SIZE -------------------------
*/

void	ConfigServer::setClientmaxbodysize(LjohnSon node) {

	if (node.getType() != NUMBER)
		throw ParsingInfo();
	_clientmaxbodysize = node.getNumber();
	return;
}

/*
** ----------------------------- SET_LOCATION -------------------------------
*/

void	ConfigServer::setLocation(LjohnSon node) {

	if (node.getType() != ARRAY)
		throw ParsingInfo();
	for (unsigned long i = 0; i < node.getArray().size(); i++)
	{
		if (node.getArray()[i].getType() == OBJECT)
		{
			Location	tmp;
			tmp.setImput(node.getArray()[i]);
			_location.push_back(tmp);
		}
	}
	return;
}

/*
** -------------------------------- SET_IP ----------------------------------
*/

void	ConfigServer::setIp(LjohnSon node) {

	if (node.getType() != STRING)
		throw ParsingInfo();
	_ip = node.getString();
	return;
}

/*
** -------------------------------- SET_PORT ---------------------------------
*/

void	ConfigServer::setPort(LjohnSon node) {

	if (node.getType() != NUMBER)
		throw ParsingInfo();
	_port = node.getNumber();
	return;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::vector<string> const	&ConfigServer::getServername() const {

	return(_server_names);
}

string const	&ConfigServer::getRoot() const {

	return(_root);
}

std::map<string, string> const	&ConfigServer::getErrorpage() const {

	return(_error_page);
}

size_t	ConfigServer::getClientmaxbodysize() const {

	return(_clientmaxbodysize);
}

std::vector<Location> const	&ConfigServer::getLocation() const {

	return(_location);
}

string const	&ConfigServer::getIp() const {

	return(_ip);
}

int const	&ConfigServer::getPort() const {

	return(_port);
}

LjohnSon const	&ConfigServer::getImput() const {

	return (_imput);
}

/* ************************************************************************** */