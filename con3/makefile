
TARGET=con3

SRCS = con3.c

OBJS = $(SRCS:.c=.o)
 
CC=gcc
CFLAGS=-Wall -std=gnu99
 
$(TARGET):$(OBJS)
	$(CC) -o $@ $^
 
clean:
	rm -rf $(TARGET) $(OBJS)


