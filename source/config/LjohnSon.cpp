/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LjohnSon.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 11:34:26 by jrinna            #+#    #+#             */
/*   Updated: 2022/12/09 13:53:23 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "LjohnSon.hpp"
#include <string> // pour transformer la string en char *

using std::cout;
using std::endl;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

LjohnSon::LjohnSon() {

	return;
}

LjohnSon::LjohnSon( char *file ) : _file(file) {

	return;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** ---------------------------------- PARSE -----------------------------------
*/

void	LjohnSon::parse( std::string &s ) {

	_file = const_cast<char*>(s.c_str());
	int i = 0;
	while (_file[i] && (_file[i] == ' ' || _file[i] == '\n' || _file[i] == '\t' || _file[i] == '\r'))
		i++;
	if (_file[i] >= '0' && this->_file[i] <= '9')
		setNumber(i);
	else if (_file[i] == '"')
		setString(i + 1);
	else if (_file[i] == '[')
		setArray(i + 1);
	else if (_file[i] == '{')
		setObject(i + 1);
	else
		throw logic_error();
}

/*
** ------------------------------- PRINT_NODE --------------------------------
*/

void	LjohnSon::printnode( int depth ) {

	int	cpy_depth = depth;
	
	if (_type == STRING)
		cout << "\"" << _value.s << "\"";
	else if (_type == NUMBER)
		cout << _value.number;
	else if (_type == ARRAY)
	{
		std::vector<LjohnSon>::iterator	it;
		while (cpy_depth--)
		cout << "\t";
		cout << '[';
		for (it = _value.array.begin(); it != _value.array.end(); it++)
		{
			(*it).printnode(depth + 1);
			cout << ',' << endl;
		}
		cout << ']';
	}
	else if (_type == OBJECT)
	{
		std::map<std::string, LjohnSon>::iterator	it;
		while (cpy_depth--)
		cout << "\t";
		cout << '{';
		for (it = _value.object.begin(); it != _value.object.end(); it++)
		{
			cout << "key : " << it->first << ':' << "value : ";
			it->second.printnode(depth + 1);
			cout << ',' << endl;
		}
		cout << '}';
	}
}

/*
** ------------------------------- SET_NUMBER --------------------------------
*/

int	LjohnSon::setNumber( int start ) {

	int	i;
	for (i = start; this->_file[i] >= '0' && this->_file[i] <= '9'; i++){}
	this->_type = NUMBER;
	this->_value.number = std::stol(std::string(&this->_file[start], i - start));
	return (i - 1);
}

/*
** ------------------------------- SET_STRING --------------------------------
*/

int	LjohnSon::setString( int i )
{
	std::string new_s; // = new std::string;
	while (_file[i])
	{
		if (_file[i] == '\\'
			&& !(_file[i + 1] == '"' || _file[i + 1] == '\\' || _file[i + 1] == '/' || _file[i + 1] == 'b'
			|| _file[i + 1] == 'f' || _file[i + 1] == 'n' || _file[i + 1] == 'r' || _file[i + 1] == 't'))
			throw logic_error();
		else if (_file[i] == '"')
			break;
		new_s += _file[i];
		i++;
	}
	if (_file[i] == EOF)
	 	throw logic_error();
	this->_type = STRING;
	this->_value.s = new_s;
	return (i);
}

/*
** ------------------------------- SET_ARRAY --------------------------------
*/

