#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

//Global Variables
static char *commands;

//Public functions
void changeDirectory();
void setEnvVar();
void getEnvVar();
void runProgram();

/*Built in commands I need to remember
	exit
	pid
	ppid
	cd
	pwd
	set
	get
*/


//Code
int main(int argc, char **argv) {
	int i;
	char buffer[500]; //this is the buffer accepting the user input
	char *prompt; //this is what is displayed at the start of every line 
	prompt = "308sh>"; //the starting prompt is 308sh, this can be changed with -p
//	printf("%s", runProgram(argv[0]));
/*	for(i=0; i<argc; i++)
		printf("Option %d is \"%s\"\n", i, argv[i]);
		
	return 0; */
	while(1){
		printf("%s", prompt);
		
		fgets(buffer, sizeof buffer, stdin);
		commands = strtok(buffer, " \t\n()<>|;");

		if(commands == NULL || !commands){
			continue; //this will continue the while loop if there's no user input
		}else if(strcmp(commands, "exit") == 0){
			exit(0); //if they type exit, then exit. 
		}else if(strcmp(commands, "pid") == 0){ //print process id
			printf("Process ID: %u\n", getpid());
		}else if(strcmp(commands, "ppid") == 0){ //print parent process id
			printf("Parent Process ID: %u\n", getppid());
		}else if(strcmp(commands, "cd") == 0){ //change directory
			changeDirectory();
		}else if(strcmp(commands, "pwd") == 0){ //print working directory
			char cwd[1024];
			if(getcwd(cwd, sizeof(cwd)) != NULL){
	      			fprintf(stdout, "Current working directory: %s\n", cwd);
			}
		}else if(strcmp(commands, "set") == 0){ //set environment variable
			setEnvVar();
		}else if(strcmp(commands, "-p") == 0){ //change prompt
			commands = strtok(NULL, "\t\n"); //tokenize the string around newlines
			if(commands != NULL){
				int destinationSize = strlen(commands);
				prompt = (char *) malloc(destinationSize + 1); //reallocate memory to the size of the new string
				int j;
				for(j = 0; j < destinationSize; j++){
					prompt[j] = commands[j];
				}
				prompt[destinationSize] = '>';
				prompt[destinationSize+1] = '\0';
			}
		}else if(strcmp(commands, "get") == 0){ //get environment variable
			getEnvVar();
		}else{ //if nothing else, try to run it as a program
			runProgram(commands);
		}
	}
	return 0;
}

void changeDirectory(){
	char *home;
	home = getenv("HOME");
	commands = strtok(NULL, " \t\n()<>|;");
	if(commands == NULL){ //if they didn't enter a directory to change to it goes to home
		if(home == NULL){ //if the HOME variable doesn't exist then exit
			printf("Home variable not set\n");
			return;
		}		
		if(chdir(getenv("HOME")) == -1){ //go to home
			
		}
	}else if(commands[0] == '~'){ //if the user enters "cd ~" it goes relative to home
		if(home == NULL){ 
			printf("Home variable not set\n");
			return;
		}
		char currentPath[500];
		strcpy(currentPath, home);
		commands = strcat(currentPath, &commands[1]);
	}

	else if(chdir(commands) == -1){ //change directory and print an error if there is one 
		perror("Error changing directory");
	}else{
		char cwd[1024];
		if(getcwd(cwd, sizeof(cwd)) != NULL){
	      		fprintf(stdout, "New working directory: %s\n", cwd);
		}
	}
}

void setEnvVar(){ //Set environment variable
	commands = strtok(NULL, " \t\n()<>|;");
	char *arguments[500];
	int numOfArgs = 0;
	arguments[0] = commands; 
	while(commands != NULL){
		commands = strtok(NULL, " \t\n()<>|;");
		numOfArgs = numOfArgs + 1;
		arguments[numOfArgs] = commands;
	}
	if(arguments[1] != NULL){ 
		setenv(arguments[0], arguments[1], 1);
		printf("Variable %s set to %s\n", arguments[0], arguments[1]);
	}else{
		unsetenv(arguments[0]);
		printf("Variable %s unset\n", arguments[0], arguments[1]);
	}
}

void getEnvVar(){ //Get environment variable
	commands = strtok(NULL, " \t\n()<>|;");
	if(getenv(commands) == NULL) { //tries getting the environment variable and throws an error if it doesn't exist.
		perror("Error changing directory");
	}else{
		printf("%s\n", getenv(commands));
	}
}

void runProgram(char *command1){ //run the program entered
	//commands = strtok(NULL, " \t\n()<>|&;");
	//printf("Running program %s\n", command1);
	pid_t pif, wpid;
	int status = 0;
	int runBackground = 0;
	char *arguments[500];
	arguments[0] = command1;
	int numCommands = 1;
	while(commands != NULL){ //This loop will go through every command entered and separate them and look for an ampersand.
		if(strcmp(commands, "&") == 0){ //if there is an ampersand run the program in the background
			runBackground = 1;
			commands = strtok(NULL, " \t\n()<>|;");
		}else{
			commands = strtok(NULL, " \t\n()<>|;");
			arguments[numCommands] = commands;
			numCommands++;
		}
	}
	arguments[numCommands - 1] = NULL;
	int pid = fork(); //spawn a child process

	if(pid == -1){
		perror("Error");
		exit(EXIT_FAILURE);
	}else if(pid == 0){ //This is done by the child


   		if ((execvp(arguments[0], arguments) == -1)) { //execvp will run whatever is entered as a program. arguments[0] is the program to run
   			perror("Error"); //print an error if there is one
   		}
		
		exit(EXIT_SUCCESS);	
	}
	else {
  	
        if(runBackground==1){ //if there was an ampersand this will let it run but still print the result when the child finishes.
		wpid = waitpid(pid, &status, WNOHANG);
	}else{ //if there wasn't an ampersand the parent will wait for the child to finish
        	wpid = waitpid(pid, &status, WUNTRACED);
	}
    	    
	printf("[%u] %s Exit %d\n", wpid, command1, WIFEXITED(status));
  }
}

/* I decided against using this
void userInput(){
	int temp;
	int i = 0;
	printf("%s", prompt);
	scanf("%[^\n]s", buffer);
	printf("%s\n", buffer);
	
/*	while((temp = getchar()) != EOF && temp !='\n'){
		buffer[i] = temp;
		i++;
		printf("%d", i);
		break;
	}
}*/





