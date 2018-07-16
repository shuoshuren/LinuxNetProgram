.PHONY:clean

CFLAGS = -O2
export OBJSDIR = ${shell pwd}/objs
TARGET = cacu
OBJS = add_int.o add_float.o sub_int.o sub_float.o main.o

$(TARGET):$(OBJSDIR) main.o
	$(MAKE) -C add
	$(MAKE) -C sub
	$(CC) -o $(TARGET) $(OBJSDIR)/*.o  $(CFLAGS)

main.o:%.o:%.c
	$(CC) -c $< -o $(OBJSDIR)/$@

$(OBJSDIR):
	mkdir -p $(OBJSDIR)

clean:
	-$(RM) $(TARGET)
	-$(RM) $(OBJSDIR)/*.o 
