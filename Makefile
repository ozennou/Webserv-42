NAME = webserv

OBJS = $(shell find . -name "*.cpp" | sed 's/\.cpp$$/.o/' | tr '\n' ' ')

FLAGS=  -Wshadow -Wall -Werror -Wextra -g #-fsanitize=address

CC=c++ -std=c++98 -Iinclude/ $(FLAGS)

HEADERS=$(shell find . -name "*.hpp" | tr '\n' ' ')

%.o : %.cpp $(HEADERS) Makefile
	$(CC) -c $< -o $@

$(NAME) : $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

all : $(NAME)

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(NAME)

re : fclean all

.PHONY : clean