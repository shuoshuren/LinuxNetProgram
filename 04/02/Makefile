.PHONY:clean

SRCS = getpid.c fork.c

TARGET= $(patsubst %.c, %, $(SRCS))

getpid:getpid.c
	$(CC) -o $@ $<

fork:fork.c
	$(CC) -o $@ $<



clean:
	-$(RM) $(TARGET)
