source_files = $(wildcard src/*/*.c)
command_files = $(wildcard commands/*/*.c)
openssl_flags = -lssl -lcrypto

# Alias for 'build'
all: build

# Clean, compile and move both the server and the client
build: build_client build_server

# Removes both client and server folder
clean: clean_client clean_server clean_pdf

# Removes the client folder if it exists, compiles the client, and 
# moves the client executable into client folder
build_client: clean_client
	# Compile the client
	gcc $(open_ssl_flags) $(source_files) $(command_files) WTFClient.c -o WTF 
	# Make client folder
	mkdir client
	# Move the client executable
	mv WTF client
	# Run 'configure' beforehand
	cd client; ./WTF configure 127.0.0.1 8000

# Removes the server folder if it exists, compiles the server, and 
# moves the server executable into server folder
build_server: clean_server
	# Compile the server
	gcc $(open_ssl_flags) $(source_files) $(command_files) WTFServer.c -o WTFServer
	# Make server folder
	mkdir server
	# Move the server executable
	mv WTFServer server

# Build then run the server
run_server: build_server
	cd server; ./WTFServer 8000

# Removes client folder
clean_client:
	rm -rf client

# Removes server folder
clean_server:
	rm -rf server

# Removes pdf
clean_pdf:
	rm -f README.pdf

# Makes pdf for submission
pdf: 
	pandoc README.md -o README.pdf