#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define HOST "localhost"
#include <sys/types.h>
#include <netdb.h>

struct pkt {
		int ACK;
		int seqnum;
		char data[25];
};



int main(){
	int senderSocket,i,n;
	char input[25];
	struct timeval timeout={2,0}; //set timeout for 2 seconds
	int recvlen;
	struct sockaddr_in receiverAddr,senderAddr;	//clientAddr is new
	socklen_t addr_size;
	struct hostent *host;
	int counter=0;
	

	host = gethostbyname("127.0.0.1");
	
	if (host == NULL) {
        printf("ERROR, no such host as %s\n", HOST);
        exit(0);
    }
    
	senderSocket = socket(AF_INET, SOCK_DGRAM, 0); //using diagram instead of stream-- UDP
	
	receiverAddr.sin_family = AF_INET;
	receiverAddr.sin_port = htons(atoi("4400"));
	receiverAddr.sin_addr = *((struct in_addr *)host->h_addr);
	
	memset(receiverAddr.sin_zero, '\0', sizeof receiverAddr.sin_zero);

	addr_size = sizeof receiverAddr;
	
	
	printf("\nEnter a string to send, q to quit\n ");

	while(1)
	{	int err;
		struct pkt packet,recvPacket;
		
		scanf("%s",input);
		
		if(strcmp("q",input)==0){
			printf("Quiting\n");
			return 0;
		}
		
		strcpy(packet.data,input);
		packet.seqnum=counter;
		
		//int socket fd,     payload,  length,      flags,      "to"-destination IP and POrt, sizeof
		sendto(senderSocket, &packet, sizeof(packet), 0, (struct sockaddr*)&receiverAddr,  addr_size);
		
		printf("Sent pkt %s with seq number %d\n",input,packet.seqnum);
				
		/* set receive UDP message timeout */

		//setsockopt(senderSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));

		/* Receive UDP message */
		//recvlen =  recvfrom(senderSocket, &recvPacket, sizeof(recvPacket), 0,(struct sockaddr*)&receiverAddr, &addr_size);
		while(1){
		
		           setsockopt(senderSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
		           recvlen = recvfrom(senderSocket, &recvPacket, sizeof(recvPacket), 0,(struct sockaddr*)&receiverAddr, &addr_size);
			if (recvlen >= 0) {
				//Message Received
				if(recvPacket.ACK == counter+1){
					printf("Ack 1 received for seq no : %d\n\n",counter);
					counter++;
					break;
				} else {	//won't be sent out of order, this is unneccesary
					printf("Resending pkt %d:%s\n",packet.seqnum,packet.data);
					sendto(senderSocket, &packet, sizeof(packet), 0, (struct sockaddr*)&receiverAddr,  addr_size);
					//recvlen = recvfrom(senderSocket, &recvPacket, sizeof(recvPacket), 0,(struct sockaddr*)&receiverAddr, &addr_size);
				}
			}
			else{
				//Message Receive Timeout			
				printf("\n\nTimeout! Ack 0 recieved for seq no: %d\n", counter);
				printf("Resending pkt %d:%s\n",packet.seqnum,packet.data);
				//printf("Ack 0 recieved for seq no: %d ",counter);
				sendto(senderSocket, &packet, sizeof(packet), 0, (struct sockaddr*)&receiverAddr,  addr_size);
				recvlen = recvfrom(senderSocket, &recvPacket, sizeof(recvPacket), 0,(struct sockaddr*)&receiverAddr, &addr_size);
				counter++;
				break;

			}
		}
	}
	return 0;
	
}
