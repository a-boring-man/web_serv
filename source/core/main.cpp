/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 11:46:32 by jrinna            #+#    #+#             */
/*   Updated: 2023/01/11 11:37:06 by jrinna           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "LjohnSon.hpp"
#include <istream>
#include <fstream>
#include "Cluster.hpp"

using std::cout;
using std::endl;

int main(int ac, char **argv, char **env) {

	if (ac != 2)
		return (1);
	try
	{
		Cluster	server_cluster(argv[1], env);
		cout << "launching" << endl;
		server_cluster.init_launch();
		server_cluster.launch();
		cout << "server ready" << endl;
	}
	catch(const Cluster::end_program& e)
	{
		std::cerr << e.what() << '\n';
	}
	catch(const Cluster::Exeption& e)
	{
		cout << "a critical execption has occur : ";
		std::cerr << e.what() << '\n';
	}
	catch(const LjohnSon::logic_error& e)
	{
		cout << "parsing error : " << endl;
		std::cerr << e.what() << '\n';
	}
	catch(const std::exception& e)
	{
		cout << "a critical execption has occur : ";
		std::cerr << e.what() << '\n';
	}
}