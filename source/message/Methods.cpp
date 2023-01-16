/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 11:39:07 by ccartet           #+#    #+#             */
/*   Updated: 2023/01/13 13:24:07 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

/*
** ---------------------------- GET_METHOD -----------------------------
*/

void	Client::methodGet( std::string fullPath )
{
	std::string fullPathIndex = "";
	int			ret = 0;
	int			indexOk = 0;
	
	if (_location.fastcgipass_has_been_set) // ci cgi pass, do the cgi thing
		_body = do_the_cgi(fullPath);
	else if (checkPath(fullPath) == true) // si c'est un dossier accessible
	{
		//cerr << "strat of GET methode\n" ;
		if (_location.defaultfileifdirectory_has_been_set) // si on cherche un certain fichier si on accept la lecture de dossier
		{
			if (fullPath.find_last_of('/') != fullPath.size() - 1) // rajout un / a la fin s'il y en a pas
				fullPath +=	"/";
			std::vector<std::string>	index = _location.getDefaultfileifdirectory();
			for (unsigned long i = 0; i < index.size(); i++) // parcour les fichier par default
			{
				fullPathIndex = fullPath + index[i];
				int	ret = getFile(fullPathIndex); // check if a file is found
				if (ret == 1)
				{
					indexOk = 1;
					break;
				}
				fullPathIndex.clear();
			}
		}
		if (!indexOk) // if no file are to be found
		{
			if (!_location.getDirectorylistening()) // si pas de fichier et pas d'autoindex
				throw std::runtime_error("404");
			else
				generateIndex(fullPath);
		}
	}
	else // si fichier
	{
		ret = getFile(fullPath); // chope le fichier
		if (ret == 403) // si pas de droit d'acces
			throw std::runtime_error("403");
	}
}



/*
** ---------------------------- DELETE_METHOD -----------------------------
*/

void	Client::methodDelete( std::string const & fullPath )
{
	if (!file_exist(fullPath)) // si pas de fichier
		throw std::runtime_error("404");
	if (std::remove(fullPath.c_str())) // no access ou alors dossier contenant des fichiers 
		throw std::runtime_error("403");
	_statusCode = "204";
}

/*
** ---------------------------- POST_METHOD -----------------------------
*/

void	Client::methodPost( std::string const & fullPath )
{
	_statusCode = "201";
	if (_request.getBody().size() > _configServer.getClientmaxbodysize()) // if exxed the client max body size
		throw std::runtime_error("413");
	if (_location.fastcgipass_has_been_set) // if cgi
	{
		if (!file_exist(fullPath)) // if no file
			throw std::runtime_error("404");
		_body = do_the_cgi(fullPath);
		if (_body.size() == 0) // request traiter mais body vide
			_statusCode = "204";
		else
			_fileType = "text/plain";
	}
	else
	{
		if (isDirectory(fullPath) == true && _request.multipartBody_has_been_set == true) // si multipart
		{
			std::vector<s_MultipartBody>	multiBody = _request.getMultiBody();
			size_t	i = 0;
			
			while (i < multiBody.size())
			{
				if (multiBody[i].miniHeader.find("filename") != multiBody[i].miniHeader.end())
				{
					std::string		fileName = multiBody[i].miniHeader.at("filename");
					std::string		fileToCreate = fullPath + fileName.substr(1, fileName.size() - 2);
					toCreate(fileToCreate, multiBody[i].body);
				}
				i++;
			}
		}
		else if (isDirectory(fullPath) == true) // if directory
			throw std::runtime_error("400");
		else // sinon cree le fichier
			toCreate(fullPath, _request.getBody());		
	}
}

/*
** ---------------------------- GENERATE_INDEX -----------------------------
*/

void	Client::generateIndex( std::string const & fullPath )
{
	DIR				*dir = opendir(fullPath.c_str());
	if (!dir)
		throw std::runtime_error("403");
	struct dirent	*dirent = NULL;
	std::string		tmp = "";
	
	_body = "<h1>Index of " + _request.getPath() + "</h1>";
	_body += "<ul>";
	while ((dirent = readdir(dir)) != NULL)
	{
		tmp = dirent->d_name;
		if (!(!tmp.compare(".") || !tmp.compare("..")))
		{
			_body += "<li>";
			if (dirent->d_type == DT_DIR)
				tmp += "/";
			_body += "<a href=\"http://127.0.0.1:" + std::to_string(_serverPort) + _request.getPath() + tmp + "\">" + tmp + "</a>";
			_body += "</li>";
		}
		tmp.clear();
	}
	_body += "</ul>";
	closedir(dir);
	_fileType = "text/html";
}

/*
** ---------------------------- UTILS -----------------------------
*/

int	Client::getFile( std::string const & fullPath )
{
	std::ifstream		in(fullPath.c_str());
	std::stringstream	buf;
	std::string			tmp;
	
	access(fullPath.c_str(), F_OK);
	tmp = setFileType(fullPath);
	if (access(fullPath.c_str(), F_OK) == -1) // si pas trouver
		return 404;
	else if (access(fullPath.c_str(), R_OK) == -1) // si pas de droit d'acces
		return 403;
	buf << in.rdbuf();
	_body = buf.str();
	in.close();
	_fileType = tmp;
	return 1;
}

std::string	Client::setFileType( std::string const & filePath ) const
{
	std::string	ext = "";
	size_t		pos1 = 0;
	size_t		pos2 = 0;
	std::string	fileType = "";
	
	pos1 = filePath.find_last_of("/");
	pos2 = filePath.find_last_of(".");
	if (pos2 == std::string::npos)
		fileType = "text/plain";
	else
	{
		if (pos2 > pos1)
		{
			ext = filePath.substr(pos2 + 1, std::string::npos);
			if (_types.find(ext) != _types.end())
				fileType = _types.at(ext);		
		}
	}
	//std::cerr << fileType << std::endl;
	return fileType;
}

int	Client::toCreate( std::string const & fullPath, std::string const & body )
{
	std::ofstream	outfile;
	struct stat		buf;
	int				ret = stat(fullPath.c_str(), &buf);
	
	if (ret == -1 && errno == ENOENT) // le fichier n'existe pas
	{
		outfile.open(fullPath, std::ios_base::out);
		if (outfile.is_open() == false)
			throw std::runtime_error("403");
		outfile << body;
		outfile.close();
	}
	else if (ret != -1 && S_ISREG(buf.st_mode))
		throw std::runtime_error("409");
	if (_statusCode == "201")
	{
		_body = "File has been created";
		_fileType = "text/plain";
	}
	return (0);
}
