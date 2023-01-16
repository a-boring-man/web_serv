/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccartet <ccartet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/22 10:30:38 by ccartet           #+#    #+#             */
/*   Updated: 2023/01/16 12:06:02 by ccartet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "utils.hpp"
#include <algorithm>

using std::cout;
using std::cerr;
using std::endl;

Request::Request( void ) : 

	multipartBody_has_been_set( false ),
	_raw(""),
	_headerOk( false ),
	_method( "" ),
	_path( "" ),
	_queries( "" ),
	_boundary( "" ),
	_body( "" ),
	_bodyLen( 0 ),
	_statusCode( "200" ),
	_finishParsing( false ) {
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** --------------------------------- PARSE ----------------------------------
*/

void	Request::parse( std::string const & tmprecv ) // tente de recupperer toute les parties du headers de request
{
	size_t	i = 0;
	std::string	bodyTmp = "";
	
	try {
		if (tmprecv.size() == 0)
			throw std::runtime_error("400");
		if (_method == "")
		{
			_headerOk = isCompleteHeader(tmprecv);
			if (_headerOk == true)
			{
				setMethod(i); // check tout les champs du header
				setPathAndQueries(i);
				checkVersion(i);
				setHeaderOptions(i);
			}
		}
		if (_headerOk == true)
		{
			if (i < _raw.size())  // pour le 1e passage du body
				bodyTmp = _raw.substr(i, _raw.length());	
			_headerOk = false;
		}
		else if (_method != "")
			bodyTmp = tmprecv;
		if (_method != "" && _finishParsing == false)
			setBody(bodyTmp);
	}
	catch (std::runtime_error& e)
	{
		_statusCode = e.what();
		_finishParsing = true;
	}
}

bool	Request::isCompleteHeader( std::string const & tmprecv ) // chope la partie header de la request peut casser si client met plusieur ligne a tester
{
	_raw.append(tmprecv);
	return (_raw.find("\r\n\r\n") != _raw.npos);
}

/*
** --------------------------------- SET_METHOD ----------------------------------
*/

void	Request::setMethod( size_t &i )
{
	std::string	tmp;
	
	size_t	pos = _raw.find(' ');
	if (pos == std::string::npos)
		throw std::runtime_error("400");
	tmp = _raw.substr(0, pos);
	if (tmp != "GET" && tmp != "POST" && tmp != "DELETE")
		throw std::runtime_error("405");
	_method = tmp;
	i = pos;
}

/*
** --------------------------------- SET_PATH_AND_QUERIES ----------------------------------
*/

void	Request::setPathAndQueries( size_t &i )
{
	while (_raw[i] != '/' && _raw[i])
		i++;
	size_t	pos = _raw.find_first_of(' ', i);
	if (pos == std::string::npos)
		throw std::runtime_error("400");
	if (pos - i > 1024)
		throw std::runtime_error("414");
	pos = _raw.find_first_of(" ?", i);
	_path = _raw.substr(i, pos - i); // set path jusqu'à ' ' ou '?'
	if (_raw[pos] == '?')
	{
		i = pos + 1;
		pos = _raw.find_first_of(' ', i);
		_queries = _raw.substr(i, pos - i);
	}
	i = pos;
}

void	Request::checkVersion( size_t &i )
{
	std::string	tmp;
	
	while (_raw[i] != 'H' && _raw[i])
		i++;
	size_t	pos = _raw.find("HTTP/");
	if (pos == std::string::npos || pos != i)
		throw std::runtime_error("400");
	i += 5;
	pos = _raw.find_first_of("\r\n", i);
	if (pos == std::string::npos)
		throw std::runtime_error("400");
	tmp = _raw.substr(i, pos - i);
	if (tmp != "1.1")
		throw std::runtime_error("505");
	i = pos + 2;
}

/*
** --------------------------------- SET_HEADER_OPTION ----------------------------------
*/

