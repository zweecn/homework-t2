objects = w_dns_conf.o w_msg.o w_udpclient.o w_udpserver.o w_dnsserver.o w_dnsclient.o
all_t = w_client w_server

all : w_client w_server
.PHONY : all

w_client : $(objects) w_client.o
	g++ -o w_client $(objects) w_client.o 

w_server: $(objects) w_server.o
	g++ -o w_server $(objects) w_server.o 

w_client.o : w_udpclient.h
w_server.o : w_udpserver.h

w_dns_conf.o : w_dns_conf.h
w_msg.o : w_msg.h
w_udpserver.o : w_udpserver.h
w_udpclient.o : w_udpclient.h
w_dnsserver.o : w_dnsserver.h
w_dnsclient.o : w_dnsclient.h


.PHONY : clean 
clean : 
	rm $(all_t) *.o

