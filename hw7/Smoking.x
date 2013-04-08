enum type {
     PAPER = 0,
     TOBACCO = 1,
     MATCHES = 2
};
typedef enum type type;

struct data {
       type supply_type;
       int supply_amount;
       int smoker;
};
typedef struct data data;
       

program SMOKE {
	version AS7 {
		int GET_ME_MY_SUPPLY(data) = 1;
	} = 1;
} = 25;