.PHONY= clean

udp_loop:
	gcc -o udp_server udp_loop_server.c
	gcc -o udp_client udp_loop_client.c

tcp_loop:
	gcc -o tcp_server tcp_loop_server.c
	gcc -o tcp_client tcp_loop_client.c

clean:
	$(RM) udp_server udp_client tcp_server tcp_client