.PHONY:clean

pthread:pthread.c
	$(CC) -o $@ $< -lpthread


mutex:mutex.c
	$(CC) -o $@ $< -lpthread

clean:
	$(RM) pthread mutex
