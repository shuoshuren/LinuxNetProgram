all:client server

client: tcp_client.c
	gcc -o client  tcp_client.c

server: tcp_server.c
	gcc -o server  tcp_server.c

clean:
	$(RM) client server *.o
