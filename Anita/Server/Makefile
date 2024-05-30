NAME = ircserv

SRC = main.cpp \
			Server.cpp \
			Client.cpp  \
			Channel.cpp 


CXX = c++
STDFLAG = -std=c++98 -pedantic
CXXFLAGS = -Wall -Wextra -Werror -g -I./inc
RM = rm

OBJ = $(SRC:.cpp=.o)

#PRUNE = git fetch --prune

all : $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(STDFLAG) $(OBJ) -o $(NAME)
clean :
	$(RM) -rf $(OBJ)

fclean : clean
	$(RM) -rf $(NAME)

re : fclean all

run : 
	./$(NAME) 6667 1234

.PHONY: all fclean clean re run
