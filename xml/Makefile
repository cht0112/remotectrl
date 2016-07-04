CC = g++
OBJECTS = tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o
CFLAGS = -g -c -D NDEBUG -Wall -O2  -D_FILE_OFFSET_BITS=64
TARGET = ../libtinyxml.a

$(TARGET): $(OBJECTS)
	ar r $(TARGET) $(OBJECTS)

tinyxml.o: tinyxml.cpp tinyxml.h
	$(CC) $(CFLAGS) tinyxml.cpp

tinystr.o: tinystr.cpp tinystr.h
	$(CC) $(CFLAGS) tinystr.cpp

tinyxmlparser.o: tinyxmlparser.cpp tinyxml.h
	$(CC) $(CFLAGS) tinyxmlparser.cpp

tinyxmlerror.o: tinyxmlerror.cpp tinyxml.h
	$(CC) $(CFLAGS) tinyxmlerror.cpp

.PHONY: clean
clean:
	-rm $(OBJECTS)
	-rm $(TARGET)
