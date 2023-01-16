/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LjohnSon.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 11:34:33 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 10:28:39 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <map>
# include <vector>
# include <iostream>

class LjohnSon;
enum	Type {ARRAY, OBJECT, STRING, NUMBER, BOOLEAN, NULL_TYPE};
struct	Values {

	std::map<std::string, LjohnSon>	object;
	std::vector<LjohnSon>	array;
	std::string	s;
	long		number;
};

class LjohnSon
{
	public:

		class	logic_error : public std::exception {
			public:
				virtual const char* what() const throw(){return ("logic error on config file");} 
		};

		LjohnSon();
		LjohnSon( char *file );

		int										getType()		const;
		int										setArray( int start );
		int										setNumber( int start ); // return the end of the string(the position of the " character)
		int										setString( int start ); // return the position of the last accepted number character
		int										setObject( int start );
		void									printnode( int depth );
		void									parse( std::string &s );
		long							const	&getNumber()	const;
		std::string						const	&getString()	const;
		std::vector<LjohnSon>			const	&getArray()		const;
		std::map<std::string, LjohnSon>	const	&getObject()	const;

	private:

		Type	_type;
		Values	_value;
		char	*_file;

};
