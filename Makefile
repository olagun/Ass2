source_files := $(wildcard src/*.c) $(wildcard src/commands/*.c) $(wildcard src/util/*.c)
include_flags := -I includes/ -I /usr/local/opt/openssl/include/ 
open_ssl_flags := -L /usr/local/opt/openssl/lib -lssl -lcrypto

all: build build_pdf
build: build_client build_server
clean: clean_client clean_client2 clean_server clean_pdf clean_test 

# Build ---
build_client: clean_client
	gcc $(include_flags) $(source_files) wtf_client.c -o WTF $(open_ssl_flags)
	mkdir client
	mv WTF client
	cd client; ./WTF configure 127.0.0.1 8000

build_client2: clean_client2
	gcc $(include_flags) $(source_files) wtf_client.c -o WTF $(open_ssl_flags)
	mkdir client2
	mv WTF client2
	cd client2; ./WTF configure 127.0.0.1 8000

build_server: clean_server
	gcc $(include_flags) $(source_files) wtf_server.c -o WTFServer $(open_ssl_flags)
	mkdir server
	mv WTFServer server

build_test: clean_test
	gcc $(include_flags) $(source_files) wtf_test.c -o WTFTest $(open_ssl_flags)
	mkdir test
	mv WTFTest test
	cp -R answers test

build_pdf: 
	pandoc README.md -o README.pdf

# Run --- 
run_server: build_server
	cd server; ./WTFServer 8000

run_test: build_test
	cd test; ./WTFTest

# Clean ---
clean_test:
	rm -rf test

clean_client:
	rm -rf client

clean_client2:
	rm -rf client2

clean_server:
	rm -rf server

clean_pdf:
	rm -f README.pdf

# Tests --- (for dev only)

# configure
test_configure:
	cd client; ./WTF configure 127.0.0.1 8000

# checkout
test_checkout_nonexist: test_configure
	cd client; ./WTF checkout madeup

test_checkout_noconfigure:
	cd client; ./WTF checkout madeup

test_checkout_alreadyexists: test_create
	cd client; ./WTF checkout test

# update
test_update: test_udpate_add_case

test_update_full_success: test_push build_client2
	cd client2; ./WTF checkout test
	cd client2; ./WTF update test

test_update_add_case: test_commit build_client2
	cd client2; ./WTF checkout test
	cd client; ./WTF push test
	cd client2; ./WTF update test

test_update_modify_case: test_push build_client2
	cd client2; ./WTF checkout test
	cd client; echo "more things" > test/example.txt
	cd client; ./WTF commit test
	cd client; ./WTF push test
	cd client2; ./WTF update test

test_update_remove_case: test_push build_client2
	cd client2; ./WTF checkout test
	cd client; ./WTF remove test example.txt
	cd client; ./WTF commit test
	cd client; ./WTF push test
	cd client2; ./WTF update test

# upgrade
test_upgrade_add_case: test_update_add_case
	cd client2; ./WTF upgrade test

test_upgrade_modify_case: test_update_modify_case
	cd client2; ./WTF upgrade test

test_upgrade_remove_case: test_update_remove_case
	cd client2; ./WTF upgrade test

# create
test_create: build_client
	cd client; ./WTF create test

test_create_alreadyexists: test_create
	cd client; ./WTF create test

# add
test_add: test_create
	echo "example text" > client/test/example.txt
	echo "example2 text" > client/test/example2.txt
	echo "example3 text" > client/test/example3.txt
	cd client; ./WTF add test example.txt;
	cd client; ./WTF add test example2.txt;
	cd client; ./WTF add test example3.txt

# commit
test_commit: test_commit_add_case

test_commit_add_case: test_add
	cd client; ./WTF commit test

test_commit_modify_case: test_push
	echo "new text" > client/test/example.txt
	cd client; ./WTF commit test

test_commit_remove_case: test_push
	cd client; ./WTF remove test example.txt
	cd client; ./WTF commit test

# push
test_push: test_commit
	cd client; ./WTF push test

# currentversion
test_currentversion_nonexistent: build_client
	cd client; ./WTF currentversion madeup

test_currentversion_valid: test_push
	cd client; ./WTF currentversion test

# destroy
test_destroy