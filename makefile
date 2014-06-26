test:test.c console.c group2.c init.c nmac_init.c ourparse.c pcap.c send_packet.c write_data.c mqueue.c
	gcc -o test test.c console.c group2.c init.c nmac_init.c ourparse.c pcap.c send_packet.c write_data.c mqueue.c write_data_thread.c -lpcap -lnet -lpthread
