NETWORK_DIR	=	${PRO_DIR}/network
SYS_DIR		=	${PRO_DIR}/sys
SDS_DIR     =   ${DEPS_DIR}/sds
HM_DIR		=	${DEPS_DIR}/hashmap

all:
	@echo $(NETWORK_DIR)
	cc -c cmd.c -I $(NETWORK_DIR) -I $(SDS_DIR) -I $(HM_DIR) -I $(SYS_DIR) -Wall -O2
clean:
	rm -f cmd.o
