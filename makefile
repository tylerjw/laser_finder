TARGET		= laser_finder
OBJECTS 	= dots_img.o no_dots_img.o laser_finder.o point.o
#######################################################################################
CFLAGS   = -g -Wall
CXXFLAGS = -g -Wall
ASFLAGS  = -Wall
LDFLAGS  = -g -Wall

CC = gcc
AS = gcc
CXX = g++

########################################################################################
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

coordTest: point.o coordTest.o
	$(CXX) point.o coordTest.o $(LDFLAGS) -o coordTest

point.o: point.h
laser_finder.o: images.h point.h
dots_img.o: images.h
no_dots_img.o: images.h

clean: 
	rm -f $(OBJECTS) $(TARGET)