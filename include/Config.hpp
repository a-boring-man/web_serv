/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 11:37:20 by jrinna            #+#    #+#             */
/*   Updated: 2022/12/06 11:33:51 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include "ConfigServer.hpp"
# include <vector>

class Config
{

	public:

		class	ParsingInfo : public std::exception {
			public:
				virtual const char* what() const throw(){return ("error on reading config file deeper information");} 
		};

		Config();

		void	configure(LjohnSon &imput);
		std::vector<ConfigServer> const	&getList()	const;

	private:

		LjohnSon					_imput;
		std::vector<ConfigServer>	_listconfig;

};

std::ostream &			operator<<( std::ostream & o, Config const & i );
