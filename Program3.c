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
		int i = 0, j = 0;		//Utility vars for incrementing

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

		//Replace $$ with PID
		pid_t myPID = getpid();
		char myPIDstr[7];
		int PIDlen;
		sprintf(myPIDstr, "%d", myPID);
		PIDlen = strlen(myPIDstr);

		for(i = 0; i < inptLen; i++){
			if(inptBuffer[i] == '$' && inptBuffer[i+1] == '$'){
				//Make room for PID in inptBuffer
				for(j = strlen(inptBuffer)-1; j > i+1; j--){
					inptBuffer[j+PIDlen-2] = inptBuffer[j];
				}
				for(j = 0; j < PIDlen; j++){
					inptBuffer[i+j] = myPIDstr[j];
				}
			}
		}

		printf("inptBuffer: %s\n", inptBuffer);

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

		for(i = 0; i < numInptArgs; i++){
			printf("%s ", inptArgs[i]);
		}
		printf("\n");

		//Exit
		if(!strcmp("exit", inptArgs[0])){
			printf("Exiting..\n");
			exit(0);
		}

		//cd
		else if(!strcmp("cd", inptArgs[0])){
			chdir(inptArgs[1]);
		}

		//Status
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