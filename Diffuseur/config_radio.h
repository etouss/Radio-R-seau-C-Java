
#ifndef __config_radio__
#define __config_radio__

#include <stdlib.h>
#include "gounki_ia.h"
#include "read_story.h"
#include "write_story.h"
#include "gounki_a.h"
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include "debug.h"
#include "guess_number.h"

typedef int (*Process_input_client)(char * mess_client,int sock_client,char * address_caller);
typedef int (*Process_input_diff)(char * mess_diff);
typedef char * (*Process_output_multicast)(void);

#endif

void init_config_radio();

