client: client.c
	gcc -g client.c -o client -lrt

server: server.c
	gcc -g server.c -o server -lrt

clean:
	rm server client
