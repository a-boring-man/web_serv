/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CreateResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 09:35:42 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 11:42:15 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <sstream>
#include <fstream>

using std::cerr;
using std::endl;

/*
** ----------------------------- CREATE_RESPOND -------------------------------
*/

void    Client::createResponse( void ) // lance tout le process de repondre a la request
{
	//cerr << "inside create response" << endl;
    _statusCode = _request.getStatusCode();
	if (_request.getStatusCode() != "200") // si la demande n'etait pas bonne
	{
		construct_error_page(_request.getStatusCode());
		ship_page();
		return ;
	}
	std::string ext = setExtension();
	Router	router(_config, _request, _serverPort, ext); // la demande est bonne, if faut trouver le bon serveur virtuelle est block location
	_configServer = router.getConfigserver();
	_location = router.getLocation();
	if (_location.getRedirection().size() > 0) // si redirection alors let's go
	{
		_statusCode = "301";
		ship_page();
	}
	else // sinon process normal de traitement de la demand
	{
		try
		{
			dispatchMethod(_request.getPath());
			ship_page();
		}
		catch(const std::runtime_error& e) // si on doit envoyer une page d'error
		{
			create_good_error_page(e);
		}
	}
}

/*
** ----------------------------- ship_page -------------------------------
*/

void	Client::ship_page() { // construit le message de reponse et dit ok on peut l'envoyer

	std::ostringstream	buffer;

	buffer << "HTTP/1.1" << " " << _statusCode << " " << getMessageStatus() << std::endl;
	buffer << "Server: Weebserv/0.42" << std::endl;
	if (_statusCode == "405") // methode not allow
	{
		unsigned long i;
		buffer << "Allow: ";
		for (i = 0; i < _location.getMethode().size() - 1; i++)
			buffer << _location.getMethode()[i] << ", ";
		buffer << _location.getMethode()[i] << std::endl;
	}
	if (_statusCode == "201") // création fichier
		buffer << "Location: " << _request.getPath() << std::endl;
	if (_statusCode == "301") // redirection
		buffer << "Location: " << _location.getRedirection() << std::endl;
	if (_body.size() != 0) // si body a envoyer
	{	
		buffer << "Content-Length: " << _body.size() << std::endl;
		buffer << "Content-Type: " << _fileType << std::endl << std::endl;
	}
	else
		buffer << std::endl;
	_response = buffer.str();
	_response += _body; // append le boddy a renvoyer au client
	_toSend = _response.size();
	_responseReady = true;
}

/*
** ----------------------------- CREATE_GOOD_ERROR_PAGE ------------------------------
*/

void	Client::create_good_error_page(const std::runtime_error& e) { // gestion du catch en cas d'envoye de page error

	_body.clear();
	_fileType.clear();
	_statusCode = e.what();
	if (!_configServer._error_page_has_been_set) // construit les pages par default
	{
		construct_error_page(e.what());
		ship_page();
	}
	else
	{
		std::map<string, string>::const_iterator it;
		for (it = _configServer.getErrorpage().begin(); it != _configServer.getErrorpage().end(); it++)
		{
			if (!strcmp(e.what(), it->first.c_str())) // si il y a une error page dans la config
			{
				string	tmp;
				tmp += "/Users/jrinna/Documents/cursus42/web_serv/site";
				tmp += it->second;
				if (!file_exist(tmp)) // si elle ne mene nul part
				{
					construct_error_page(e.what());
					ship_page();
					return ;
				}
				else // si elle existe
				{
					std::ifstream	file;
					file.open(tmp);
					if (file.fail()) // si fail de l'open
					{
						construct_error_page(e.what());
						ship_page();
						return ;
					}
					std::stringstream	buffer;
					buffer << file.rdbuf();
					_body = buffer.str();
					_fileType = "text/html";
					file.close();
				}
			}
		}
		construct_error_page(e.what()); // si auccune ne match
		ship_page();
	}
}
