#!/bin/zsh 

# --------------------------------------------
# Test Case 1: One Project
# --------------------------------------------
# Create test1 ................................
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
# Result: ok

# --------------------------------------------
# Test Case 2: Two Projects: test1 & test2
# --------------------------------------------
# Create test1 ................................
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

./WTF update  test1
./WTF upgrade test1

# Create test2 ................................
./WTF create test2
echo "Test2, example 1 file" > test2/example1.txt
echo "Test2, example 2 file" > test2/example2.txt
echo "Test2, example 3 file" > test2/example3.txt
./WTF add test2 example1.txt
./WTF add test2 example2.txt
./WTF add test2 example3.txt

./WTF commit test2
./WTF push   test2

./WTF update  test2
./WTF upgrade test2

# --------------------------------------------
# Test Case 3: Test update/upgrade twice: Should succeed.
# --------------------------------------------
# First execute test case 1 .................
./WTF update  test1
./WTF upgrade test1

./WTF update  test1
./WTF upgrade test1

# --------------------------------------------
# Test Case 4: Test Update with Conflicts: Should not fail.
# --------------------------------------------
# First execute test case 1 .................
# Introduce conflicts by changing client files.
echo "Test1, example 1 file, version 2" >> test1/example1.txt
echo "Test1, example 2 file, version 2" >> test1/example2.txt

# update should not fail
./WTF update  test1
./WTF upgrade test1

# --------------------------------------------
# Test Case 5: Test Update with Conflicts: New files on client side: Should not fail.
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
# Test Case 6: Test checkout
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

