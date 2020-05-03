#!/bin/zsh 
# --------------------------------------------
# Test Case 0: Configure
# --------------------------------------------
cd client; ./WTF configure 127.0.0.1 8000
# test scceeded.

# --------------------------------------------
# Test Case 1: Create one Project, Add files, Commit, Push
# --------------------------------------------
# make run_server
cd client
./WTF create test1
echo "Test1, example 1 file" > test1/example1.txt
echo "Test1, example 2 file" > test1/example2.txt
echo "Test1, example 3 file" > test1/example3.txt
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt

./WTF commit test1
./WTF push   test1

# Q: suppose i commit and push a file, and then physically delete that file locally, after that I issue the update command -- what happens?? -- according to project description I won't get it
#testing update and upgrade - how to make serverside version different from the clientside?
./WTF update  test1
./WTF upgrade test1
# test scceeded.

# --------------------------------------------
# Test Case 1a: Create one Project. Add the same files twice.
# Expected Result: Error 
# --------------------------------------------
# make run_server
cd client
./WTF create test1
# Add 1st time:
echo "Test1, example 1 file" > test1/example1.txt
echo "Test1, example 2 file" > test1/example2.txt
echo "Test1, example 3 file" > test1/example3.txt
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt
# Add the 2nd time:
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt
# test ***FAILED*** Since .Manifest file lists the .txt files twices.

./WTF commit test1
# test ***FAILED*** Since .Commit file lists the .txt file stwices.

./WTF push   test1
# test ***FAILED*** Since server side .Manifest file lists the .txt file twices.


# --------------------------------------------
# Test Case 2: Create one Project, Add files, Commit, Push.
#              Then modify example1.txt, and remove example3.txt,
#              Then Commit, Push.
# --------------------------------------------
# make run_server
cd client
./WTF create test1
echo "Test1, example 1 file" > test1/example1.txt
echo "Test1, example 2 file" > test1/example2.txt
echo "Test1, example 3 file" > test1/example3.txt
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt

./WTF commit test1
./WTF push   test1
# Modify exmple1.txt
echo "Test1, example 1 file, modified" >> test1/example1.txt
# Remove exmple3.txt
./WTF remove test1 example3.txt

./WTF commit test1
./WTF push   test1
# test scceeded.

# --------------------------------------------
# Test Case 3: Create one Project, Add files, Commit, Push.
#              Then modify example1.txt, and remove example3.txt,
#              Then Commit, Push.
#              Try checkout - which should fail.
# --------------------------------------------
cd client
./WTF create test1
echo "Test1, example 1 file" > test1/example1.txt
echo "Test1, example 2 file" > test1/example2.txt
echo "Test1, example 3 file" > test1/example3.txt
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt

./WTF commit test1
./WTF push   test1
# Modify exmple1.txt
echo "Test1, example 1 file, modified" >> test1/example1.txt
# Remove exmple3.txt
./WTF remove test1 example3.txt
./WTF commit test1
./WTF push   test1
# test scceeded.

./WTF checkout test1
# test failed: project directory already exists.

# --------------------------------------------
# Test Case 4: Create one Project, Add files, Commit, Push.
#              Then modify example1.txt, and remove example3.txt,
#              Then Commit, Push.
#              REMOVE THE PROJECT LOCALLY, then
#              Try CHECKOUT - which should suceed.
# --------------------------------------------
cd client
./WTF create test1
echo "Test1, example 1 file" > test1/example1.txt
echo "Test1, example 2 file" > test1/example2.txt
echo "Test1, example 3 file" > test1/example3.txt
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt

./WTF commit test1
./WTF push   test1
# Modify exmple1.txt
echo "Test1, example 1 file, modified" >> test1/example1.txt
# Remove exmple3.txt
./WTF remove test1 example3.txt
./WTF commit test1
./WTF push   test1
# test scceeded.

# Remove the project locally.
rm -r test1
./WTF checkout test1
# test succeeded: Successfully checked out example2.txt & the modified file exmaple1.txt.

# --------------------------------------------
# Test Case 5: Create Two Projects Simultaneously: test1 & test2
# --------------------------------------------
# Create project: test1 ................................
cd client
./WTF create test1
echo "Test1, example 1 file" > test1/example1.txt
echo "Test1, example 2 file" > test1/example2.txt
echo "Test1, example 3 file" > test1/example3.txt
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt
./WTF commit test1
./WTF push   test1
echo "Test1, example 1 file, modified." >> test1/example1.txt

# Create project test2 ................................
./WTF create test2
echo "Test2, example 1 file" > test2/example1.txt
echo "Test2, example 2 file" > test2/example2.txt
echo "Test2, example 3 file" > test2/example3.txt
./WTF add test2 example1.txt
./WTF add test2 example2.txt
./WTF add test2 example3.txt
./WTF commit test2
./WTF push   test2

# Now remove test1/example1.txt 
.WTF remove test1 exmaple1.txt
./WTF commit test1
./WTF push   test1
# Now remove test2/example1.txt 
.WTF remove test2 exmaple1.txt
./WTF commit test1
./WTF push   test1

# Remove both the projects locally, then checkout again
rm -r test1
./WTF checkout test1
rm -r test2
./WTF checkout Test2
# test succeeded: Successfully checked out example2.txt & exmaple3.txt in both projects test1 & test2

# --------------------------------------------
# Test Case 6: Test update/upgrade twice: Should succeed.
# --------------------------------------------
# First execute test case 1 .................
./WTF update  test1
./WTF upgrade test1

./WTF update  test1
./WTF upgrade test1

# --------------------------------------------
# Test Case 7: Test Update with Conflicts: Should not fail.
# --------------------------------------------
# First execute test case 1 .................
# Introduce conflicts by changing client files.
echo "Test1, example 1 file, version 2" >> test1/example1.txt
echo "Test1, example 2 file, version 2" >> test1/example2.txt

# update should not fail
./WTF update  test1
./WTF upgrade test1

# --------------------------------------------
# Test Case 8: Test Update with Conflicts: New files on client side: Should not fail.
# --------------------------------------------
# First execute test case 1 .................
# Introduce conflicts, by adding new files on client
echo "Test1, example 4 file" > test1/example4.txt
echo "Test1, example 5 file" > test1/example5.txt
./WTF add test1 example4.txt
./WTF add test1 example5.txt

# update should not fail
./WTF update  test1
./WTF upgrade test1

# --------------------------------------------
# Test Case 9: Test checkout
# --------------------------------------------
cd client
./WTF create test1
echo "Test1, example 1 file" > test1/example1.txt
echo "Test1, example 2 file" > test1/example2.txt
echo "Test1, example 3 file" > test1/example3.txt
./WTF add test1 example1.txt
./WTF add test1 example2.txt
./WTF add test1 example3.txt

./WTF commit test1
./WTF push   test1

rm -r test1
./WTF checkout test1

