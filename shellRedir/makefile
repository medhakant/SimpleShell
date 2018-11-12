CC=gcc
CFLAGS=-I.
DEPS =
OBJ = a3p2redir1 a3p3redir2
All: $(OBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ): %: %.c
	$(CC) -o $@ $< $(CFLAGS)