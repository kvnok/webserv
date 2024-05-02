NAME = webserv
CC = c++
HEADER ?= $(shell find ./include -iname "*.hpp")
FLAGS = #-Wall -Wextra -Werror
SRCS := $(shell find ./src -iname "*.cpp")
OBJS := $(addprefix obj/,$(notdir $(SRCS:.cpp=.o)))
CHECKFLAGS = valgrind --leak-check=full

all: $(NAME)

check: all
	$(CHECKFLAGS) ./$(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) -o $@ $^

obj/%.o: src/%.cpp $(HEADER)
	@mkdir -p obj
	@$(CC) -c $(FLAGS) -o $@ $<

clean:
	rm -rf obj

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
