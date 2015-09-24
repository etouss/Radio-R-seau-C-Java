
SOURCES= diffuseur.c debug.c com_gestionnaire.c com_client.c radio.c email.c config_radio.c format_mess.c vote_lib.c read_story.c write_story.c gounki_ia.c gounki_a.c guess_number.c Gounki/grille.c Gounki/heuristique.c Gounki/Jouer.c Gounki/partieHeuristique.c Gounki/persistance.c Gounki/pion.c Gounki/test.c Gounki/modeTest.c
OBJECTS=$(SOURCES:.c=.o)
CFLAGS= -Wall -pthread -g -DDEBUG -DNC
EXECUTABLE=diffuseur

$(EXECUTABLE): $(OBJECTS)
	gcc -o $(EXECUTABLE) $(CFLAGS) $(OBJECTS)

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

cleanall:
	rm -f $(EXECUTABLE) $(OBJECTS)
