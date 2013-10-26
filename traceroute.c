#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

void sendping(struct sockaddr*, int, uint16_t);
struct sockaddr_in reciveping(int sock);

int main(int argc, char* argv[]) {
  	int sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);

  	if(sock < 0) {
    		perror("socket");
    		exit(-1);
  	}
  
  	if(argc != 2) {
    		fprintf(stderr,"Use the correct argument, dumbass. (ping IP-ADDR)\n");
    		exit(1);
  	}

  	char* host = argv[1];
  	struct addrinfo *ai;
  	if (getaddrinfo(host, NULL, NULL, &ai) < 0) {
    		perror("getaddrinfo");
    		exit(1);
  	}

  	// get destination by pinging normally
	sendping(ai->ai_addr, sock, (uint16_t)0);
  	struct sockaddr_in msg = reciveping(sock);
  	char target[128];
 	getnameinfo((struct sockaddr *)&msg, sizeof(msg),
      	target, sizeof(target), NULL, 0, 0);
	printf("Normally pinging destination is(with out setsockt with TTL) %s\n", target);

  	int ttl = 1;
 	while(1) {
		printf("now the TTL = %d\n", ttl);
    		setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    		sendping(ai->ai_addr, sock, (uint16_t)0);
    		struct sockaddr_in msg = reciveping(sock);
    		char hostname[128];
    		getnameinfo((struct sockaddr *)&msg, sizeof(msg),
        	hostname, sizeof(hostname), NULL, 0, 0);
   	 	printf("from=%s\n", hostname);
		ttl++;
    		if(strcmp(hostname, target) == 0) break; //when find the destination, then stop the loop;
  	}

  return 0;
}

void sendping(struct sockaddr* ai_addr, int sock, uint16_t seq) {
	struct icmp message;
	message.icmp_type = ICMP_ECHO; // 8
  	message.icmp_code = ICMP_ECHOREPLY; // 0
  	message.icmp_cksum = 0;
  	message.icmp_hun.ih_idseq.icd_id = htons(8700); 
  	message.icmp_hun.ih_idseq.icd_seq = htons(seq);
  	memset(&message.icmp_dun, 0, sizeof(message.icmp_dun));

 	uint32_t sum = 0;
  	int i;
  	for (i = 0; i < sizeof(message)/2; i++) { //this assumes even message size
    		sum += *((uint16_t *)&message+i);
  	}

  	// Finish calculating and set message checksum
  	message.icmp_cksum = (uint16_t) ~(sum + (sum >> 16));

  	if ( sendto(sock, &message, sizeof(message),
        	0, ai_addr, sizeof(*ai_addr)) < 0) {
   		 perror("sendto");
  	} 
}

struct sockaddr_in reciveping(int sock) {
	struct icmp message;
  	struct sockaddr_storage sendr; 
  	int fromlen = sizeof(sendr);
  	if (recvfrom(sock, &message, sizeof(message),
        	0, (struct sockaddr*) &sendr, &fromlen ) < 0) {
    		perror("recvfrom");
    		exit(-1);
  	} else {
    		return *((struct sockaddr_in *)&sendr);
  	}
}
