all:netfileserver.c libnetfiles.o netclient
	gcc -lpthread netfileserver.c -o netfileserver

libnetfiles.o: libnetfiles.c libnetfiles.h
	gcc -c libnetfiles.c

netclient: netclient.c
	gcc libnetfiles.o netclient.c -o netclient

clean:
	rm libnetfiles.o ; rm netfileserver ; rm netclient
