# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/12 14:53:55 by kichkiro          #+#    #+#              #
#    Updated: 2024/04/26 13:36:01 by kichkiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Config ---------------------------------------------------------------------->

NAME        := webserv
SRCS        := \
				directives/ADirective.cpp \
				directives/Alias.cpp \
				directives/Autoindex.cpp \
				directives/ClientMaxBodySize.cpp \
				directives/ErrorPage.cpp \
				directives/Http.cpp \
				directives/Include.cpp \
				directives/Listen.cpp \
				directives/LimitExcept.cpp \
				directives/Location.cpp \
				directives/Rewrite.cpp \
				directives/Root.cpp \
				directives/Server.cpp \
				directives/ServerName.cpp \
				directives/Index.cpp\
				directives/FastCGIPass.cpp \
				\
				cgi/Cgi.cpp \
				\
				logging/Log.cpp \
				\
				socket/Socket.cpp \
				\
				config/ConfigFile.cpp \
				\
				main/WebServer.cpp \
				main/main.cpp \
				main/utils.cpp
SRC_DIR     := src
SRCS        := $(SRCS:%=$(SRC_DIR)/%)
BUILD_DIR   := .build
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS        := $(OBJS:.o=.d)
INCS        := ./include
CC          := c++
CFLAGS      := -g -std=c++98 -Wall -Wextra -Werror
CPPFLAGS    := $(addprefix -I,$(INCS)) -MMD -MP
RM          := rm -rf
MAKEFLAGS   := --no-print-directory
DIR_DUP     = mkdir -p $(@D)

.PHONY: all clean fclean re
.SILENT:

# Rules ----------------------------------------------------------------------->

all: banner $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)
	${info }
	${info ${BOLD}Creating  -> ${WHITE}$(NAME)${NO_COLOR}}

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(DIR_DUP)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
	${info ${BOLD}Compiling -> ${WHITE}$@${NO_COLOR}}

-include $(DEPS)

tidy:
	clang-tidy -header-filter=.* -system-headers $(SRCS) -- $(CFLAGS) $(CPPFLAGS)

clean:
	$(RM) $(BUILD_DIR)
	${info }
	${info ${BOLD}${YELLOW}Cleaning in progress...${NO_COLOR}    🧹 🧹 🧹}

fclean: clean
	$(RM) $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

# Style ----------------------------------------------------------------------->

YELLOW		:= ${shell tput setaf 3}
NO_COLOR	:= ${shell tput sgr0}
BOLD		:= ${shell tput bold}

banner:
	${info ${BOLD}${YELLOW} }
	${info ██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗ }
	${info ██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║ }
	${info ██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║ }
	${info ██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝ }
	${info ╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝  }
	${info ${} ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝   }
	${info ${NO_COLOR} }

# ------------------------------------------------------------------------------
