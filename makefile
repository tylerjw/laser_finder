TARGET		= laser_finder
OBJECTS 	= dots_img.o no_dots_img.o laser_finder.o
#######################################################################################
CFLAGS   = -g -Wall
ASFLAGS  = -Wall
LDFLAGS  = -Wall

CC = gcc
AS = gcc

########################################################################################
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

laser_finder.o: images.h
dots_img.o: images.h
no_dots_img.o: images.h

clean: 
	rm -f $(OBJECTS) $(TARGET)