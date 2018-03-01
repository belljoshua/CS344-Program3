#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

int main(){

	pid_t spawnpid = -5;
	int childExitStatus = -5;
	
	while(1){
		int i = 0;		//Utility var for incrementing

		//Get user input
		char* inptBuffer = NULL;
		size_t bufferSize = 0;
		char usrInpt[256];
		int curChar = 0;		//For incrementing through input
		int inptLen = 0;
		int numInptArgs = 0;

		printf(": ");
		inptLen = getline(&inptBuffer, &bufferSize, stdin);
		inptBuffer[inptLen-1] = '\0';

		char* inptArgs[512];
		for(i = 0; i < 512; i++){
			inptArgs[i] = "\0";
		}

		char* pch;
		pch = strtok(inptBuffer, " ");
		i = 0;
		while(pch != NULL){
			inptArgs[i] = pch;
			pch = strtok(NULL, " ");
			i++;
		}
		numInptArgs = i;


		if(!strcmp("exit", inptArgs[0])){
			printf("Exiting..\n");
			exit(0);
		}
		else if(!strcmp("cd", inptArgs[0])){

		}
		else if(!strcmp("status", inptArgs[0])){

		}
		else if(inptArgs[0][0] != '#'){
			printf("Not a comment..\n");

			spawnpid = fork();
			switch (spawnpid){

				case -1:{
					perror("ErRoR!");
					exit(1);
					break;
				}
				case 0:{
					printf("Running your command..\n"); fflush(stdout);
					execlp(inptArgs[0], inptArgs[0], NULL);
					exit(0);
					break;
				}
				default:{
					pid_t actualPid = waitpid(spawnpid, &childExitStatus, 0);
					printf("I am the parent!\n"); fflush(stdout);
					break;
				}
			}
			printf("this will be executed by both of us!\n");
		}
	}
}