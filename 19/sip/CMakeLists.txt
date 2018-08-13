cmake_minimum_required(VERSION 3.12)
project(sip C)

set(CMAKE_C_STANDARD 11)

add_executable(sip main.c skbuff/sip_skbuff.c skbuff/sip_skbuff.h ether/sip_ether.c ether/sip_ether.h arp/sip_arp.c arp/sip_arp.h ip/sip_ip.c ip/sip_ip.h icmp/sip_icmp.c icmp/sip_icmp.h udp/sip_udp.c udp/sip_udp.h socket/sip_socket.c socket/sip_socket.h sip.h bsd/sip_socket.c bsd/sip_socket.h)