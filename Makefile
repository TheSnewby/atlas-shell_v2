SRC = colors.h custom_commands.c env.c env2.c main.c main.h more_funcs.c shell_loop.c

all:
	gcc $(SRC) -o hsh