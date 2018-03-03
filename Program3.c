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
		int sourceFD, targetFD; //File descriptors
		char* args[512];		//Arguments to be passed to execvp

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
				//Insert PID
				for(j = 0; j < PIDlen; j++){
					inptBuffer[i+j] = myPIDstr[j];
				}
			}
		}

		char* inptArgs[512];		//Array of cmd line input
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

		//Exit
		if(!strcmp("exit", inptArgs[0])){
			//printf("Exiting..\n");
			exit(0);
		}

		//cd
		char* homeDir = getenv("HOME");
		if(!strcmp("cd", inptArgs[0])){
			//If no argument given to cd
			if(!strcmp("\0", inptArgs[1])){
				printf("Changing to %s\n", homeDir);
				chdir(homeDir);
			}
			//Argument given to cd
			else{
			chdir(inptArgs[1]);
			}
		}

		//Status
		else if(!strcmp("status", inptArgs[0])){

		}
		else if(inptArgs[0][0] != '#'){

			spawnpid = fork();
			switch (spawnpid){

				case -1:{
					perror("ErRoR!");
					exit(1);
					break;
				}
				//Child
				case 0:{

					j = 0;		//Incrementor for args

					for(i = 0; i < numInptArgs; i++){

						//Redirect stdin
						if(inptArgs[i][0] == '<'){
							printf("Pointing stdin to %s\n", inptArgs[i+1]);
							sourceFD = open(inptArgs[i+1], O_RDONLY);
							if(sourceFD == -1){
								printf("Failed to open file %s\n", inptArgs[i+1]);
								perror("error opening file");
								exit(1);
							}
							fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
							dup2(sourceFD, 0);
						}
						//Redirect stdout
						else if(inptArgs[i][0] == '>'){
							printf("Pointing stdout to %s\n", inptArgs[i+1]);
							targetFD = open(inptArgs[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);

							//Error if failed to open
							if(targetFD == -1){
								perror("error opening file");
								exit(1);
							}
							else{
								fcntl(targetFD, F_SETFD, FD_CLOEXEC);
								dup2(targetFD, 1);
							}
						}
						//Add arguments for exec
						else if((i != 0 && inptArgs[i-1][0] != '>' && inptArgs[i-1][0] != '<') || i == 0){
							args[j] = inptArgs[i];	
							j++;
						}
					}
					args[j] = NULL;
					execvp(args[0], args);
					exit(0);
					break;
				}
				//Parent
				default:{
					pid_t actualPid = waitpid(spawnpid, &childExitStatus, 0);
					//printf("I am the parent!\n"); fflush(stdout);
					break;
				}
			}
		}
	}
}