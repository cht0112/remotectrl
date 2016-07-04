CC= gcc
XML_PATH = xml/
INCLUDE_OPT = -Ixml 
#CPPFLAGS =  -g -c -O2 -Wall
FLAGS =  -g -c -Wall -D__STDC_CONSTANT_MACROS

OBJECTS = remotectrl.o remotehash.o

supernode: $(OBJECTS)
	 $(CC) -g -o supernode $(OBJECTS) -lpthread

remotectrl.o: remotectrl.c remotectrl.h
	$(CC) $(INCLUDE_OPT) $(FLAGS) remotectrl.c

remotehash.o: remotehash.c remotehash.h
	$(CC) $(INCLUDE_OPT) $(FLAGS) remotehash.c
