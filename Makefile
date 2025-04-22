CC      = gcc
CFLAGS  = -Wall -g -pthread
TARGET  = sh257
SRCS    = shellex.c csapp.c
OBJS    = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
