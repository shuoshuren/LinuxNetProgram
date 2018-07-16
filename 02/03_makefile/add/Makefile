OBJS = add_int.o add_float.o

all:$(OBJS)
.PHONY = clean

$(OBJS):%.o:%.c
	$(CC) -c $< -o $(OBJSDIR)/$@ $(CFLAGS)

clean:
	$(RM) $(OBJS)

