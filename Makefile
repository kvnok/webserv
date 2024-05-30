NAME = webserv
CC = c++
HEADER ?= $(shell find ./include -iname "*.hpp")
FLAGS = #-Wall -Wextra -Werror
SRCS := $(shell find ./src -iname "*.cpp")
OBJS := $(addprefix obj/,$(notdir $(SRCS:.cpp=.o)))
CHECKFLAGS = valgrind --leak-check=full

# vpath searches all dirs and subdirs for .cpp files
vpath %.cpp $(sort $(dir $(SRCS)))

all: $(NAME)

check: all
	$(CHECKFLAGS) ./$(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) -o $@ $^

obj/%.o: %.cpp $(HEADER)
	@mkdir -p obj
	@$(CC) -c $(FLAGS) -I include -o $@ $<

clean:
	rm -rf obj

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
