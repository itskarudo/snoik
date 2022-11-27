SOURCES := snoik.c
CFLAGS := -ggdb -fsanitize=address,undefined
LIBS := -lcurses

BIN := snoik

$(BIN): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS) -o $(BIN)
