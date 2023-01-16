/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/22 10:30:35 by ccartet           #+#    #+#             */
/*   Updated: 2023/01/13 13:47:03 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <vector>
#include <map>
#include <iostream>
#include <exception>
#include <sstream>
#include <string>

typedef struct MultipartBody
{
	std::map<std::string, std::string>	miniHeader;
	std::string							body;	

}				s_MultipartBody; 

class Request {
	
	public:

		Request( void );
		bool								getFinishParsing( void ) const;
		long								getBodyLen( void ) 			const;
		void								clear_client( void );
		void								parse( std::string const & tmprecv );
		std::string const &					getBody( void ) const;
		std::string const &					getHost( void ) const;
		std::string const &					getPath( void ) const;
		std::string const &					getMethod( void ) const;
		std::string const &					getQueries( void ) const;
		std::string const &					getStatusCode( void ) const;
		std::vector<s_MultipartBody>		getMultiBody( void ) const;
		std::map<std::string, std::string>	getHeaderOptions( void ) const;
		std::vector<std::string>	const	&getHeaderOptionSet( void )	const;
		bool	multipartBody_has_been_set;
		
	private:

		std::string							_raw;
		bool								_headerOk;
		std::string							_method;
		std::string							_path;
		std::string							_queries; // à passer ensuite à la variable d'environnement QUERY_STRING ?
		std::map<std::string, std::string>	_headerOptions;
		std::vector<std::string>			_headerOptionSet;
		std::string							_boundary;
		std::string							_body;
		long								_bodyLen;
		std::vector<s_MultipartBody>		_multiBody;
		std::string							_statusCode;
		bool								_finishParsing;

		int			checkTypeOfBody( void ) const;
		bool		setRawHeader( std::string const & tmprecv );
		bool		nextLine( std::string str, size_t &i ) const;
		void		setMethod( size_t &i );
		void		setMultipartBody( void );
		void		checkVersion( size_t &i );
		void		setHeaderOptions( size_t &i );
		void		setBody( std::string bodyTmp );
		void		setPathAndQueries( size_t &i );
		void		setSimpleBody( std::string bodyTmp );
		void		setChunkedBody( std::string bodyTmp );
		long		setMiniHeaderBody( std::string bodyTmp, s_MultipartBody *new_partBody );

};

std::ostream &	operator<<( std::ostream & o, Request const & rhs );

#endif

// ACCEPT <MIME_type>/<MIME_subtype>, */* => Tout type MIME