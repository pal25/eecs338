CFLAGS = -g -Werror -std=gnu99
BIN = agent smoker
GEN = Smoking_clnt.c Smoking_svc.c Smoking_xdr.c Smoking.h 
RPCCOM = rpcgen

all: $(BIN)

smoker: smoker.c Smoking_clnt.c Smoking_xdr.c Smoking.h
	gcc $(CFLAGS) smoker.c Smoking_clnt.c Smoking_xdr.c -o $@

agent: agent.c Smoking_svc.c Smoking_xdr.c Smoking.h
	gcc $(CFLAGS) agent.c Smoking_svc.c Smoking_xdr.c -o $@

$(GEN): Smoking.x
	$(RPCCOM) -C Smoking.x

clean:
	rm -f $(GEN) *.o $(BIN)