void	Request::setHeaderOptions( size_t &i )
{
	std::string	key;
	std::string	value;

	while (i < _raw.size())
	{
		if (nextLine(_raw, i) == false)
			break;
		while (_raw[i] && _raw[i] != ':')
			key += _raw[i++];
		if (_raw[i])
			i++;
		else
			throw std::runtime_error("400");
		while (_raw[i] && _raw[i] == ' ')
			i++;
		while (_raw[i] && _raw[i] != '\r')
			value += _raw[i++];
		_headerOptions.insert(std::make_pair(key, value));
		_headerOptionSet.push_back(key);
		key.clear();
		value.clear();
	}
	if (_headerOptions.find("Content-Type") != _headerOptions.end()) // set _boundary
	{
		if (_headerOptions.at("Content-Type").find("multipart") != std::string::npos)
		{
			std::string contentType = _headerOptions.at("Content-Type");
			int	pos = contentType.find("=") + 1;
			_boundary = contentType.substr(pos, contentType.size());
		}
	}
	if (_method == "POST" && _headerOptions.find("Transfer-Encoding") == _headerOptions.end()
		&& _headerOptions.find("Content-Length") == _headerOptions.end())
		throw std::runtime_error("411");
}

/*
** --------------------------------- SET_BODY ----------------------------------
*/

void	Request::setBody( std::string bodyTmp )
{
	int ret = checkTypeOfBody();
	switch (ret)
	{
		case 0:
			_finishParsing = true; // pas de body
			break;
		case 1:
			setSimpleBody(bodyTmp);
			break;
		case 2:
			setChunkedBody(bodyTmp);
			break;
	}
	if (_headerOptions.find("Content-Type") != _headerOptions.end() && _finishParsing == true)
	{
		if (_headerOptions.at("Content-Type").find("multipart") != std::string::npos)
			setMultipartBody();	
	}
}

/*
** --------------------------------- CHECK_TYPE_OF_BODY ----------------------------------
*/

int	Request::checkTypeOfBody( void ) const
{
	if (_headerOptions.find("Transfer-Encoding") == _headerOptions.end() && _headerOptions.find("Content-Length") == _headerOptions.end())
		return (0); // pas de body
	if (_headerOptions.find("Transfer-Encoding") != _headerOptions.end() && _headerOptions.find("Content-Length") != _headerOptions.end())
		throw std::runtime_error("400");
	if (_headerOptions.find("Transfer-Encoding") != _headerOptions.end() &&
		_headerOptions.at("Transfer-Encoding").find("chunked") != std::string::npos)
		return (2); // chunked
	return (1); // simple body
}

/*
** --------------------------------- SET_SIMPLE_BODY ----------------------------------
*/

void	Request::setSimpleBody( std::string bodyTmp ) // attention a vérifier !!!!
{
	if (_bodyLen == 0)
		_bodyLen = stol(_headerOptions.at("Content-Length"), nullptr, 10);
	_body.append(bodyTmp, 0, ft_min(bodyTmp.length(), _bodyLen));
	_bodyLen -= bodyTmp.length();
	std::string	endB = "--" + _boundary + "--";
	if (_bodyLen < 0)
		throw std::runtime_error("400");
	if (_bodyLen == 0)
		_finishParsing = true;
}

/*
** --------------------------------- SET_CHUNCKED_BODY ----------------------------------
*/

void	Request::setChunkedBody( std::string bodyTmp )
{
	std::string	line;
	std::istringstream	input(bodyTmp.c_str());
	
	while (std::getline(input, line))
	{
		if (_bodyLen <= 0 && (int)line.c_str()[0] != 0)
		{
			_bodyLen = std::stoi(line, nullptr, 16);
			if (_bodyLen == 0)
			{
				_finishParsing = true;
				break;
			}
		}	
		else
		{
			_body.append(line, 0, line.size());
			_bodyLen -= line.size();
		}
		line.clear();
	}
}

/*
** --------------------------------- SET_MULTIPART_BODY ----------------------------------
*/

