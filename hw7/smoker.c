#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "Smoking.h"

int paper = 0;
int tobacco = 0;
int matches = 0;

int check_and_request(int* value, data* msg, CLIENT* clnt)
{
    int* result = get_me_my_supply_1(msg, clnt);
    if(result == NULL) {
	clnt_perror(clnt, "RPC Failed...");
	exit(EXIT_FAILURE);
    }

    if(*result) {
	*value = *value + msg->supply_amount;
	return 1;
    } else {
	return 0;
    }
}

data *msg_setup(int type, int amount)
{
    data* msg;
    msg->supply_type = type;
    msg->supply_amount = amount;
    msg->smoker = (int) getpid();

    return msg;
}

void smoke()
{
    paper -= 1;
    tobacco -= 1;
    matches -= 1;
    
    printf("Smoking! Resources Left: P=%d, T=%d, M=%d\n", paper, tobacco, matches);
    sleep(2);
}

CLIENT *clnt_create_checked(char* host, unsigned long program, unsigned long version, char* proto)
{
   CLIENT* clnt;
    clnt = clnt_create(host, program, version, proto);
    if(clnt == NULL) {
	clnt_pcreateerror(host);
	exit(EXIT_FAILURE);
    }

    return clnt;
}

int main(int argc, char** argv)
{
    char* host;
    if(argc < 2) {
	printf("Usage: %s server_host\n", argv[0]);
	exit(EXIT_FAILURE);
    } else {
	host = argv[1];
    }
    
    CLIENT* clnt = clnt_create_checked(host, SMOKE, AS7, "udp");
    data* paper_msg = msg_setup(PAPER, 1);
    data* tobacco_msg = msg_setup(TOBACCO, 1);
    data* matches_msg = msg_setup(MATCHES, 1);

    int resources_available = 1;
    while(resources_available) {
	if(paper == 0) {
	    if(!check_and_request(&paper, paper_msg, clnt)) {
		printf("No more paper available\n");
		resources_available = 0;
	    }
	} else if(tobacco == 0) {
	    if(!check_and_request(&tobacco, tobacco_msg, clnt)) {
		printf("No more tobacco available\n");
		resources_available = 0;
	    }
	} else if(matches == 0) {
	    if(!check_and_request(&matches, matches_msg, clnt)) {
		printf("No more matches available\n");
		resources_available = 0;
	    }
	} else {
	    smoke();
	}
    }

    return 0;
}
