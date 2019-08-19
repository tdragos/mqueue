client: client.c
	gcc client.c -o client -lrt

server: server.c
	gcc server.c -o server -lrt

clean:
	rm server client
