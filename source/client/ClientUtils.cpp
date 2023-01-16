/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/16 13:46:36 by ccartet           #+#    #+#             */
/*   Updated: 2023/01/11 11:22:20 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	Client::initMapError( void )
{
	_errors["200"] = "OK";
	_errors["201"] = "Created";
	_errors["204"] = "No Content"; // Requête traitée avec succès mais pas d’information à renvoyer
	_errors["301"] = "Moved Permanently";
	_errors["400"] = "Bad Request";
	_errors["403"] = "Forbidden"; // pas de droits d'accès pour ce fichier/dossier
	_errors["404"] = "Not Found"; // ressource non trouvée
	_errors["405"] = "Method Not Allowed"; // Méthode de requête non autorisée
	_errors["406"] = "Not Acceptable"; // Ressource demandée pas disponible dans « Accept » de la requête
	_errors["409"] = "Conflict";
	_errors["411"] = "Length Required";
	_errors["414"] = "Request-URI Too Long";
	_errors["413"] = "Playload Too Large";
	_errors["415"] = "Unsupported Media Type";
	_errors["500"] = "Internal Server Error";
	_errors["505"] = "HTTP Version Not Supported";
}

void	Client::initMapType( void )
{
	_types["txt"] = "text/plain";
	_types["html"] = "text/html";
	_types["htm"] = "text/html";
	_types["css"] = "text/css";
	_types["js"] = "text/javascript";
	_types["mjs"] = "text/javascript";
	_types["csv"] = "text/csv";
	_types["ics"] = "text/calendar";
	_types["py"] = "text/x-python";
	
	_types["gif"] = "image/gif";
	_types["png"] = "image/png";
	_types["jpeg"] = "image/jpeg";
	_types["jpg"] = "image/jpeg";
	_types["tif"] = "image/tiff";
	_types["tiff"] = "image/tiff";
	_types["bmp"] = "image/bmp";
	_types["webp"] = "image/webp";
	_types["avif"] = "image/avif";
	_types["ico"] = "image/vnd.microsoft.icon";
	_types["svg"] = "image/svg+xml";
	
	_types["webm"] = "video/webm";
	_types["ogv"] = "video/ogg";
	_types["mpeg"] = "video/mpeg";
	_types["mp4"] = "video/mp4";
	_types["avi"] = "video/x-msvideo";
	_types["ts"] = "video/mp2t";
	_types["3gp"] = "video/3gpp";
	_types["3g2"] = "video/3gpp2";
	
	_types["mp3"] = "audio/mpeg";
	_types["weba"] = "audio/webm";
	_types["oga"] = "audio/ogg";
	_types["wav"] = "audio/wav";
	_types["aac"] = "audio/aac";
	_types["opus"] = "audio/opus";
	_types["mid"] = "audio/midi";
	_types["midi"] = "audio/midi";
	
	_types["bin"] = "application/octet-stream";
	_types["gz"] = "application/gzip";
	_types["xhtml"] = "application/xhtml+xml";
	_types["xml"] = "application/xml";
	_types["pdf"] = "application/pdf";
	_types["abw"] = "application/x-abiword";
	_types["arc"] = "application/x-freearc";
	_types["azw"] = "application/vnd.amazon.ebook";
	_types["bz"] = "application/x-bzip";
	_types["bz2"] = "application/x-bzip2";
	_types["cda"] = "application/x-cdf";
	_types["csh"] = "application/x-csh";
	_types["doc"] = "application/msword";
	_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_types["eot"] = "application/vnd.ms-fontobject";
	_types["epub"] = "application/epub+zip";
	_types["jar"] = "application/java-archive";
	_types["json"] = "application/json";
	_types["jsonld"] = "application/ld+json";
	_types["mpkg"] = "application/vnd.apple.installer+xml";
	_types["odp"] = "application/vnd.oasis.opendocument.presentation";
	_types["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_types["odt"] = "application/vnd.oasis.opendocument.text";
	_types["ogx"] = "application/ogg";
	_types["php"] = "application/x-httpd-php";
	_types["ppt"] = "application/vnd.ms-powerpoint";
	_types["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_types["rar"] = "application/vnd.rar";
	_types["rtf"] = "application/rtf";
	_types["sh"] = "application/x-sh";
	_types["tar"] = "application/x-tar";
	_types["vsd"] = "application/vnd.visio";
	_types["xls"] = "application/vnd.ms-excel";
	_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_types["xul"] = "application/vnd.mozilla.xul+xml";
	_types["zip"] = "application/zip";
	_types["7z"] = "application/x-7z-compressed";
	
	_types["otf"] = "font/otf";
	_types["ttf"] = "font/ttf";
	_types["woff"] = "font/woff";
	_types["woff2"] = "font/woff2";
}

std::string	Client::setExtension( void ) const
{
	std::string	ext = "";
	std::string	path = _request.getPath();
	size_t	pos1 = path.find_last_of("/");
	size_t	pos2 = path.find_last_of(".");
	if (pos2 != std::string::npos)
	{
		if (pos2 > pos1)
			ext = path.substr(pos2, std::string::npos);
	}
	else
	{
		std::map<std::string, std::string> headerOpt = _request.getHeaderOptions();
		if (headerOpt.find("Content-Type") != headerOpt.end())
		{
			std::string	type = headerOpt.at("Content-Type");
			std::map<std::string, std::string>::const_iterator	ite = _types.end();
			std::map<std::string, std::string>::const_iterator	itb;
			for (itb = _types.begin(); itb != ite; itb++)
			{
				if (type == (*itb).second)
					ext = "." + (*itb).first;
			}
		}
	}
	return ext;
}

void	Client::clear_client() {

	_request.clear_client();
	//_location.clear();
	_statusCode.clear();
	_fullPath.clear();
	_body.clear();
	_fileType.clear();
	_response.clear();
	_toSend = 0;
	_responseReady = false;
	if (_env_malloc)
	{
		for (size_t i = 0; _env_malloc[i]; i++)
		{
			delete _env_malloc[i];
		}
		delete _env_malloc;
		_env_malloc = NULL;
	}
}
