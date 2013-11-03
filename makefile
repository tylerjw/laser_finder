TARGET		= laser_finder
OBJECTS 	= dots_img.o no_dots_img.o laser_finder.o
#######################################################################################
CFLAGS   = -g -Wall
ASFLAGS  = -Wall
LDFLAGS  = -Wall

CC = gcc
AS = gcc
CXX = g++

########################################################################################
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

coordTest: dot.o coordTest.o
	$(CXX) dot.o coordTest.o $(LDFLAGS) -o coordTest

dot.o: dot.hpp
# coordTest.o: dot.hpp
laser_finder.o: images.h
dots_img.o: images.h
no_dots_img.o: images.h

clean: 
	rm -f $(OBJECTS) $(TARGET)