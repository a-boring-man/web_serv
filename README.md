# web_serv

This is an end-of-course project at 42 School.
The goal of the project is to build a program that takes as input a JSON configuration file
that specifies which virtual server will respond to which port, what to look for
in case a folder is requested by the GET method, what CGI to use, and more.
The program then parses the configuration file and creates multiple virtual HTTPS servers.

## Usage

Go to the ```config``` folder and change the paths in all the ```root``` and ```fastcgipass``` options to the appropriate locations.

You can also play with the other options if you want to see what happens.
Then, go back to the root of the project folder and enter the command make.

The server should start up and the console should display the number of correctly configured virtual servers.
You can then go to your localhost address in a browser to see the results.
