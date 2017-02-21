# get DEPS_DIR from make cmd 
HM_DIR      =   ${DEPS_DIR}/hashmap
SDS_DIR     =   ${DEPS_DIR}/sds
all:
	cc -c gateway.c network.c -I $(HM_DIR) -I $(SDS_DIR) -Wall
clean:
	rm -f *.o
