/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 12:50:27 by jrinna            #+#    #+#             */
/*   Updated: 2022/12/20 10:55:21 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server() {

	return;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Server::getSocket( void ) const
{
	return _serversocket;
}

void	Server::setIp(string const &s) {

	_ip = s;
	return;
}

void	Server::setPort(int const &i) {

	_port = i;
	return;
}

string const &Server::getIp() const {

	return (_ip);
}

int const &Server::getPort() const {

	return (_port);
}

/* ************************************************************************** */
