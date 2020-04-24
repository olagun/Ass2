source_files = $(wildcard src/*.c) $(wildcard src/commands/*.c)

# Look for #include inside the "includes/" folder
include_flags = -I includes/

# Adds OpenSSL SHA256
# Use this for the iLab
# open_ssl_flags = -lssl -lcrypto

# Use this for Mac
open_ssl_flags = -L /usr/local/opt/openssl/lib  -lssl -lcrypto

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
	gcc $(include_flags) $(source_files) wtf_client.c -o WTF $(open_ssl_flags)
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
	gcc $(include_flags) $(source_files) wtf_server.c -o WTFServer $(open_ssl_flags)
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

# Tests to make dev faster
test_create: build_client
	cd client; ./WTF create test

test_add: test_create
	echo "example text" > client/test/example.txt
	cd client; ./WTF add test example.txt

# Makes pdf for submission
pdf: 
	pandoc README.md -o README.pdf