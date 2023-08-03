# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jrinna <jrinna@student.42lyon.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/03 10:05:32 by jrinna            #+#    #+#              #
#    Updated: 2023/08/03 12:17:43 by jrinna           ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

#update on every project
LST_SRC :=	cgi/CGI \
			client/Client client/ClientUtils \
			config/Config config/ConfigServer config/LjohnSon config/Location \
			core/main \
			message/CreateResponse message/Methods message/Request \
			other/Router other/utils \
			server/Cluster server/Server server/ServerMethode \

NAME := weebserv

#update to match c or cpp
CC := c++
FILE_EXT := .cpp
HEADER_EXT := .hpp
CPPFLAGS := #-std=c++98 #-pedantic

#update if needed
CFLAGS = -Wall -Wextra -Werror -MD -I$(DIR_INC) -g3 #-fsanitize=address -g3
DIR_SRC := source#.
SUB_DIR_LST := core test cgi client config message other server

#shouldn't need to update
RM := rm -rf
MD := mkdir -p

DIR_INC := include
DIR_OBJ := .object

OBJ=$(addprefix $(DIR_OBJ)/,$(addsuffix .o,$(LST_SRC)))
DEP=$(addprefix $(DIR_OBJ)/,$(addsuffix .d,$(LST_SRC)))
SUB_DIR=$(addprefix $(DIR_OBJ)/,$(SUB_DIR_LST))

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(DIR_OBJ)/%.o : $(DIR_SRC)/%$(FILE_EXT) Makefile | $(SUB_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

$(SUB_DIR) :
	$(MD) $@

clean :
	$(RM) $(DIR_OBJ)

fclean : clean
	$(RM) $(NAME)

re : fclean all

.PHONY : all clean fclean re

-include $(DEP)