int	LjohnSon::setArray( int i )
{
	std::vector<LjohnSon>	new_array; // = new std::vector<LjohnSon*>;
	int						valid_array = 0;

	while (_file[i])
	{
		LjohnSon				node = LjohnSon(_file);// = new LjohnSon(_file);
		while (_file[i] == ' ' || _file[i] == '\n' || _file[i] == '\t' || _file[i] == '\r')
			i++;
		if (_file[i] >= '0' && this->_file[i] <= '9')
		{
			i = node.setNumber(i);
			new_array.push_back(node);
		}
		else if (_file[i] == '"')
		{
			i = node.setString(i + 1);
			new_array.push_back(node);
		}
		else if (_file[i] == '[')
		{
			i = node.setArray(i + 1);
			new_array.push_back(node);
		}
		else if (_file[i] == '{')
		{
			i = node.setObject(i + 1);
			new_array.push_back(node);
		}
		else
			throw logic_error();
		i++;
		while (_file[i] == ' ' || _file[i] == '\n' || _file[i] == '\t' || _file[i] == '\r')
			i++;
		if (_file[i] == ']')
		{
			valid_array = 1;
			break;
		}
		else if (_file[i] == ',')
			i++;
		else
			throw logic_error();
	}
	if (valid_array == 0)
		throw logic_error();
	this->_type = ARRAY;
	this->_value.array = new_array;
	return (i);
}

/*
** -------------------------------- SET_OBJECT ---------------------------------
*/

int	LjohnSon::setObject( int start ) {

	int								should_end_first_while;
	int								should_end_second_while = 0;
	int								i = start;
	LjohnSon						tmp_string_node(this->_file);
	std::string						key;
	std::map<std::string, LjohnSon>	object;// = new(std::map<std::string, LjohnSon*>);
	while (!should_end_second_while)
	{
		LjohnSon						node = LjohnSon(_file);// = new LjohnSon(this->_file);
		should_end_first_while = 0;
		while (!should_end_first_while) // detection de la key (string)
		{
			if (_file[i] == '"')
			{
				i = tmp_string_node.setString( i + 1 );
				key = tmp_string_node.getString();
				should_end_first_while = 1;
			}
			else if (_file[i] == ' ' || _file[i] == '\n' || _file[i] == '\t' || _file[i] == '\r')
				i++;
			else
			{
				throw logic_error();
			}
		}
		i++;
		while (this->_file[i] && (this->_file[i] == ' ' || _file[i] == '\n' || _file[i] == '\t' || _file[i] == '\r')) // detection du :
			i++;
		if (this->_file[i] && this->_file[i] == ':')
			i++;
		else
			throw logic_error();
		while (this->_file[i] && (this->_file[i] == ' ' || _file[i] == '\n' || _file[i] == '\t' || _file[i] == '\r'))
			i++;
		if (this->_file[i] >= '0' && this->_file[i] <= '9') // detection de la node (values)
		{
			i = node.setNumber(i);
			object.insert(std::pair<std::string, LjohnSon>(key, node));
		}
		else if (this->_file[i] == '"')
		{
			i = node.setString(i + 1);
			object.insert(std::pair<std::string, LjohnSon>(key, node));
		}
		else if (this->_file[i] == '[')
		{
			i = node.setArray(i + 1);
			object.insert(std::pair<std::string, LjohnSon>(key, node));
		}
		else if (this->_file[i] == '{')
		{
			i = node.setObject(i + 1);
			object.insert(std::pair<std::string, LjohnSon>(key, node));
		}
		else
			throw logic_error();
		i++;
		while (this->_file[i] && (this->_file[i] == ' ' || _file[i] == '\n' || _file[i] == '\t' || _file[i] == '\r'))
			i++;
		if (this->_file[i] == ',') // check a la fin de value si je doit continuer
			i++;
		else if (this->_file[i] == '}')
		{
			this->_type = OBJECT;
			this->_value.object = object;
			should_end_second_while = 1;
		}
		else
			throw logic_error();
	}
	return (i);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	LjohnSon::getType() const {

	return (_type);
}

long const	&LjohnSon::getNumber() const {

	return(this->_value.number);
}

std::string const	&LjohnSon::getString() const {

	return(_value.s);
}

std::vector<LjohnSon> const	&LjohnSon::getArray() const {

	return(_value.array);
}

std::map<std::string, LjohnSon> const	&LjohnSon::getObject() const {

	return (_value.object);
}

/* ************************************************************************** */