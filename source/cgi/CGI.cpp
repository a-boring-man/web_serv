/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/16 13:36:24 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/13 13:39:47 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "utils.hpp"
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

/*
** ------------------------------- DO_THE_CGI --------------------------------
*/

std::string	Client::do_the_cgi( std::string const & fullPath ) {

	std::string	newBody = "";
	
	initiate_cgi_env(fullPath);
	newBody = call_cgi(fullPath);
	return newBody;
}

/*
** ------------------------------- INITIATE_CGI --------------------------------
*/

void	Client::initiate_cgi_env( std::string const & fullPath ) {

	(void)fullPath;
	//cerr << "initiate cgi" << endl;
	std::string    fileTypeTmp = "";
	std::map<int, std::string>	envm = env_to_map();
	std::map<std::string, std::string> headerOpt = _request.getHeaderOptions();
	/* for (unsigned long i = 0; i < envm.size(); i++)
	{
		cerr << "map : " << i << " : -" << envm[i] << "-" << endl;
	} */
	envm[envm.size()] = std::string("REDIRECT_STATUS=200");
	envm[envm.size()] = std::string("DOCUMENT_ROOT=") + _configServer.getRoot();
	if (_request.getBody().size())
		envm[envm.size()] = std::string("CONTENT_LENGH=") + std::to_string(_request.getBody().length());
	if (headerOpt.find("Content-Type") != headerOpt.end())
		fileTypeTmp = headerOpt.at("Content-Type");
    envm[envm.size()] = std::string("CONTENT_TYPE=") + fileTypeTmp;
	envm[envm.size()] = std::string("GATEWAY_INTERFACE=") + std::string("CGI/1.1");
	envm[envm.size()] = std::string("PATH_INFO=") + _request.getPath();
	envm[envm.size()] = std::string("PATH_TRANSLATED=") + fullPath;
	envm[envm.size()] = std::string("QUERY_STRING=") + _request.getQueries();
	envm[envm.size()] = std::string("REQUEST_METHOD=") + _request.getMethod();
	envm[envm.size()] = std::string("SCRIPT_NAME=") + fullPath;
	envm[envm.size()] = std::string("SERVER_NAME=weebserv");
	envm[envm.size()] = std::string("SERVER_PORT=") + std::to_string(_serverPort);
	envm[envm.size()] = std::string("SERVER_PROTOCOL=HTTP/1.1");
	envm[envm.size()] = std::string("SERVER_SOFTWARE=weebserv/1.0");
	if (headerOpt.find("Accept") != headerOpt.end())
        envm[envm.size()] = std::string("HTTP_ACCEPT=") + headerOpt.at("Accept");
    if (headerOpt.find("Accept-Language") != headerOpt.end())
        envm[envm.size()] = std::string("HTTP_ACCEPT_LANGUAGE=") + headerOpt.at("Accept-Language");
    if (headerOpt.find("User-Agent") != headerOpt.end())
        envm[envm.size()] = std::string("HTTP_USER_AGENT=") + headerOpt.at("User-Agent");
    if (headerOpt.find("Accept-Encoding") != headerOpt.end())
        envm[envm.size()] = std::string("HTTP_ACCEPT_ENCODING=") + headerOpt.at("Accept-Encoding");
    if (headerOpt.find("Accept-Charsets") != headerOpt.end())
        envm[envm.size()] = std::string("HTTP_ACCEPT_CHARSET=") + headerOpt.at("Accept-Charsets");
    envm[envm.size()] = std::string("REQUEST_URI=") + _request.getPath() + ((!_request.getQueries().empty()) ? ("?" + _request.getQueries()) : "");
    envm[envm.size()] = std::string("UPLOAD_PATH=") + _location.getRoot();
	map_to_env(envm);
}

/*
** ------------------------------- ENV_TO_MAP --------------------------------
*/

std::map<int, std::string>	Client::env_to_map() const {

	std::map<int, std::string>	my_map;
	for (unsigned long i = 0; _env[i]; i++)
	{
		my_map[my_map.size()] = _env[i];
	}
	return (my_map);
}

/*
** ------------------------------- ENV_TO_MAP --------------------------------
*/

void	Client::map_to_env( std::map<int, std::string> envm ) {

	char	**env = new char*[envm.size() + 1];

	env[envm.size()] = NULL;
	for (unsigned i = 0; i < envm.size(); i++)
	{
		env[i] = new char[envm[i].size() + 1];
		std::strcpy(env[i], envm[i].c_str());
		// std::cerr << env[i] << std::endl;
	}
	_env_malloc = env;
}

std::string	Client::call_cgi( std::string const & fullPath )
{
	pid_t       f_pid;
    std::string newBody;
    int         saveIn = dup(STDIN_FILENO);
    int         saveOut = dup(STDOUT_FILENO);
    FILE*       file_in = tmpfile();
    FILE*       file_out = tmpfile();
    int         fd_in = fileno(file_in);
    int         fd_out = fileno(file_out);

    if (write(fd_in, _request.getBody().c_str(), _request.getBody().size()) == -1)
        std::cerr << "ERROR: write" << std::endl;
    if (lseek(fd_in, 0, SEEK_SET) == -1)
        std::cerr << "ERROR: lseek" << std::endl;
    f_pid = fork();
    if (f_pid == -1)
        throw std::runtime_error("500");
    if (f_pid == 0) // child
    {
        std::string pass = _location.getFastcgipass();
        char    *argv[3];
        argv[0] = new char[_location.getFastcgipass().size() + 1];
        _location.getFastcgipass().copy(argv[0], _location.getFastcgipass().size());
        argv[0][_location.getFastcgipass().size()] = '\0';
        argv[1] = new char[fullPath.size() + 1];
        fullPath.copy(argv[1], fullPath.size());
        argv[1][fullPath.size()] = '\0';
        argv[2] = NULL;
        // std::cerr << "argv[0]: " << argv[0] << ", argv[1]: " << argv[1] << std::endl;
        // char        ttmp[1024];
        // ft_memset(ttmp, 0, 1024);
        // read(fd_in, ttmp, _request.getBody().size());
        // cerr << "esque c possible de lire fdin : |" << ttmp << "|" << endl;
        if (dup2(fd_in, STDIN_FILENO) == -1)
            std::cerr << "ERROR: dup2in" << std::endl;
        if (dup2(fd_out, STDOUT_FILENO) ==  -1)
            std::cerr << "ERROR: dup2out" << std::endl;
        // std::cerr << "pass: " << pass.c_str() << std::endl;
		close(fd_in);
        execve(pass.c_str(), argv, _env_malloc);
        std::cerr << errno << ": " << strerror(errno) << std::endl;
        write(STDOUT_FILENO, "500\0", 4);
        exit(0);
    }
    else
    {
        waitpid(-1, NULL, 0);
        lseek(fd_out, 0, SEEK_SET);
        char    buf[1024];
        ft_memset(buf, 0, 1024);
        int     ret = read(fd_out, buf, 1024);
        while (ret > 0)
        {
            for (int i = 0; i < ret; i++)
                newBody += buf[i];
            ret = read(fd_out, buf, 1024);
        }
        std::cerr << "|" << newBody << "|" << std::endl;
    }
    dup2(saveIn, STDIN_FILENO);
    dup2(saveOut, STDOUT_FILENO);
    fclose(file_in);
    fclose(file_out);
    close(fd_in);
    close(fd_out);
    close(saveIn);
    close(saveOut);
    return (newBody);
}
