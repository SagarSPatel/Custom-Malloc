#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

typedef struct Node {
	int sfd;
	int mode;
	int flag;
	struct Node *next; 

}Node;
char *curate(char *string){
	int i;	
	for(i=0;i<strlen(string);i++){
		if(string[i]=='\n'){
		   string[i]='\0';
		}
	}
	return string;
}
Node *head=NULL;
void nodeinit(){
	head=malloc(sizeof(Node));
	head->next=NULL;
	head->sfd=-1001;
}

void * client_handler(void *sockfd){
	int sock=*((int*) sockfd);
	char buffer[256];
	int n;
	char Ecatch[256];
	while(1){
		n=read(sock,buffer,255);
		
		if(n==0){
			return ;
		}
	char *pos = index(buffer,'(');
	char command[2];
	memcpy(command,&buffer[0],1);
	command[1] = '\0';
	int len = pos-&buffer[0];
	pos = pos + 1;
	int count = 0;
	char *pos2;
	//printf("COMMAND = %s\n",command);
	char *bbuffer;
	if (strcmp(command,"o")==0){
		char *fpath;
		int fflag;		
		pos2 = index(pos,'(');
		if (count == 0){
			len = pos2-&pos[0];
			char path[len+1];
			memcpy(path,&pos[0],len);
			path[len] = '\0';
			pos2 = pos2 + 1;
			//printf("PATH = %s\n",path);
			count = count + 1;
			fpath=malloc(strlen(path));
			strcpy(fpath,path);
		}
		if (count == 1){
			//len = pos2-&pos[0];
			char flag[2];
			memcpy(flag,&pos2[0],1);
			flag[1] = '\0';
			//printf("FLAG = %s\n",flag);
			fflag=atoi(flag);
			if(fflag>2 || fflag<0){
				bzero(Ecatch,256);
				strcpy(Ecatch,"IFM");
				n=write(sock,Ecatch,strlen(Ecatch));
			}
		}

			Node *parse=head;
			while(parse->next!=NULL){
				parse=parse->next;
			}
			int f=open(fpath,fflag);
			if(f==-1){
				bzero(Ecatch,256);
				strcpy(Ecatch,"IFN");
				n=write(sock,Ecatch,strlen(Ecatch));
				free(sockfd);
				return -1;
			}
			parse->sfd=f;
			parse->flag=fflag;
			//printf("%d %d\n",parse->sfd,parse->mode);
			int b=-1*parse->sfd;
			parse->next=malloc(sizeof(Node));
			parse=parse->next;
			parse->next=NULL;
			char c[1];
			sprintf(c,"%d",b);
			bbuffer=c;	
			n=write(sock,bbuffer,strlen(bbuffer));
			//printfd(head);
			free(sockfd);
			return 1;
	
	
	}else if(strcmp(command,"w")==0){
		pos2 = index(pos,'(');
		int cfd;
		char *tbw;
		int nlen;
		if (count == 0){
			len = pos2-&pos[0];
			char filed[len+1];
			memcpy(filed,&pos[0],len);
			filed[len] = '\0';
			pos2 = pos2 + 1;
			//printf("FD = %s\n",filed);
			count = count + 1;
			cfd=atoi(filed);
		}
		if (count == 1){
			pos = pos2;
			pos2 = index(pos,'(');
			len = pos2-&pos[0];
			char bstring[len+1];
			memcpy(bstring,&pos[0],len);
			bstring[len] = '\0';
			pos2 = pos2 + 1;
			//printf("BYTES = %s\n",bstring);
			count = count + 1;
			tbw=malloc(strlen(bstring));
			strcpy(tbw,bstring);
		}
		if (count == 2){
			//printf("NUMBYTES = %s\n",pos2);
			nlen=atoi(pos2);
		}
		Node *parse=head;
		while(parse->next!=NULL){
			if(-1*(parse->sfd)==cfd){
				break;
			}
			parse=parse->next;
		}
		if(parse->next==NULL && -1*(parse->sfd)!=cfd){
			bzero(Ecatch,256);
			bzero(buffer,256);
			strcpy(Ecatch,"badfiled");
			n=write(sock,Ecatch,strlen(Ecatch));
		}
		if(parse->flag==0){
			bzero(Ecatch,256);
			bzero(buffer,256);
			strcpy(Ecatch,"rdonly");
			n=write(sock,Ecatch,strlen(Ecatch));
		}
		int written=write(parse->sfd,tbw,strlen(tbw));
		char a[256];
		sprintf(a,"%d",written);
		//bbuffer="netwrite used";
		n=write(sock,a,strlen(a));
		free(sockfd);
		return 1;
	}else if(strcmp(command,"r")==0){
		pos2 = index(pos,'(');
		int cfd;
		int nlen;
		if (count == 0){
			len = pos2-&pos[0];
			char filed[len+1];
			memcpy(filed,&pos[0],len);
			filed[len] = '\0';
			pos2 = pos2 + 1;
			//printf("FD = %s\n",filed);
			count = count + 1;
			cfd=atoi(filed);
			//printf("%d\n",cfd);
		}
		/*if (count == 1){
			pos = pos2;
			pos2 = index(pos,'(');
			len = pos2-&pos[0];
			char bstring[len+1];
			memcpy(bstring,&pos[0],len);
			bstring[len] = '\0';
			pos2 = pos2 + 1;
			printf("BYTES = %s\n",bstring);
			count = count + 1;
		}*/
		if (count == 1){
			//printf("NUMBYTES = %s\n",pos2);
			nlen=atoi(pos2);
			//printf("%d\n",nlen);
		}
		Node *parse=head;
		while(parse->next!=NULL){
			if(-1*(parse->sfd)==cfd){
				break;
			}
			parse=parse->next;
		}
		if(parse->next==NULL && -1*(parse->sfd)!=cfd){
			bzero(Ecatch,256);
			bzero(buffer,256);
			strcpy(Ecatch,"badfiled");
			n=write(sock,Ecatch,strlen(Ecatch));
			return -1;
		}
		if(parse->flag==1){
			bzero(Ecatch,256);
			bzero(buffer,256);
			strcpy(Ecatch,"wronly");
			n=write(sock,Ecatch,strlen(Ecatch));
		}
		char buf[nlen];
		int sizeread;
		sizeread=read(parse->sfd,buf,nlen);
		//printf("%d %d %d\n",nlen,parse->sfd,sizeread);
		char a[256];
		//printf("%s\n",buf);
		sprintf(a,"%d",sizeread);
		int mlen=strlen(a)+strlen(buf);
		char totalmessage[mlen];
		strcpy(totalmessage,a);
		strcat(totalmessage,"(");
		strcat(totalmessage,buf);
		//bbuffer="netread used";
		n=write(sock,totalmessage,strlen(totalmessage));
		free(sockfd);
		return 1;
	}else if(strcmp(command,"c")==0){
		//printf("FD = %s\n",pos);
		int cfd=atoi(pos);
		Node *parse=head;
		Node *prev=NULL;
		while(parse->next!=NULL){
			if(-1*(parse->sfd)==cfd){
				break;
			}
			prev=parse;
			parse=parse->next;
		}
		if(parse->next==NULL && -1*(parse->sfd)!=cfd){
			bzero(Ecatch,256);
			bzero(buffer,256);
			strcpy(Ecatch,"badfiled");
			n=write(sock,Ecatch,strlen(Ecatch));
			return -1;
		}
		int c=close(parse->sfd);
		if(parse==head){
			head=head->next;
			free(parse);
		}
		else{
			prev->next=parse->next;
			free(parse);
		}
		bbuffer="netclose used";
		char a[2];
		sprintf(a,"%d",c);
		n=write(sock,a,strlen(a));
		//printfd(head);
		free(sockfd);
		return 1;
	}else{ 
		printf("fail");
	}
		
		
	}//while close
	free(sockfd);
	return 0;
}
int main(int argc, char *argv[])
{
     int sockfd, newsockfd, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int i=0;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        perror("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(13159);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
              perror("Error printed by perror");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     nodeinit();
     while(1){
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	pthread_t sniffer_thread;
	int *new_sock;
	new_sock=malloc(1);
	*new_sock=newsockfd;
	if(pthread_create(&sniffer_thread,NULL,client_handler,(void*) new_sock) <0){
		printf("could not create thread\n");
		return 1;
	}
    }
	Node *parse=head;
	while(parse->next!=NULL){
		char *hello;
		hello="hello";
		write(parse->sfd,hello,strlen(hello));
		parse=parse->next;		
	} 
	return 0;
}

printfd(Node *head){
	Node *parse=head;
	if(parse==NULL){
		return;
	}
	while(parse->next!=NULL){
		printf("Node:%d->",parse->sfd);
		parse=parse->next;
	}
	printf("\n");
	return;
}

int isnegative(int a){
	if(a>0){
		return -1;
	}
}

