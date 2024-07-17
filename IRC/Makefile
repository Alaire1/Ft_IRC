
NAME = ircserv

SRC = main.cpp\
			Server.cpp\
			Client.cpp\
			Channel.cpp\

SRC_DIR = src

CXX = c++
STDFLAG = -std=c++98 -pedantic
CXXFLAGS = -Wall -Wextra -Werror -g -I./inc
RM = rm

OBJ_DIR = obj

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

#PRUNE = git fetch --prune

define ASCII_ART
                    ,----,                                    
                  ,/   .`|                                    
      ,---,.    ,`   .'  :        ,---,,-.----.     ,----..   
    ,'  .' |  ;    ;     /     ,`--.' |\    /  \   /   /   \  
  ,---.'   |.'___,/    ,'      |   :  :;   :    \ |   :     : 
  |   |   .'|    :     |       :   |  '|   | .\ : .   |  ;. / 
  :   :  :  ;    |.';  ;       |   :  |.   : |: | .   ; /--`  
  :   |  |-,`----'  |  |       '   '  ;|   |  \ : ;   | ;     
  |   :  ;/|    '   :  ;       |   |  ||   : .  / |   : |     
  |   |   .'    |   |  '       '   :  ;;   | |  \ .   | '___  
  '   :  '      '   :  |   ___ |   |  '|   | ;\  \'   ; : .'| 
  |   |  |      ;   |.' .'  .`|'   :  |:   ' | \.''   | '/  : 
  |   :  \      '---'.'  .'   :;   |.' :   : :-'  |   :    /  
  |   | ,'        ,---, '   .' '---'   |   |.'     \   \ .'   
  `----'          ;   |  .'            `---'        `---`     
                  `---'                                       


endef
export ASCII_ART

RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
RESET = \033[0m
BANNER_IRC = @echo "$$ASCII_ART"

all : $(NAME)

$(NAME) : $(OBJ)
	@echo "$(BLUE)";
	$(BANNER_IRC)
	@echo "$(GREEN)ft_irc's compilation done...$(RESET)";
	@$(CXX) $(CXXFLAGS) $(STDFLAG) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	$(RM) -rf $(OBJ)

fclean : clean
	$(RM) -rf $(NAME)

re : fclean all

run : $(NAME)
	./$(NAME) 6667 1234

.PHONY: all fclean clean re run