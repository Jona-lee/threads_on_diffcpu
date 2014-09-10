
CC=arm-none-linux-gnueabi-gcc
STRIP=arm-none-linux-gnueabi-strip

CFLAGS = -lpthread
LIBS =

CC_SRC = \
	pthread_on_cpu.c

OBJS = pthread_on_cpu.o
.SUFFIXES: .c

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: pthread_on_cpu

clean:
	rm -f *.o
	rm -f pthread_on_cpu

pthread_on_cpu: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o pthread_on_cpu
	$(STRIP) pthread_on_cpu

