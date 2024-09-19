NAME = webserv
CC = c++ -std=c++17
HEADER ?= $(shell find ./include -iname "*.hpp")
FLAGS = #-Wall -Wextra -Werror
FLAGS += #-fsanitize=address
SRCS := $(shell find ./src -iname "*.cpp")
# to keep the same dir structure in obj as in src
OBJS := $(patsubst ./src/%.cpp,./obj/%.o,$(SRCS))

# vpath searches all dirs and subdirs for .cpp files
vpath %.cpp $(sort $(dir $(SRCS)))

# colors
GREEN = \033[32;01m
BLUE = \033[34;01m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) -o $@ $^
	@printf "$(GREEN)Compiled $@$(RESET)\n"

obj/%.o: src/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) -c $(FLAGS) -I include -o $@ $<
	@printf "$(BLUE)Compiled $<$(RESET)\n"

clean:
	rm -rf obj

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
