/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/05 11:05:59 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 10:34:21 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include "Config.hpp"
# include "Request.hpp"


class Router
{

	public:

		Router( Config const &conf, Request const &header, int port, std::string ext );

		int						getPort()			const;
		Config			const	&getConfig()		const;
		Request			const	&getRequest()		const;
		Location		const	&getLocation()		const;
		ConfigServer	const	&getConfigserver()	const;

	private:

		bool	is_there_this_header_option(std::string ho)	const;
		void	choose();
		void	setLocation();
		void	setConfigserver();
		void	add_matching_port( std::vector<ConfigServer> &portselection );
		void	erase_nonmatching_servername( std::vector<ConfigServer> &portselection );

		Config	const	&_conf;
		Request	const	&_header;
		Location		_location;
		ConfigServer	_configserver;
		int				_port;
		std::string		_ext;

};

std::ostream &			operator<<( std::ostream & o, Router const & i );
