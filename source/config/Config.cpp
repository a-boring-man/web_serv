/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 11:37:17 by jrinna            #+#    #+#             */
/*   Updated: 2022/12/06 11:33:51 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

using std::endl;
using std::cout;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Config::Config() {

	return;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Config const & i )
{
	for (unsigned long	j = 0; j < i.getList().size(); j++)
		o << "config : " << j << " " << endl << i.getList()[j] << endl;
	o << endl;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Config::configure(LjohnSon &node) {

	_imput = node;
	if (_imput.getType() != ARRAY)
		throw ParsingInfo();
	for (unsigned long	i = 0; i < _imput.getArray().size(); i++)
	{
		if (_imput.getArray()[i].getType() == OBJECT)
		{
			ConfigServer	tmp = ConfigServer(_imput.getArray()[i]);
			tmp.setImput();
			_listconfig.push_back(tmp);
		}
	}
	return;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::vector<ConfigServer> const	&Config::getList() const {

	return(_listconfig);
}

/* ************************************************************************** */