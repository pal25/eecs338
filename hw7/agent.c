#include <rpc/rpc.h>
#include "Smoking.h"

static int paper = 10;
static int matches = 10;
static int tobacco = 10;

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
    return &result;
}

int* get_me_my_supply_1_svc(data* msg, struct svc_req *req)
{
    CLIENT* client = NULL;
    printf("Getting request from PID=%d\n", msg->smoker);
    return get_me_my_supply_1(msg, client);
}
