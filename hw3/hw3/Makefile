#makefile for creating the 'ttftps' program

#Flags
CC = gcc
CFLAGS = -g -Wall
CCLINK = $(CC)
OBJS = ttftps.o
TARGET = ttftps
RM = rm -f

#Default target
all: $(TARGET)

#link the final excecutable
$(TARGET): $(OBJS)
	$(CCLINK) -o $(TARGET) $(OBJS)
	 

# compile ttftps.c to create ttftps.o
 ttftps.o: ttftps.c 
	$(CC) $(CFLAGS) -c ttftps.c
	
#Cleaning old files before new makefile
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*