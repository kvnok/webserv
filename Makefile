NAME = webserv
CC = c++
HEADER ?= $(shell find ./include -iname "*.hpp")
FLAGS = #-Wall -Wextra -Werror
SRCS := $(shell find ./src -iname "*.cpp")
# to keep the same dir structure in obj as in src
OBJS := $(patsubst ./src/%.cpp,./obj/%.o,$(SRCS))
CHECKFLAGS = valgrind --leak-check=full

# vpath searches all dirs and subdirs for .cpp files
vpath %.cpp $(sort $(dir $(SRCS)))

all: $(NAME)

check: all
	$(CHECKFLAGS) ./$(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) -o $@ $^
	@printf "\033[32;01mCompiled $@\033[0m\n"

obj/%.o: src/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) -c $(FLAGS) -I include -o $@ $<
	@printf "\033[34;01mCompiled $<\033[0m\n"

clean:
	rm -rf obj

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
