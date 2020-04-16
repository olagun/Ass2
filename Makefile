src_files = $(wildcard src/*.c)

all: WTFServer WTFClient

WTFClient:
	gcc $(src_files) WTFClient.c -o WTF

WTFServer:
	gcc $(src_files) WTFServer.c -o WTFServer

clean:
	rm -rf WTF WTFServer .configure