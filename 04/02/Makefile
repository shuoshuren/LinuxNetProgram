.PHONY:clean

SRCS = getpid.c

TARGET= $(patsubst %.c, %, $(SRCS))


$(TARGET):$(SRCS)
	$(CC) -o $@ $<


clean:
	-$(RM) $(TARGET)
