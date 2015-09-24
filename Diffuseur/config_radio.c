
#include "config_radio.h"

Process_input_client process_input_client[5];
Process_output_multicast process_output_multicast[5];
Process_input_diff process_input_diff[5];
int multicaste_delay[5];

void init_config_radio()
{
    process_input_client[0] = process_input_client_read_story;
    process_input_client[1] = process_input_client_gounki_ia;
    process_input_client[2] = process_input_client_write_story;
    process_input_client[3] = process_input_client_gounki_a;
    process_input_client[4] = process_input_client_guess_number;
    process_output_multicast[0] = read_story;
    process_output_multicast[1] = send_plateau_ia;
    process_output_multicast[2] = write_story;
    process_output_multicast[3] = send_plateau_a;
    process_output_multicast[4] = send_recap_guess;
    multicaste_delay[0] = 5;
    multicaste_delay[1] = 10;
    multicaste_delay[2] = 10;
    multicaste_delay[3] = 10;
    multicaste_delay[4] = 5;
    process_input_diff[0] = NULL;
    process_input_diff[1] = NULL;
    process_input_diff[2] = NULL;
    process_input_diff[3] = process_input_diffu_gounki_a;
    process_input_diff[4] = NULL;
}

