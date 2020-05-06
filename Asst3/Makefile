source_files := $(wildcard src/*.c) $(wildcard src/commands/*.c) $(wildcard src/util/*.c)
include_flags := -I includes/ -I /usr/local/opt/openssl/include/ 
open_ssl_flags := -L /usr/local/opt/openssl/lib -lssl -lcrypto
other_flags := -pthread -lz

client: build_client
server: build_server
test: build_test
pdf: build_pdf

all: build_client build_client2 build_server build_test
clean: clean_client clean_client2 clean_server clean_test

# Run --- 
run_server: build_server
	cd server; ./WTFServer 8000

run_test: build_test
	cd test; ./WTFTest
	
# Build ---
build_client: clean_client
	gcc $(include_flags) $(other_flags) $(source_files) wtf_client.c -o WTF $(open_ssl_flags)
	mkdir client
	mv WTF client
	cd client; ./WTF configure 127.0.0.1 8000

build_client2: clean_client2
	gcc $(include_flags) $(other_flags) $(source_files) wtf_client.c -o WTF $(open_ssl_flags)
	mkdir client2
	mv WTF client2
	cd client2; ./WTF configure 127.0.0.1 8000

build_server: clean_server
	gcc $(include_flags) $(other_flags) $(source_files) wtf_server.c -o WTFServer $(open_ssl_flags)
	mkdir server
	mv WTFServer server

build_test: clean_test
	gcc $(include_flags) $(other_flags) $(source_files) wtf_test.c -o WTFTest $(open_ssl_flags)
	rm -rf test
	mkdir test
	mv WTFTest test

build_pdf: 
	pandoc README.md -o README.pdf --wrap=preserve

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
test_destroy_nonexistent: build_client
	cd client; ./WTF destroy madeup

test_destroy_valid: test_push
	cd client; ./WTF destroy test

# history
test_history_nonexistent: test_case41
	cd client; ./WTF history madeup

test_history_valid: test_case41
	cd client; ./WTF history test1
# ****FAILED*****

# --------------------------------------------
# Test Case 4: Create Project 'test1', Add files, Commit, Push.
# --------------------------------------------
test_case4: 
	rm -rf client/test1
	cd client; ./WTF create test1
	echo "Test1, example 1 file" > client/test1/example1.txt
	echo "Test1, example 2 file" > client/test1/example2.txt
	echo "Test1, example 3 file" > client/test1/example3.txt
	cd client; ./WTF add test1 example1.txt
	cd client; ./WTF add test1 example2.txt
	cd client; ./WTF add test1 example3.txt
	cd client; ./WTF commit test1
	cd client; ./WTF push   test1

# --------------------------------------------
# Test Case 4.1: Create Project 'test1', Add files, Commit, Push.
#              Then modify example1.txt, and remove example3.txt,
#              Then Commit, Push.
#              REMOVE THE PROJECT LOCALLY, then
#              Try CHECKOUT - which should suceed.
# --------------------------------------------
test_case41: test_case4
	# Modify exmple1.txt
	echo "Test1, example 1 file, modified" >> client/test1/example1.txt
	# Remove exmple3.txt
	cd client; ./WTF remove test1 example3.txt
	cd client; ./WTF commit test1
	cd client; ./WTF push   test1
	# test scceeded.
	# Remove the project locally.
	cd client; rm -r test1
	cd client; ./WTF checkout test1
	# test succeeded: Successfully checked out example2.txt, & the modified file exmaple1.txt. Correct files in .Manifest.

# --------------------------------------------
# Test Case 4.2: Create second Project 'test2', Add files, Commit, Push.
#              Then modify example1.txt, and remove example3.txt,
#              Then Commit, Push.
# --------------------------------------------
test_case42: test_case4
	cd client; ./WTF create test2
	echo "Test2, example 1 file" > client/test2/example1.txt
	echo "Test2, example 2 file" > client/test2/example2.txt
	echo "Test2, example 3 file" > client/test2/example3.txt
	cd client; ./WTF add test2 example1.txt
	cd client; ./WTF add test2 example2.txt
	cd client; ./WTF add test2 example3.txt
	cd client; ./WTF commit test2
	cd client; ./WTF push   test2
	# Now remove test2/example1.txt 
	cd client; ./WTF remove test2 example1.txt
	cd client; ./WTF commit test2
	cd client; ./WTF push   test2
	# test succeeded.

# --------------------------------------------
# Test Case 5: Create Two Projects Simultaneously: test1 & test2
# --------------------------------------------
test_case5: test_case4 test_case4a
	# test succeeded: Successfully checked out example2.txt & exmaple3.txt in both projects test1 & test2

# --------------------------------------------
# Test Case 6: Test update/upgrade twice: Should succeed.
# --------------------------------------------
test_case6: test_case4
	cd client; ./WTF update  test1
	cd client; ./WTF upgrade test1
	cd client; ./WTF update  test1
	cd client; ./WTF upgrade test1
	# Test succeeded.

# --------------------------------------------
# Test Case 7: Test Update with Conflicts: Should not fail.
# --------------------------------------------
test_case7: test_case4
	# Introduce conflicts by changing client files.
	cd client; echo "Test1, example 1 file, modified again" >> test1/example1.txt
	# update should not fail
	cd client; ./WTF update  test1
	cd client; ./WTF upgrade test1

# --------------------------------------------
# Test Case 8: Test Update/Upgrade with Conflicts: New files on client side: Should not fail.
# --------------------------------------------
test_case8: test_case4
	# Introduce conflicts, by adding new files on client
	echo "Test1, example 4 file" > client/test1/example4.txt
	echo "Test1, example 5 file" > client/test1/example5.txt
	cd client; ./WTF add test1 example4.txt
	cd client; ./WTF add test1 example5.txt
	# update should not fail
	cd client; ./WTF update  test1
	cd client; ./WTF upgrade test1

# --------------------------------------------
# Test Case 9: Add same file twice. Should give error message.
# --------------------------------------------
test_case9: test_case4
	cd client; ./WTF add test1 example1.txt

# --------------------------------------------
# Test Case 10: Test 'update/upgrade' with 2nd client
# --------------------------------------------
test_case10: test_case4 
	# Create a 2'nd client 'Sam' & then checkout files for 'Sam'.
	rm -rf client/Sam
	cd client; mkdir Sam
	cd client/Sam; ../WTF configure 127.0.0.1 8000
	cd client/Sam; ../WTF checkout test1
	# Client 1 Adds a new file, commits it & pushes it.
	echo "Test1, example 5 file" > client/test1/example5.txt
	cd client; ./WTF add test1 example5.txt
	cd client; ./WTF commit test1
	cd client; ./WTF push   test1
	# Do Update/Upgrade for client 2.
	cd client/Sam; ../WTF update  test1
	cd client/Sam; ../WTF upgrade test1
	# Client 2 successfully gets the new file: example5.txt

#destroy
