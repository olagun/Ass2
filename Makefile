src_files = $(wildcard src/*.c)

all: WTFServer WTFClient

WTFClient:
	gcc $(src_files) WTFClient.c -o WTFClient

WTFServer:
	gcc $(src_files) WTFServer.c -o WTFServer

clean:
	rm -rf WTFClient WTFServer