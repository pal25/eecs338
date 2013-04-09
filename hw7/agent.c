#include <rpc/rpc.h>
#include "Smoking.h"

static int paper = 100;
static int matches = 100;
static int tobacco = 100;

static int result;

int check_and_decrement(int* value, int amount)
{
    if(*value >= amount) {
	*value -= amount;
	return 1;
    } else {
	return 0;
    }
}

int* get_me_my_supply_1(data* msg, CLIENT* client)
{
    int* value;
    switch(msg->supply_type) {
    case (PAPER):
	value = &paper;
	break;
    case(TOBACCO):
	value = &tobacco;
	break;
    case(MATCHES):
	value = &matches;
	break;
    }

    result = check_and_decrement(value, msg->supply_amount);
    printf("Remaining: P=%d, T=%d, M=%d\n", paper, tobacco, matches);
    return &result;
}

int* get_me_my_supply_1_svc(data* msg, struct svc_req *req)
{
    CLIENT* client = NULL;
    printf("Request: PID=%d, Type=%d, Amount=%d\n", msg->smoker, msg->supply_type, msg->supply_amount);

    if(paper == 0 && tobacco == 0 && matches ==0) {
	printf("No more resources...\n");
    }
    
    return get_me_my_supply_1(msg, client);
}
