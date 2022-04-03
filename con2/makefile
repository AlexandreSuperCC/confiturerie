
TARGET=con2

SRCS = con2.c

OBJS = $(SRCS:.c=.o)
 
CC=gcc
CFLAGS=-Wall -std=gnu99
 
$(TARGET):$(OBJS)
	$(CC) -o $@ $^
 
clean:
	rm -rf $(TARGET) $(OBJS)


