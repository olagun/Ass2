src_files = $(wildcard src/*.c)
# c_files = $(wildcard *.c)

all: wtf_server wtf_client

wtf_client: clean_client
	gcc $(src_files) create.c WTFClient.c -o WTF
	mkdir client
	mv WTF client
	cd client; ./WTF configure 127.0.0.1 8000

wtf_server: clean_server
	gcc $(src_files) create.c WTFServer.c -o WTFServer
	mkdir server
	mv WTFServer server

clean_client:
	rm -rf client

clean_server:
	rm -rf server

clean: clean_client clean_server