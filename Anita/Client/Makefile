NAME = client

#SRC_DIR = src

SRC = client-main.cpp \
			Client.cpp \


CXX = c++
STDFLAG = -std=c++98 -pedantic
CXXFLAGS = -Wall -Wextra -Werror -g #-I./inc
RM = rm

OBJ = $(SRC:.cpp=.o)

PRUNE = git fetch --prune

all : $(NAME)

$(NAME) : $(OBJ)
	$(PRUNE)
	$(CXX) $(CXXFLAGS) $(STDFLAG) $(OBJ) -o $(NAME)

clean :
	$(RM) -rf $(OBJ)

fclean : clean
	$(RM) -rf $(NAME)

re : fclean all

.PHONY: all fclean clean re
