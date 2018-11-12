CC=gcc
CFLAGS=-I.
DEPS =
OBJ = a3p1shell1 a3p1shell2 a3p1shell3
All: $(OBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ): %: %.c
	$(CC) -o $@ $< $(CFLAGS)