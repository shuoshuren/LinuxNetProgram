
PROCESS_SRC = process_readv_writev.c


client:client.c $(PROCESS_SRC)
	gcc -o client client.c $(PROCESS_SRC)

server:server.c $(PROCESS_SRC)
	gcc -o server server.c $(PROCESS_SRC)



clean:
	$(RM) client server *.o
