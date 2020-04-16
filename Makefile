source_files = $(wildcard src/*/*.c)
command_files = $(wildcard commands/*/*.c)

# Alias for 'build'
all: build

# Clean, compile and move both the server and the client
build: build_client build_server

# Removes both client and server folder
clean: clean_client clean_server

# Removes the client folder, compiles the client, and 
# moves the client executable into client folder
build_client: clean_client
	# Compile the client
	gcc $(source_files) $(command_files) WTFClient.c -o WTF
	mkdir client
	# Move the client executable
	mv WTF client
	# Run 'configure' beforehand
	cd client; ./WTF configure 127.0.0.1 8000

# Removes the server folder, compiles the server, and 
# moves the server executable into client folder
build_server: clean_server
	# Compile the server
	gcc $(source_files) $(command_files) WTFServer.c -o WTFServer
	mkdir server
	# Move the server executable
	mv WTFServer server

# Removes client folder
clean_client:
	rm -rf client

# Removes server folder
clean_server:
	rm -rf server