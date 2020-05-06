#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "src/commands/add.h"
#include "src/commands/checkout.h"
#include "src/commands/commit.h"
#include "src/commands/create.h"
#include "src/commands/remove.h"
#include "src/configure.h"
#include "src/util/color.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_file_size.h"

bool TESTING = true;

void system1(char* command) {
  printf("%s\n", command);
  system(command);
}

int main(int argc, char** argv){
	int i;
  char junk;
  int* wstatus;

//TESTING CONFIGURE ---------------------------------
  system("pwd");

  printf("Test Case -1: 'checkout' project without running configure. Expect failure.\n");
  scanf("%c", &junk);
  i = fork();
	if(!i){
		system1("../client/WTF checkout non_existent_project");
		exit(0);
	}
  wait(wstatus);

	printf("Test Case 1: Configure. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
 		system1("../client/WTF configure 127.0.0.1 8000");
		exit(0);
	}
	wait(wstatus);

//TESTING CREATE -----------------------------------
	printf("Test Case 2: 'Create' project. Expect success.\n");
  scanf("%c", &junk);
 	i = fork();
	if(!i){
		system1("../client/WTF create proj1");
		exit(0);
	}
	wait(wstatus);
  

	printf("Test Case 3: 'Create' project when it already exists. Expect failure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF create proj1");
		exit(0);
	}
	wait(wstatus);
  

//TESTING ADD -----------------------------------
	printf("Test Case 4: 'Add' three files to the project. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("echo 'example file 1' > proj1/ex1.txt");
		system1("echo 'example file 2' > proj1/ex2.txt");
    system1("echo 'example file 3' > proj1/ex3.txt");
		system1("../client/WTF add proj1 ex1.txt");
		system1("../client/WTF add proj1 ex2.txt");
		system1("../client/WTF add proj1 ex3.txt");
		exit(0);
	}
	wait(wstatus);
 

	printf("Test Case 5: Attempt to 'Add' a file that is already in the project. Expect failure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF add proj1 ex1.txt");
		exit(0);
	}
	wait(wstatus);
 
//TESTING COMMIT AND PUSH --------------------------

  printf("Test Case 6: Attempt to 'Push' without a .commit file. Expect faiure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF push proj1");
		exit(0);
	}
	wait(wstatus);
 
  printf("Test Case 7: 'Commit' the project. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF commit proj1");
		exit(0);
	}
	wait(wstatus);


  printf("Test Case 8: 'Push' the project after commit. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF push proj1");
		exit(0);
	}
	wait(wstatus);


  printf("Test Case 9: Attempting to 'commit' the project without any differences. Expect success.\n");
  scanf("%c", &junk);
  i = fork();
	if(!i){
		system1("../client/WTF commit proj1");
		exit(0);
	}
	wait(wstatus);


  printf("Test Case 10: Modify a file ex1.txt, then 'commit' & 'push'. Expect success.\n");
  scanf("%c", &junk);  
	i = fork();
	if(!i){
		system1("echo 'Exmaple 1 modified' >> proj1/ex1.txt");
		system1("../client/WTF commit proj1");
		system1("../client/WTF push   proj1");
		exit(0);
	}
	wait(wstatus);

//TESTING UPGRADE AND UPDATE--------------------------------------------
  printf("Test Case 11: Attempting to 'Upgrade' the Project without a .update file. Expect failure.\n");
  scanf("%c", &junk);  
	i = fork();
	if(!i){
    system1("../client/WTF upgrade proj1");
		exit(0);
	}
	wait(wstatus);

  printf("Test Case 12: Attempting to 'Update' the Project without any removed Project files. Expect 'Up to date'.\n");
  scanf("%c", &junk);  
	i = fork();
	if(!i){
    system1("../client/WTF update proj1");
		exit(0);
	}
	wait(wstatus);


//TESTING REMOVE--------------------------------------------
  printf("Test Case 13: 'Remove' a Project file. Expect success.\n");
  scanf("%c", &junk);  
	i = fork();
	if(!i){
    system1("../client/WTF remove proj1 ex1.txt");
		exit(0);
	}
	wait(wstatus);

  printf("Test Case 14: Attempting to 'Remove' a file that is not being tracked. Expect failure.\n");
  scanf("%c", &junk);  
	i = fork();
	if(!i){
    system1("../client/WTF remove proj1 ex1.txt");
		exit(0);
	}
	wait(wstatus);

  printf("Test Case 15: Removing Project files, then Update the Project. Expect success.\n");
  scanf("%c", &junk);  
	i = fork();
	if(!i){
    system1("../client/WTF update proj1 ex1.txt");
		exit(0);
	}
	wait(wstatus);

  printf("Test Case 16: 'Upgrade' the Project. Expect success.\n");
  scanf("%c", &junk);  
	i = fork();
	if(!i){
    system1("../client/WTF upgrade proj1");
		exit(0);
	}
	wait(wstatus);

  printf("Test Case 17: Modifing a Project file and attempting to Update\n");
  scanf("%c", &junk); 
	i = fork();
	if(!i){
		int contents = open("client/ex1.txt", O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR);
		write(contents, "stuff", 5);
		close(contents);
		system1("../client/WTF update proj1");
		exit(0);
	}
	wait(wstatus); 


  // CHECKOUT 
  printf("Test Case 18: First delete project locally, then 'checkout' a project. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("rm -rf proj1");
    system1("../client/WTF checkout proj1");
		exit(0);
	}
  wait(wstatus);

  // current version
  printf("Test Case 19: Get currentversion of a project. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF currentversion proj1");
		exit(0);
	}
  wait(wstatus);

  printf("Test Case 20: Get currentversion of a project that does not exist. Expect failure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF currentversion non_existent_project");
		exit(0);
	}
  wait(wstatus);

  // history
  printf("Test Case 21: Get 'history' of a project. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF history proj1");
		exit(0);
	}
  wait(wstatus);

  printf("Test Case 22: Get 'history' of a project that does not exist. Expect failure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF history non_existent_project");
		exit(0);
	}
  wait(wstatus);


  printf("Test Case 23: 'checkout' a project when it already exists locally. Expect failure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF checkout proj1");
		exit(0);
	}
  wait(wstatus);

  // rollback
  printf("Test Case 24: 'rollback' an existing project. Expect success.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF rollback proj1 1");
		exit(0);
	}
  wait(wstatus);

  printf("Test Case 25: 'rollback' a project that does not exist. Expect failure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF rollback non_existent_project 1");
		exit(0);
	}
  wait(wstatus);

  // destroy ...............
  printf("Test Case 26: 'Destroy' the Project. Expect success.\n");
  scanf("%c", &junk); 
	i = fork();
	if(!i){
		system1("../client/WTF destroy proj1");
		exit(0);
	}
	wait(wstatus);
	

	printf("Test case 27: Attempting to 'Destroy' the Project when it doesn't exist. Expect failure.\n");
  scanf("%c", &junk); 
  i = fork();
	if(!i){
		system1("../client/WTF destroy proj1");
		exit(0);
	}
	wait(wstatus);


  printf("Test Case 28: 'checkout' a project after we have destryed the project. Expect failure.\n");
  scanf("%c", &junk);
	i = fork();
	if(!i){
		system1("../client/WTF checkout proj1");
		exit(0);
	}
  wait(wstatus);




  printf("End of tests. Bye!\n");
}
