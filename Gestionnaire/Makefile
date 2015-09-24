
SOURCES= gestionnaire.c listDc.c com_client.c com_diff.c
OBJECTS=$(SOURCES:.c=.o)
CFLAGS= -Wall -pthread -g -DDEBUG
EXECUTABLE=gestionnaire

$(EXECUTABLE): $(OBJECTS)
	gcc -o $(EXECUTABLE) $(CFLAGS) $(OBJECTS)

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

cleanall:
	rm -f $(EXECUTABLE) $(OBJECTS)
