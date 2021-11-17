// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*
 * 	File server_sys_bad.c
 *	TCP SERVER that executes shell commands received as inputs from the network.
 */

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#include <ctype.h>


#define RBUFLEN		128

/* FUNCTION PROTOTYPES */
void service(int);

/* GLOBAL VARIABLES */
char buf[RBUFLEN];		 /* reception buffer */
char *prog_name;

int main(int argc, char *argv[])
{
    int		conn_request_skt;	/* passive socket */
    uint16_t 	lport_n, lport_h;	/* port used by server (net/host ord.) */
    int		bklog = 2;		/* listen backlog */
    int		childpid;		/* pid of child process */
    int		s;			/* connected socket */
    socklen_t	addrlen;
    struct sockaddr_in	saddr, caddr;	/* server and client addresses */ 

    prog_name = argv[0];

    if (argc != 2) {
	printf("Usage: %s <port number>\n", prog_name);
	exit(1);
    }

    /* input server port number */
    if (sscanf(argv[1], "%" SCNu16, &lport_h)!=1) {
	printf("Invalid port number");
	exit(1);
    }
    lport_n = htons(lport_h);

    /* create the socket */
    printf("creating socket...\n");
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1) {
        printf("socket creation failed...\n");
        exit(1);
    }

    /* bind the socket to any local IP address */
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family      = AF_INET;
    saddr.sin_port        = lport_n;
    saddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr *) &saddr, sizeof(saddr))!=0){
        printf("bind failed...\n");
        exit(1);
    }

    /* listen */
    printf ("Listening at socket %d with backlog = %d \n",s,bklog);
    if (listen(s, bklog) != 0){
        printf("listen failed...\n");
        exit(1);
    }

    conn_request_skt = s;

    /* main server loop */
    for (;;)
    {
	/* accept next connection */
	addrlen = sizeof(struct sockaddr_in);
	s = accept(conn_request_skt, (struct sockaddr *) &caddr, &addrlen);
	if (s<0) {
        	printf("accept failed...\n");
        	exit(1);
    	}
	printf("new socket: %u\n",s);
	service(s);
    }
}

/* Provides service on the passed socket */
void service(int s)
{
    int	 n;

    for (;;)
    {
	n=read(s, buf, RBUFLEN-1);
	if (n < 0)
	{
	   printf("Read error\n");
	   close(s);
	   printf("Socket %d closed\n", s);
	   break;
	}
	else if (n==0)
	{
	   printf("Connection closed by party on socket %d\n",s);
	   close(s);
	   break;
	}
       	else
	{
	    system(buf);
	    if(write(s, "Executed", sizeof("Executed")) != n)
		printf("Write error while replying\n");
	    else
		printf("Reply sent\n");
	}
    }
}


