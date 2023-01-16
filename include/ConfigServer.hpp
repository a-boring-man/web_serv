/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 14:16:46 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 10:22:43 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include "Location.hpp"
# include <vector>

using std::string;

class ConfigServer
{

	public:

		class	ParsingInfo : public std::exception {
			public:
				virtual const char* what() const throw(){return ("error on reading config file deeper information");} 
		};

		ConfigServer();
		ConfigServer(LjohnSon node);

		void	setImput();
		void	setIp(LjohnSon node);
		void	setPort(LjohnSon node);
		void	setRoot(LjohnSon node);
		void	setLocation(LjohnSon node);
		void	setErrorpage(LjohnSon node);
		void	setServername(LjohnSon node);
		void	setClientmaxbodysize(LjohnSon node);

		ConfigServer &		operator=( ConfigServer const & rhs );

		int							const	&getPort()				const;
		size_t								getClientmaxbodysize()	const;
		string						const	&getRoot()				const;
		string						const	&getIp()				const;
		LjohnSon					const	&getImput()				const;
		std::vector<string>			const	&getServername()		const;
		std::vector<Location>		const	&getLocation()			const;
		std::map<string, string>	const	&getErrorpage()			const;

		int			_server_names_has_been_set;
		int			_error_page_has_been_set; // optionelle ?

	private:

		LjohnSon					_imput;
		std::vector<string>			_server_names; // optionelle
		string						_ip;
		int							_port;
		string						_root;
		std::map<string, string>	_error_page; // a changer en int, string
		size_t						_clientmaxbodysize;
		std::vector<Location>		_location;
		// rajouter redirection map<int, string>

};

std::ostream &			operator<<( std::ostream & o, ConfigServer const & i );
