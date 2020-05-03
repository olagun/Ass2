source_files = $(wildcard src/*.c) $(wildcard src/commands/*.c) $(wildcard src/util/*.c)

# Look for #include inside the "includes/" folder
include_flags = -I includes/ -I /usr/local/opt/openssl/include/ 

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
clean: clean_client clean_server clean_pdf clean_test

# DOES NOT Remove the client folder if it exists, compiles the client, and 
# moves the client executable into client folder
client: 
	# Compile the client
	gcc $(include_flags) $(source_files) wtf_client.c -o WTF $(open_ssl_flags)
	# Move the client executable
	mv WTF client
	# Run 'configure' beforehand
	cd client; ./WTF configure 127.0.0.1 8000

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

build_client_no_configure: clean_client
	# Compile the client
	gcc $(include_flags) $(source_files) wtf_client.c -o WTF $(open_ssl_flags)
	# Make client folder
	mkdir client
	# Move the client executable
	mv WTF client


# Removes the server folder if it exists, compiles the server, and 
# moves the server executable into server folder
build_server: clean_server
	# Compile the server
	gcc $(include_flags) $(source_files) wtf_server.c -o WTFServer $(open_ssl_flags)
	# Make server folder
	mkdir server
	# Move the server executable
	mv WTFServer server

# Just RUN the server
run_server2: 
	cd server/projects; cp -R /Users/MaanyaTandon/Documents/Code/Systems/Ass2/client/test .
	cd server; ./WTFServer 8000

# Build then run the server
# copy_to_server:

# Build then run the server
run_server: build_server
	cd server; ./WTFServer 8000

# Build test
build_test: clean_test
	# Compile the server
	gcc $(include_flags) $(source_files) wtf_test.c -o WTFTest $(open_ssl_flags)
	# Make test folder
	mkdir test
	# Move the test executable
	mv WTFTest test
	# move server and client into test
	cp -R answers test

run_test: build_test
	cd test; ./WTFTest

# Removes test folder
clean_test:
	rm -rf test

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
	echo "example text"  > client/test/example.txt
	echo "example2 text" > client/test/example2.txt
	echo "example3 text" > client/test/example3.txt
	cd client; ./WTF add test example.txt; ./WTF add test example2.txt; ./WTF add test example3.txt

test_commit: test_add
	cd client; ./WTF commit test

test_push: test_commit
	cd client; ./WTF push test

test_update:
	cd client; ./WTF update test

test_history: test_push
	cd client; ./WTF history test

test_destroy: test_push
	cd client; ./WTF destroy test

test_rollback: test_push
	cd client; ./WTF rollback test 0
# test_commit_remove:
# test_commit_modify:

# Makes pdf for submission
pdf: 
	pandoc README.md -o README.pdf