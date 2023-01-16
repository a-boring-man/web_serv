/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 15:03:57 by jrinna            #+#    #+#             */
/*   Updated: 2022/12/23 11:18:59 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

using std::endl;
using std::cout;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location() : redirection_has_been_set(0), defaultfileifdirectory_has_been_set(0), fastcgipass_has_been_set(0), fastcgipara_has_been_set(0), root_has_been_set(0), _type("/"), _directorylistening(1) {

	_methode.push_back("GET");
	_methode.push_back("POST");
	_methode.push_back("DELETE");
	return;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Location const & i )
{
	o << "Type : " << i.getType() << endl;
	for (unsigned long j = 0; j < i.getMethode().size(); j++)
		o << "methode : " << j << " : " << i.getMethode()[j] << endl;
	if (i.fastcgipass_has_been_set)
		o << "fastcgipass : " << i.getFastcgipass() << endl;		
	if (i.fastcgipara_has_been_set)
		o << "fastcgipara : " << i.getFastcgipara() << endl;
	if (i.root_has_been_set)
		o << "root : " << i.getRoot() << endl;
	o << "directory_listening : " << i.getDirectorylistening() << endl;
	if (i.redirection_has_been_set)
		o << "redirection : " << i.getRedirection() << endl;
	if (i.defaultfileifdirectory_has_been_set)
		for (unsigned long j = 0; j < i.getDefaultfileifdirectory().size(); j++)
			o << "default_file_if_directory : " << j << " : " << i.getDefaultfileifdirectory()[j] << endl;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Location::setImput(LjohnSon imput) {

	_imput = imput;
	if (_imput.getType() != OBJECT) // verifie que location soit bien de type object
		throw ParsingInfo();
	if (_imput.getObject().find("fastcgipass") != _imput.getObject().end())
		setFastcgipass(_imput.getObject().find("fastcgipass")->second);
	if (_imput.getObject().find("fastcgipara") != _imput.getObject().end())
		setFastcgipara(_imput.getObject().find("fastcgipara")->second);
	if (_imput.getObject().find("root") != _imput.getObject().end())
		setRoot(_imput.getObject().find("root")->second);
	if (_imput.getObject().find("type") != _imput.getObject().end())
		setType(_imput.getObject().find("type")->second);
	if (_imput.getObject().find("methode") != _imput.getObject().end())
		setMethode(_imput.getObject().find("methode")->second);
	if (_imput.getObject().find("autoindex") != _imput.getObject().end())
		setDirectorylistening(_imput.getObject().find("autoindex")->second);
	if (_imput.getObject().find("redirection") != _imput.getObject().end())
		setRedirection(_imput.getObject().find("redirection")->second);
	if (_imput.getObject().find("index") != _imput.getObject().end())
		setDefaultfileifdirectory(_imput.getObject().find("index")->second);
	return;
}

/*
** --------------------------------- SET_TYPE ----------------------------------
*/

void	Location::setType(LjohnSon node) {

	if (node.getType() != STRING)
		throw ParsingInfo();
	_type = node.getString();
	return;
}

/*
** --------------------------------- SET_FAST_CGI_PASS ----------------------------------
*/

void	Location::setFastcgipass(LjohnSon node) {

	if (node.getType() != STRING)
		throw ParsingInfo();
	_fastcgipass = node.getString();
	fastcgipass_has_been_set = 1;
	return;
}

/*
** --------------------------------- SET_FAST_CGI_PARA ----------------------------------
*/

void	Location::setFastcgipara(LjohnSon node) {

	if (node.getType() != STRING)
		throw ParsingInfo();
	_fastcgipara = node.getString();
	fastcgipara_has_been_set = 1;
	return;
}

/*
** --------------------------------- SET_ROOT ----------------------------------
*/

void	Location::setRoot(LjohnSon node) {

	if (node.getType() != STRING)
		throw ParsingInfo();
	_root = node.getString();
	// if (_root.find_last_of('/') == _root.size() - 1)
	// 	_root.erase(_root.end() - 1);
	root_has_been_set = 1;
	return;
}

/*
** --------------------------------- SET_METHODS ----------------------------------
*/

void	Location::setMethode(LjohnSon node) {

	string	tmp;
	if (node.getType() != ARRAY)
		throw ParsingInfo();
	int	j = 0;
	for (unsigned long i = 0; i < node.getArray().size(); i++)
		if (node.getArray()[i].getType() == STRING)
			j++;
	if (j)
		_methode.clear();
	for (unsigned long i = 0; i < node.getArray().size(); i++)
	{
		if (node.getArray()[i].getType() == STRING)
		{
			tmp = node.getArray()[i].getString();
			_methode.push_back(tmp);
		}
	}
	return;
}

/*
** ------------------------------ SET_DIRECTORY_LISTENING -------------------------------
*/

void	Location::setDirectorylistening(LjohnSon node) {

	if (node.getType() != NUMBER)
		throw ParsingInfo();
	_directorylistening = node.getNumber();
	return;
}

/*
** --------------------------------- SET_REDIRECTION ----------------------------------
*/

void	Location::setRedirection(LjohnSon node) {

	if (node.getType() != STRING)
		throw ParsingInfo();
	_redirection = node.getString();
	redirection_has_been_set = 1;
	return;
}

/*
** --------------------------------- SET_DEFAULT_FILE_IF_DIRECTORY ----------------------------------
*/

void	Location::setDefaultfileifdirectory(LjohnSon node) {

	string	tmp;
	if (node.getType() != ARRAY)
		throw ParsingInfo();
	for(unsigned long i = 0; i < node.getArray().size(); i++)
	{
		if (node.getArray()[i].getType() == STRING)
		{
			tmp = node.getArray()[i].getString();
			_defaultfileifdirectory.push_back(tmp);
			defaultfileifdirectory_has_been_set = 1;
		}
	}
	return;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

string const &	Location::getType() const {

	return(_type);
}

string const &	Location::getFastcgipass() const {

	return(_fastcgipass);
}

string const &	Location::getFastcgipara() const {

	return(_fastcgipara);
}

string const &	Location::getRoot() const {

	return(_root);
}

std::vector<string> const &	Location::getMethode() const {

	return(_methode);
}

int const &	Location::getDirectorylistening() const {

	return(_directorylistening);
}

string const &	Location::getRedirection() const {

	return(_redirection);
}

std::vector<string> const &	Location::getDefaultfileifdirectory() const {

	return(_defaultfileifdirectory);
}

/* ************************************************************************** */

void	Location::clear() {

	_type.clear();
	_methode.clear();
	_directorylistening = 1;
	_redirection.clear();
	_defaultfileifdirectory.clear();
	_fastcgipass.clear();
	_fastcgipara.clear();
	_root.clear();
	redirection_has_been_set = false;
	defaultfileifdirectory_has_been_set = false;
	fastcgipara_has_been_set = false;
	fastcgipass_has_been_set = false;
	root_has_been_set = false;
	setImput(_imput);
}