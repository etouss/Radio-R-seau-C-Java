#include <stdio.h>
#include <string.h>
#include "partieHeuristique.h"
#include "Jouer.h"
#include "modeTest.h"

int main(int argc, const char * argv[])
{
    if(argv[1]==NULL)jouer_radio(NULL);
    if(strcmp(argv[1],"heuri")==0)
    partiH();
    if(strcmp(argv[1],"-t")==0)modeTest(argv[2]);
    return 0;
}
