#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
char *hostName;
int netopen(char *path, int flags){
	if(hostName==NULL){
		printf("ERROR server connection not initialized\n");
		return -1;
        }
	int sockfd, n;  
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char *command;
	char *bbuffer[256];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/*if (sockfd < 0) {
        	error("ERROR opening socket");
	}*/
	server = gethostbyname(hostName);
	/*if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
    	}*/
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(13159);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
		errno=ETIMEDOUT;
		perror("ERROR");
		return -1 ;
    	}
	bzero(bbuffer,256);
	char a[1];
	sprintf(a,"%d",flags);
	int len =strlen(path)+2+1+1+1;
	char totalmessage[len];
	strcpy(totalmessage,"o");
	strcat(totalmessage,"(");
	strcat(totalmessage,path);
	strcat(totalmessage,"(");
	strcat(totalmessage,a);
	//printf("%s\n",totalmessage);
	n=write(sockfd,totalmessage,strlen(totalmessage));
	n=read(sockfd,bbuffer,255);
	//printf("%s\n",bbuffer);
	if(strcmp(bbuffer,"IFN")==0){
		printf("ERROR on open\n");
		return -1;
	}
	if(strcmp(bbuffer,"IFM")==0){
		printf("ERROR file mode not supported\n");
		return -1;
	}
	int b=atoi(bbuffer);
    	close(sockfd);
	return b;
}

int netclose(int fd){
    if(hostName==NULL){
		printf("ERROR server connection not initialized\n");
		return -1;
    }
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *buffer;
    char *bbuffer[256];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(hostName);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
         serv_addr.sin_port = htons(13159);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
		errno=ETIMEDOUT;		
		perror("ERROR");
		return -1;
	}
    if(fd>0){
    	errno=EBADF;
	perror("ERROR");
	return -1;
    }
	bzero(bbuffer,256);
	buffer="p";
	char a[256];
	sprintf(a,"%d",fd);
	char totalmessage[256];
	strcpy(totalmessage,"c");
	strcat(totalmessage,"(");
	strcat(totalmessage,a);
	//printf("%s\n",totalmessage);
	n=write(sockfd,totalmessage,strlen(totalmessage));
	n=read(sockfd,bbuffer,255);
	if(strcmp("badfiled",bbuffer)==0){
		errno=EBADF;
		perror("ERROR");
		return -1;
	}
    	close(sockfd);
	//return atoi(bbuffer);
}

ssize_t netread(int fildes, void *buf,size_t nbyte){
    if(hostName==NULL){
		printf("ERROR server connection not initialized\n");
		return -1;
    }
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *buffer;
    char bbuffer[256];
    char **bufp=buf;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(hostName);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(13159);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
    		errno=ETIMEDOUT;
		perror("ERROR");
		return -1;
    }
     if(fildes>0){
		errno=EBADF;
		perror("ERROR");
		return -1;
    }
	bzero(bbuffer,256);
	char a[256];
	char size[256];
	sprintf(a,"%d",fildes);
	sprintf(size,"%d",nbyte);
	int len= strlen(a)+strlen(size)+2+2;
	char totalmessage[len];
	strcpy(totalmessage,"r");
	strcat(totalmessage,"(");
	strcat(totalmessage,a);
	strcat(totalmessage,"(");
	strcat(totalmessage,size);
	//printf("%s\n",totalmessage);
	n=write(sockfd,totalmessage,strlen(totalmessage));
	n=read(sockfd,bbuffer,255);
	if(strcmp("badfiled",bbuffer)==0){
		errno=EBADF;
		perror("ERROR");
		return -1;
	}
	if(strcmp("wronly-1(",bbuffer)==0){
		printf("ERROR WRITE ONLY FILE\n");
	}
	//printf("%s\n",bbuffer);
	char *pos = index(bbuffer,'(');
	int len3 = pos-&bbuffer[0];
	char readsuc[len3+1];
	memcpy(readsuc,&bbuffer[0],len);
	readsuc[len3] = '\0';
	pos = pos + 1;
	char *stringread = pos;
	//buf=malloc(strlen(stringread));
	strcpy(buf,stringread);
	//memcpy(buf,&stringread[0],strlen(stringread)-1);	
	//printf("%s %s\n",buf,readsuc);
    	close(sockfd);
	return atoi(readsuc);
}

ssize_t netwrite(int fildes, void *buf,size_t nbyte){
    if(hostName==NULL){
		printf("ERROR server connection not initialized\n");
		return -1;
    }
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *buffer;
    char *bbuffer[256];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(hostName);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(13159);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
		errno=ETIMEDOUT;
    		perror("ERROR");
		return -1;
    }
    if(fildes>0){
		errno=EBADF;
		perror("ERROR");
		return -1;
     }
	bzero(bbuffer,256);
	char *a[256];
	char *size[256];
	sprintf(a,"%d",fildes);
	sprintf(size,"%d",nbyte);
	char *tbw=(char *)buf;
	int len=strlen(a)+strlen(size)+3+1+1+strlen(tbw);
	char totalmessage[len];
	strcpy(totalmessage,"w");
	strcat(totalmessage,"(");
	strcat(totalmessage,a);
	strcat(totalmessage,"(");
	strcat(totalmessage,tbw);
	strcat(totalmessage,"(");
	strcat(totalmessage,size);
	//printf("%s\n",totalmessage);
	n=write(sockfd,totalmessage,strlen(totalmessage));
	bzero(bbuffer,256);
	bzero(totalmessage,strlen(totalmessage));
	n=read(sockfd,bbuffer,255);
	char b[strlen(tbw)];
	sprintf(b,"%d",strlen(tbw));
	int elen=strlen(b)+strlen("badfiled");
	char ecatcher[elen];
	strcpy(ecatcher,"badfiled");
	strcat(ecatcher,b);
	//printf("%s    %s",ecatcher,bbuffer);
	if(strcmp(ecatcher,bbuffer)==0){
		errno=EBADF;
		perror("ERROR");
		return -1;
	}
	if(strcmp("rdonly-1",bbuffer)==0){
		printf("ERROR:FILE READ ONLY\n");
		return -1;
	}
	//perror(bbuffer);
	//printf("%s\n",bbuffer);
    	close(sockfd);
	return atoi(bbuffer);
}
int netserverinit(char *a){
	hostName=a;
	if(gethostbyname(hostName)!=NULL){
		return 0;
	}
	else{
		printf("HOST_NOT_FOUND\n");
		return -1;
	}
}
netprint(){
	printf("%s\n",hostName);
}
