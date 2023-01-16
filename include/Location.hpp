/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 14:45:01 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 10:32:21 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include "LjohnSon.hpp"

using std::string;

class Location
{

	public:

		class	ParsingInfo : public std::exception {
			public:
				virtual const char* what() const throw(){return ("error on reading config file deeper information");} 
		};

		Location();

		void	clear( void );
		void	setRoot(LjohnSon node);
		void	setType(LjohnSon node);
		void	setMethode(LjohnSon node);
		void	setImput( LjohnSon imput );
		void	setFastcgipara(LjohnSon node);
		void	setFastcgipass(LjohnSon node);
		void	setRedirection(LjohnSon node);
		void	setDirectorylistening(LjohnSon node);
		void	setDefaultfileifdirectory(LjohnSon node);

		int					const &	getDirectorylistening()		const;
		string				const & getRoot()					const;
		string				const & getType()					const;
		string				const & getFastcgipass()			const;
		string				const & getFastcgipara()			const;
		string				const & getRedirection()			const;
		std::vector<string>	const & getMethode()				const;
		std::vector<string>	const & getDefaultfileifdirectory()	const;

		bool	redirection_has_been_set;
		bool	defaultfileifdirectory_has_been_set;
		bool	fastcgipass_has_been_set;
		bool	fastcgipara_has_been_set;
		bool	root_has_been_set;

	private:

		LjohnSon			_imput;
		string				_type; // |
		std::vector<string>	_methode; // | //  a changer vector de string
		int					_directorylistening; // |
		string				_redirection; // |
		std::vector<string>	_defaultfileifdirectory; // | at least one of them	 // vector string
		string				_fastcgipass; // |
		string				_fastcgipara; // |
		string				_root; // / 

};

std::ostream &			operator<<( std::ostream & o, Location const & i );
