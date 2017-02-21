PRO_DIR		=	$(PWD)
DEPS_DIR	=	$(PRO_DIR)/deps
LINE_DIR	=	$(DEPS_DIR)/line
HM_DIR		=	$(DEPS_DIR)/hashmap
SDS_DIR 	=	$(DEPS_DIR)/sds
NETWORK_DIR	=	$(PRO_DIR)/network
SYS_DIR		=	$(PRO_DIR)/sys
CMD_DIR		=	$(PRO_DIR)/cmd

OBJS=linenoise.o hashmap.o sds.o network.o system.o cmd.o  cli.o

DEBUG=-g

all:$(OBJS)
	cc -Wall -O2 $(LINE_DIR)/linenoise.o		  				\
	$(HM_DIR)/hashmap.o $(SDS_DIR)/sds.o 						\
	$(NETWORK_DIR)/gateway.o $(SYS_DIR)/system.o				\
	$(NETWORK_DIR)/network.o  $(CMD_DIR)/cmd.o					\
	cli.o -o cli $(DEBUG)
	@echo "make success."
cli.o:
	cc -c cli.c -I $(LINE_DIR) -I $(HM_DIR) -I $(NETWORK_DIR) 	\
	-I $(CMD_DIR) -Wall -O2
linenoise.o:
	cd $(LINE_DIR) && make
hashmap.o:
	cd $(HM_DIR) && make
sds.o:
	cd $(SDS_DIR) && make
network.o:
	cd $(NETWORK_DIR) && make DEPS_DIR=$(DEPS_DIR)
cmd.o:
	cd $(CMD_DIR) && make PRO_DIR=$(PRO_DIR) DEPS_DIR=$(DEPS_DIR)
system.o:
	cd $(SYS_DIR) && make
install:
	cp -af cli /usr/local/bin
clean:
	cd $(SDS_DIR) && make clean
	cd $(HM_DIR) && make clean
	cd $(LINE_DIR) && make clean
	cd $(NETWORK_DIR) && make clean
	cd $(CMD_DIR) && make clean
	cd $(SYS_DIR) && make clean
	rm -f cli.o cli