void	Request::setMultipartBody( void )
{
	std::string	boundary = "--" + _boundary;
	int	boundLen = boundary.size();
	size_t	sizeOfBoundary = 0;
	size_t	sizeOfMiniHeader = 0;
	size_t	sizeOfMiniBody = 0;
	std::string	tmpBody = _body;

	while (_body[sizeOfBoundary])
	{
		s_MultipartBody	new_partBody;
		if (tmpBody.find(boundary + "--") == 0)
			break;
		sizeOfBoundary = tmpBody.find(boundary) + boundLen + 2;
		tmpBody.erase(0, sizeOfBoundary);
		sizeOfMiniHeader = setMiniHeaderBody(tmpBody, &new_partBody);
		sizeOfMiniBody = tmpBody.find(boundary);
		new_partBody.body = tmpBody.substr(sizeOfMiniHeader, sizeOfMiniBody - sizeOfMiniHeader);
		_multiBody.push_back(new_partBody);
		tmpBody.erase(0, sizeOfMiniBody);
		sizeOfBoundary = 0;
	}
	multipartBody_has_been_set = true;
}

/*
** --------------------------------- SET_MINI_HEADER_BODY ----------------------------------
*/

long	Request::setMiniHeaderBody( std::string bodyTmp, s_MultipartBody *new_partBody )
{
	std::string	key;
	std::string	value;
	size_t	i = 0;
	
	while (i < bodyTmp.size())
	{
		if (nextLine(bodyTmp, i) == false)
			break;
		while (bodyTmp[i] == ' ' || bodyTmp[i] == ';')
			i++;
		while (bodyTmp[i] != ':' && bodyTmp[i] != '=')
			key += bodyTmp[i++];
		i++;
		while (bodyTmp[i] == ' ')
			i++;
		while (bodyTmp[i] != ';' && bodyTmp[i] != '\r')
			value += bodyTmp[i++];
		new_partBody->miniHeader.insert(std::make_pair(key, value));
		key.clear();
		value.clear();
	}
	return (i);
}

/*
** --------------------------------- NEXT_LINE ----------------------------------
*/

bool	Request::nextLine( std::string str, size_t &i ) const
{
	if (str[i] == '\r' && str[i + 1] == '\n')
		i += 2;
	if (str[i] == '\r')
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
			i += 2;
		return false;
	}
	return true;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string const &	Request::getMethod( void ) const
{
	return _method;
}

std::string const &	Request::getPath( void ) const
{
	return _path;
}

std::string const &	Request::getQueries( void ) const
{
	return _queries;
}
		
std::map<std::string, std::string>	Request::getHeaderOptions( void ) const
{
	return _headerOptions;
}

std::string const &	Request::getHost( void ) const
{
	return _headerOptions.at("Host");
}

std::string const &	Request::getBody( void ) const
{
	return _body;
}

std::vector<s_MultipartBody>	Request::getMultiBody( void ) const
{
	return _multiBody;
}

std::string const &	Request::getStatusCode( void ) const
{
	return _statusCode;
}

bool	Request::getFinishParsing( void ) const
{
	return _finishParsing;
}

std::vector<std::string>	const	&Request::getHeaderOptionSet( void ) const {

	return (_headerOptionSet);
}

long	Request::getBodyLen( void ) const {

	return (_bodyLen);
}

/* ************************************************************************** */

std::ostream &	operator<<( std::ostream & o, Request const & rhs )
{
	o << rhs.getMethod() << " " << rhs.getPath();
	if (rhs.getQueries().size() != 0)
		o << "?" << rhs.getQueries();
	o << " HTTP/1.1" << std::endl;
	
	std::map<std::string, std::string>	print = rhs.getHeaderOptions();
	std::map<std::string, std::string>::iterator	ite = print.end();
	std::map<std::string, std::string>::iterator	itb;
	
	for (itb = print.begin(); itb != ite; itb++)
		o << (*itb).first << ": " << (*itb).second << std::endl;

	return o;
}

void	Request::clear_client() {

	_raw.clear();
	_headerOk = false;
	_method.clear();
	_path.clear();
	_queries.clear();
	_headerOptions.clear();
	_headerOptionSet.clear();
	_boundary.clear();
	_body.clear();
	_bodyLen = 0;
	_multiBody.clear();
	_statusCode = "200";
	_finishParsing = false;
	multipartBody_has_been_set = false;
}
