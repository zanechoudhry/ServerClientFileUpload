//Started with code from Blackboard
/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT "5432"
#define MAX_LINE 256
#define MAX_PENDING 5

/*
 * Create, bind and passive open a socket on a local interface for the provided service.
 * Argument matches the second argument to getaddrinfo(3).
 *
 * Returns a passively opened socket or -1 on error. Caller is responsible for calling
 * accept and closing the socket.
 */
int bind_and_listen( const char *service);
int sendall(int socket, char * buf, int *size);

int main( int argc, char *argv[] ) {
	if (argc != 2) {
		fprintf(stderr, "Server Error: Usage: %s Port_Number\n", argv[0]);
		exit(1);
	}
	char filename[2000];
	char* port_no = argv[1];
	char buf[MAX_LINE];
	int s, new_s;
	int len;
	int read;
	char *err = NULL;
	/* Bind socket to local interface and passive open */
	if ((s = bind_and_listen(port_no)) < 0 ) {
		exit(1);
	}
	/* Wait for connection, then receive and print text */
	while (1) {
		if ( ( new_s = accept( s, NULL, NULL ) ) < 0 ) {
			perror( "Server Error: accept" );
			close(new_s);
			exit(1);
		}
		buf[MAX_LINE-1] = '\0';
		if((read = recv(new_s , filename , sizeof(filename) , 0)) < 0 ){
			perror( "Server Error: recv" );
			close( new_s );
			exit(1);
		}
		FILE *f;
		f = fopen(filename, "r");
		if(f == NULL){
			err = "Server Error: Unable to open File";
			int size = strlen(err);
			if(sendall(new_s, err, &size) == -1){
				perror( "Server Error: send" );
				close( new_s );
				exit( 1 );
			}
		}
		else{
			while (fread(buf, sizeof(char), sizeof(buf) - 1, f) > 0) {
				buf[MAX_LINE-1] = '\0';
				len = strlen( buf ) + 1;
				if (sendall(new_s, buf, &len) == -1 ) {
					perror( "Server Error: send" );
					close( new_s );
					exit( 1 );
				}
				memset(buf,0,sizeof(buf));
			}
		}
		fclose(f);
		close( new_s );
		return 0;
	}
}

int bind_and_listen( const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Build address data structure */
	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;

	/* Get local address info */
	if ( ( s = getaddrinfo( NULL, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "Server Error: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to perform passive open */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( !bind( s, rp->ai_addr, rp->ai_addrlen ) ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "Server Error: bind" );
		return -1;
	}
	if ( listen( s, MAX_PENDING ) == -1 ) {
		perror( "Server Error: listen" );
		close( s );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}
int sendall(int socket, char * buf, int *size){
    int totalBytes = 0;
    int bytesRemaining = *size;
    int n;

    while(totalBytes < *size) {
        n = send(socket, buf + totalBytes, bytesRemaining, 0);
        if (n == -1) { break; }
        totalBytes += n;
        bytesRemaining -= n;
    }
    *size = totalBytes;
		if(n == -1){
			return -1;
		}
		else{
			return 0;
		}
}
