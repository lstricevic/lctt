# Corba Test Tool Makefile v0.2
#
# Commands start with TAB not spaces
#

# Here set omniORB include and library dirs
INC_DIR=include
LIB_DIR=lib

CC=g++

OBJS=ctt.o echoSK.o fgetopt.o
COMP_OPT=-I$(INC_DIR) -c
LINK_OPT=-L$(LIB_DIR)
export LD_LIBRARY_PATH=$(LIB_DIR)

LIBS=omniORB4 

ctt: $(OBJS)
	$(CC) $(LINK_OPT) -l $(LIBS) -o $@ $^ 

ctt.o: ctt.cc timer.cc options.cc
	$(CC) $(COMP_OPT) -o $@ $<
             
echoSK.o: echoSK.cc
	$(CC) $(COMP_OPT) -o $@ $< 

fgetopt.o: fgetopt.c 

.PHONY: clean

clean:
	rm -f ctt $(OBJS)

