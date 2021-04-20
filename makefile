NAME = zfetch
BIN = bin
OBJ = obj

SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c,$(OBJ)/%.o,$(SOURCES))

FLAGS   = -Wall -Wextra -std=c99
LFLAGS  =

$(OBJ)/%.o: src/%.c
	@echo "\e[1mBuilding \e[4m$@\e[0m"
	@mkdir -p $(@D)
	@$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

$(BIN)/$(NAME): $(OBJECTS)
	@echo "\e[1mLinking \e[4m$@\e[0m"
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LFLAGS) $(LDFLAGS)

cleanobj:
	@echo "Cleaning objects"
	@rm -fv $(OBJECTS)

clean: cleanobj
	@echo "Cleaning executables"
	@rm -fv $(BIN)/$(NAME)

check: $(BIN)/$(NAME)
	mkdir -p .home/
	HOME=$(PWD)/.home $(BIN)/$(NAME) --regenerate-all
	PATH=$(PATH):$(PWD)/bin HOME=$(PWD)/.home $(BIN)/$(NAME)
	rm -r .home/

# vim: filetype=make tabstop=2 shiftwidth=2
