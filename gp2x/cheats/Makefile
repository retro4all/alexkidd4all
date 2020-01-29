NAME	  = addcheat

CC = 	gcc
STRIP = strip
LIBS = -lz -static

OBJECTS   = ioapi.o unzip.o fileio.o addcheat.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	$(STRIP) $(NAME)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)