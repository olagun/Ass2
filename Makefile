src_files = $(wildcard src/*.c)

all: WTFServer WTFClient

WTFClient:
	gcc -g $(src_files) WTFClient.c -o WTFClient

WTFServer:
	gcc -g $(src_files) WTFServer.c -o WTFServer

clean:
	rm -rf WTFClient WTFServer