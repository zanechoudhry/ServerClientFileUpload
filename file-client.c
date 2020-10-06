/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT "5432"
#define MAX_LINE 256

/*
 * Lookup a host IP address and connect to it using service. Arguments match the first two
 * arguments to getaddrinfo(3).
 *
 * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
 * the returned socket.
 */
int lookup_and_connect( const char *host, const char *service );
int sendall(int socket, char * buf, int *len);

int main( int argc, char *argv[] ) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s Host Port_Number Filename\n", argv[0]);
		exit(1);
	}
	char* port_no = argv[2];
	char *host = argv[1];
	char* filename = argv[3];
	char buf[MAX_LINE];
	int s;
	int len;
	int size = strlen(filename);

	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect(host, port_no) ) < 0 ) {
		exit( 1 );
	}
	if(sendall(s, filename, &size) == -1){
		perror( "stream-talk-client: send" );
		close(s);
		exit(1);
	}
	/* Main loop: get and send lines of text */
		while (1) {
			if (strcmp(buf, "stream-talk-server: Error Opening File") == 0){
				fprintf(stderr,"stream-talk-server: Error Opening File");
				close(s);
				exit(1);
			}
			FILE *f;
			f = fopen(filename, "w");
			while((len = recv( s, buf, sizeof(buf),0)) > 0){
				buf[MAX_LINE -1] = '\0';
			  if ( len < 0 ) {
					perror( "streak-talk-clent: recv" );
					close(s);
					exit(1);
				}
			fputs(buf,f);
			}
			break;
		}
	close(s);
	return 0;
}

int lookup_and_connect( const char *host, const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Translate host name into peer's IP address */
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ( ( s = getaddrinfo(host, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to connect */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-client: connect" );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}
int sendall(int socket, char * buf, int *len){
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(socket, buf + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}
