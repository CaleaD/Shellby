#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <fcntl.h>

#define BLACK "\e[0;30m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"
#define BLUE "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN "\e[0;36m"
#define END "\e[0m"

// Mentions the code should be compiled as such: 
// Open up the terminal and type "gcc Shell.c -o Shell.out -lreadline" .
// Then type "./Shell.out".*/

#define MAX_COMMAND_LENGTH 1000
#define MAX_PATH_LENGTH 200

//--------------------HELP FUNCTIONS--------------------

void version(){
	printf("\tShellby (West University of Timisoara) v1.0\n\tCopyright (C) 2020 Calea Dora-Ana.\n\tThis is free software: you are free to change and redistribute it.\n\n");
	printf("-> Used GNU history library.\n");
}

void wc_help(){
	printf(YELLOW"wc [OPTION]... [FILE]...\n\n"END);
	printf(GREEN"Implemented options: -c, -w, -l, -L -v\n\n"END);
	printf(GREEN"-c"END" : print the byte counts\n");
	printf(GREEN"-w"END" : print the word counts\n");
	printf(GREEN"-l"END" : print the newline counts\n");
	printf(GREEN"-L"END" : print the length of the longest line\n");
	printf("\n");

}

void expand_help(){
	printf(YELLOW"expand [OPTION]... [FILE]...\n\n"END);
	printf(GREEN"Implemented options: -t, -i\n\n"END);
	printf(GREEN"-t"END" : have tabs NUMBER characters apart, not 8\n");
	printf(GREEN"-i"END" : do not convert tabs after non blanks\n");
	printf("\n");
}

void env_help(){
	printf(YELLOW"env [OPTION]... [-] [NAME=VALUE]... [COMMAND [ARG]...]\n\n"END);
	printf(GREEN"Implemented options: -u\n\n"END);
	printf(GREEN"-u"END" : remove variable from the environment \n");
	printf("\n");
}

void help(){
	int choice;
	int end = 1;
	printf(YELLOW"\t\t\t* SELF IMPLEMENTED SHELL COMMANDS *\n");
	printf("\t(1) wc - print newline, word, and byte counts for each file\n");
	printf("\t(2) expand - convert tabs to spaces\n");
	printf("\t(3) env - run a program in a modified environment/print environment variables \n");
	printf("\t(4) version (standalone) - offers author information, versioning information, and other valuable information\n");
	printf("\t(5) quit\n\n"END);

	while(end){
		printf("Please enter a number between 1-5 (1 - wc, 2 - expand, 3 - env, 4 - version, 5 - quit): ");
		scanf("%d", &choice);
		printf("\n");
		switch(choice){
			case 1: wc_help(); break;
			case 2: expand_help(); break;
			case 3: env_help(); break;
			case 4: version(); break;
			case 5: end=0; break;
			default: printf("Error: Not an option.\n");
		}
	}

}

//--------------------HELP FUNCTIONS--------------------

//--------------------CUSTOM COMMANDS--------------------

//multiple options do not work (ex wc -c -l test.txt)
//however multiple files work (ex wc -L test1.txt test2.txt)

int* analyse_file(int* arr,char* f, int w, int l, int c, int max_line){

        FILE *file; //= fopen( f, "r");
        char ch;
        size_t size;
        int c_per_line = 0;


        if ((file = fopen(f, "r")) == NULL){

        	printf("%s\n", f);
            printf("Error: Could not open the file.\n");
            perror("fopen");
	        arr[0] = -1;
	        arr[1] = -1;
	        arr[2] = -1;
	        arr[3] = -1;
	        return arr;
        }
        else {
            fseek (file, 0, SEEK_END);
            size = ftell(file);
            fseek (file, 0, SEEK_SET);
            char buff[10000];

            if(size != 0) w++; //size == 0 

            while ((ch = fgetc(file)) != EOF){
                buff[c++] = ch;
                //printf("%d\n", c_per_line);
                if(ch != '\n' && ch != '\0'){
                	c_per_line++;
                    //printf("%c -> %c\n",buff[c-2],ch);
                    if(buff[c-2] == ' ' && ch != ' '){
                        w++;
                    }
                }
                if(ch == '\n' || ch == '\0') {
                    w++;
                    l++;
                    if(c_per_line>max_line)
                    	max_line = c_per_line;
                    c_per_line = 0;
                }
            }

            //For testing buffer content
            /*for(int i=0;i<size;i++){
                printf("%c", buff[i]);
            }*/

	        fclose(file);
	        arr[0] = l;
	        arr[1] = w;
	        arr[2] = size;
	        arr[3] = max_line;
	        return arr;

        }

        //Cohesive printing

        /*printf("\n");
        printf("Lines: %d\n", l);
        printf("Words: %d\n", w);
        printf("Chars: %d\n", c);
        printf("Size in bytes: %d\n", size);*/
        //fclose(file);
}

void wc_command(char**cmd_parsed,int num){
    int words = 0;
    int lines = 0;
    int characters = 0;
    int values[4];
    int *ptr;
    int flag = 0;
    char opt;
    int max_line = 0;
    //We should reset optind each time after use
    optind = 1;

    while((opt = getopt(num,cmd_parsed,"c:w:l:L:")) != -1){
        switch(opt){        	
            case 'c': {
                flag = 1;
                if(num == 3){
                    ptr = analyse_file(values,optarg,words,lines,characters,max_line);
                     printf(" %d %s\n", ptr[2], optarg);
                }
                else if(num > 3) {
                    for(int i=2;i<num;i++){
                        ptr = analyse_file(values,cmd_parsed[i],words,lines,characters,max_line);
                    	if(ptr[2]!=-1) {
                            characters+=ptr[2];
                    		printf(" %d %s\n", ptr[2], cmd_parsed[i]);
                        }
                    }
                    printf(" %d total\n", characters);

                }
                break;
            }
            case 'w': {
                flag = 1;
                if(num == 3){
                    ptr = analyse_file(values,optarg,words,lines,characters,max_line);
                     printf(" %d %s\n", ptr[1], optarg);
                }
                else if(num > 3) {
                    for(int i=2;i<num;i++){
                        ptr = analyse_file(values,cmd_parsed[i],words,lines,characters,max_line);
                        if(ptr[1]!=-1) {
	                        printf(" %d %s\n", ptr[1], cmd_parsed[i]);
	                        words+=ptr[1];
                    	}
                    }
                    printf(" %d total\n", words);

                }
                break;
            }
            case 'l':{
                flag = 1;
                if(num == 3){
                    ptr = analyse_file(values,optarg,words,lines,characters,max_line);
                     printf("%d %s\n", ptr[0], optarg);
                }
                else if(num > 3) {
                    for(int i=2;i<num;i++){
                        ptr = analyse_file(values,cmd_parsed[i],words,lines,characters,max_line);
                        if(ptr[0]!=-1) {
	                        printf("%d %s\n", ptr[0], cmd_parsed[i]);
	                        lines+=ptr[0];
                    	}
                    }
                    printf(" %d total\n", lines);

                }

                break;
            }
            case 'L':
                flag = 1;
                int max = 0;
                if(num == 3){
                    ptr = analyse_file(values,optarg,words,lines,characters,max_line);
                    if(ptr[3]!=-1) printf("%d %s\n", ptr[3], optarg);
                }
                else if(num > 3) {
                    for(int i=2;i<num;i++){
                        ptr = analyse_file(values,cmd_parsed[i],words,lines,characters,max_line);
                        if(ptr[3]!=-1) {
	                        printf("%d %s\n", ptr[3], cmd_parsed[i]);
	                        if(ptr[3]>max) max = ptr[3];
                    	}
                    }
                    printf(" %d total\n", max);

                }
                break;
            default: break;
        }
    }


    if(num<=1){
        printf("Error: wc should have the syntax wc [OPTION]... [FILE]...\n");
    }
    else if(flag == 0 && num == 2){
        ptr = analyse_file(values,cmd_parsed[1],words,lines,characters,max_line);
        printf(" %d  %d %d %s\n",ptr[0],ptr[1],ptr[2],cmd_parsed[1]);
    }
    else if(flag == 0) {
        for(int i=1;i<num;i++){
            ptr = analyse_file(values,cmd_parsed[i],words,lines,characters,max_line);
            printf(" %d  %d %d %s\n",ptr[0],ptr[1],ptr[2],cmd_parsed[1]);
            lines+=ptr[0];
            words+=ptr[1];
            characters+=ptr[2];
        }
        printf("%d  %d %d total\n", lines,words,characters);
    }
}


void analyse_tabs_infile(char* f, int spaces, int iflag){
    FILE *file;
    char ch;

    if ((file = fopen(f, "r")) == NULL){
        printf("Error: Could not open the file.\n");
        perror("fopen");
    }
    else if(iflag == 0) {
        char buff[10000];
        char out[10000];
        int i = 0,j = 0;

        while ((ch = fgetc(file)) != EOF){
            if(ch == '\t'){
                for(int i=0;i<spaces;i++){
                    out[j++] = ' ';
                }
            }
            else {
                buff[i++] = ch;
                out[j++] = ch;
            }
        }

        i = 0;
        while(out[i]!='\0'){
            printf("%c", out[i]);
            i++;
        }
    }
    else {
        char buff[10000];
        char out[10000];
        int i = 0,j = 0;

        while ((ch = fgetc(file)) != EOF){
            if(ch == '\t' && buff[i-1]==' ' && buff[i-1]=='\n'){
                for(int i=0;i<spaces;i++){
                    out[j++] = ' ';
                }
            }
            else {
                buff[i++] = ch;
                out[j++] = ch;
            }
        }

        i = 0;
        while(out[i]!='\0'){
            printf("%c", out[i]);
            i++;
        }

    }
    fclose(file);
}

void expand_command(char**cmd_parsed,int num){
    char opt;
    int flag = 0;
    //We should reset optind each time after use
    optind = 1;
    while((opt = getopt(num,cmd_parsed,"t:i:"))!=-1){
        switch(opt){
            case 't': {
            	flag = 1;
                int sp = atoi(optarg);
                analyse_tabs_infile(cmd_parsed[2],sp,0);
                break;
            }
            case 'i': {
            	flag = 1;
                analyse_tabs_infile(cmd_parsed[2],8,1);
                break;
            }
            	//There is no -a option for expand.
            default: break;
        }
    }

    if(flag == 0){
	    if(num >= 2){
		    	for(int i=1;i<num;i++){
		    		analyse_tabs_infile(cmd_parsed[i],8,0);
		    	}
	    	}
	    else if(num <= 1) {
	        printf("Error: expand should have the syntax expand [OPTION]... [FILE]\n");
	    }
    }
}

extern char **environ;

char* string_before_char(char* str, char ch){
    //returns string that ends with character ch
    int i = 0;
    while(str[i]!='\0'){
        if(str[i] == ch)
            str[i] = '\0';
        i++;
    }
    return str;
}

void env_command(char**cmd_parsed,int num){
    char opt;
    //We should reset optind each time after use
    optind = 1;
    while((opt = getopt(num,cmd_parsed,"u:"))!=-1){
        switch(opt){
            case 'u': {
                        if(unsetenv(optarg) == 0)
					        for (char **env = environ; *env != 0; env++){
					            char *current = *env;
					            printf("%s\n", current);    
					        }
					    else{
					    	printf("There was an error.\n");
					    }

                      break;
                      }
            default: break;
        }
    }


    if(num==1)
        for (char **env = environ; *env != 0; env++){
            char *current = *env;
            printf("%s\n", current);    
        }
}

//--------------------CUSTOM COMMANDS--------------------


//------------------------SHELL--------------------------

void display_history(){
    printf("History length: %d\n", history_length);
    //write_history ("history_file");
    //read_history("history_file");
    HIST_ENTRY **history = history_list();
    int i = 0;
    while(history[i] && history!=NULL){
        printf ("%d %s\n", i+history_base, history[i]->line);
        i++;
    }
}

void execute(char* cmd,char** cmd_parsed){

    pid_t pid = fork();

    if (pid == 0){ 
        if (execvp(cmd, cmd_parsed) < 0) {
            printf("The command was not executed properly.\n"); 
        }
    }
    else if (pid == -1) { 
        printf("Fork process failed.\n");
    }
    else {
        //We wait for the child to finish
        wait(NULL);
    } 
}

char *validCommand(char* command){
    char *path = "/bin";
    char *str = malloc (sizeof (char) * MAX_PATH_LENGTH);

    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return NULL;

    while ((entry = readdir(dir)) != NULL) 
    {
        //printf("%s\n",entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
              continue;

        if (entry->d_type == DT_REG && strcmp(entry->d_name,command) == 0){
            //printf("%s\n",path);
            strcat(str,path);
            strcat(str,"/");
            strcat(str,command);
            //printf("%s\n",str);

            return str;
        }
    }

    printf("The command was not recognized.\n");
    return NULL;
}

//------------------PIPES, RED & PARSING-----------------

int parse_args(char* cmd_line, char** cmds){
    char *str = malloc (sizeof (char) * MAX_COMMAND_LENGTH);
    strcpy(str,cmd_line);
    char *p = strtok(str, " \0\t\n");
    int count = 0;
    while(p != NULL)
    {
        //printf("'%s'\n", p);
        *cmds++ = p;
        p = strtok(NULL, " ");
        count++;
    }
    *cmds = '\0';
    return count;

}

int hasPipes(char* cmd_line){
    for(int i=0;i<MAX_COMMAND_LENGTH;i++){
        if(cmd_line[i] == '|')
            return 1;
    }
    return 0;
}

int hasInRedirection(char* cmd_line){
    for(int i=0;i<MAX_COMMAND_LENGTH;i++){
        if(cmd_line[i] == '<')
            return 1;
    }
    return 0;
}

int hasOutRedirection(char* cmd_line){
    for(int i=0;i<MAX_COMMAND_LENGTH;i++){
        if(cmd_line[i] == '>')
            return 1;
    }
    return 0;
}

int parse_pipes(char* cmd_line, char* cmds[]){
    char *str = malloc (sizeof (char) * MAX_COMMAND_LENGTH);
    strcpy(str,cmd_line);
    char *p = strtok(str, "|");
    int count = 0;
    while(p != NULL)
    {
        *cmds++ = p;
        p = strtok(NULL, "|");
        count++;
    }
    *cmds++ = '\0';
    return count;
}

int parse_in_red(char* cmd_line, char* cmds[]){
    char *str = malloc (sizeof (char) * MAX_COMMAND_LENGTH);
    strcpy(str,cmd_line);
    char *p = strtok(str, "<");
    int count = 0;
    while(p != NULL)
    {
        *cmds++ = p;
        p = strtok(NULL, "<");
        count++;
    }
    *cmds++ = '\0';
    return count;
}

int parse_out_red(char* cmd_line, char* cmds[]){
    char *str = malloc (sizeof (char) * MAX_COMMAND_LENGTH);
    strcpy(str,cmd_line);
    char *p = strtok(str, ">");
    int count = 0;
    while(p != NULL)
    {
        *cmds++ = p;
        p = strtok(NULL, ">");
        count++;
    }
    *cmds++ = '\0';
    return count;
}

void piping(char** cmds, int num){
	pid_t pid;
    int fd[2];
    char *args[1000];
    int in = 0;
    
    for(int i=0;i<num;i++){
    	int args_num = parse_args(cmds[i],args);
    	pipe(fd);

		if (pipe(fd) == -1) {
	      perror("Pipe error.");
	      exit(1);
	    }
	    if ((pid=fork()) == -1) {
	      perror("Fork error.");
	      exit(1);
	    }

	    if(pid == 0){
    		dup2(in,0);
    		if(cmds[i+1] != NULL)
    			dup2(fd[1],1);
    		close(fd[0]);
			execvp(args[0], args); //It uses the external commands
			exit(1);
    	}
	    else {
	    	wait(NULL);
	    	close(fd[1]);
	    	in = fd[0];
	    }


    }

}

void redirection(char** cmds, int num, int mode){
	int fd;
    char *args[1000];

	if(num == 2){
		if(mode == 0){
			//OUTPUT MODE
			printf("%s\n", cmds[0]);
			fd = open(cmds[1], O_CREAT | O_WRONLY, 0644);
			if(fd < 0) {
				perror("Couldn't open file.\n");
				exit(1);
			}
			dup2(fd,STDOUT_FILENO);
			close(fd);
			if(hasPipes(cmds[0])){
            	int num = parse_pipes(cmds[0],args);
	    		piping(args,num);
    		}
    		else {
    		int args_num = parse_args(cmds[0],args);
    		execvp(args[0], args);
    		}
		}
		else {
			//INPUT MODE - doesn't work
			int args_num = parse_args(cmds[0],args);
			fd = open(cmds[0], O_RDONLY);
			if(fd < 0) {
				perror("Couldn't open file.\n");
				exit(1);
			}
			dup2(fd,STDIN_FILENO);
			close(fd);
			execvp(args[0], args);
		}
		

	}
	else {
		printf("Too many redirects.\n");
	}
}




//------------------PIPES, RED & PARSING-----------------



void runShell(){
	while(1){
		char *args[1000];
        char *cmd_line;
        cmd_line = readline(MAGENTA"Shellby"BLUE"> "END);
        add_history(cmd_line);


    	if(hasOutRedirection(cmd_line)){
    		int num = parse_out_red(cmd_line,args);
    		redirection(args,num,0);
    	}
    	else if(hasInRedirection(cmd_line)){
    		int num = parse_in_red(cmd_line,args);
    		redirection(args,num,1);
    	}
		else if(hasPipes(cmd_line)){
            int num = parse_pipes(cmd_line,args);
	    	piping(args,num);
    	}
    	else {
	        int num = parse_args(cmd_line,args);
	        if(strcmp(args[0],"exit")==0 && num==1) {
	        	exit(0);
	        }
	        else if(strcmp(args[0],"history")==0 && num==1) {
	            display_history();
	        }
	        else if(strcmp(args[0],"help")==0 && num==1){
	        	help();
	        }
	        else if(strcmp(args[0],"wc")==0){
	            wc_command(args,num);
	        }
	        else if(strcmp(args[0],"expand")==0){
	            expand_command(args,num);
	        }
	        else if(strcmp(args[0],"env")==0){
	            env_command(args,num);
	        }
	        else if(strcmp(args[0],"version")==0){
	        	version();
	        }
	        else{
	            if(validCommand(args[0]))
		    		execute(*args,args);
	        }
    	}
    	
	}
}


//------------------------SHELL--------------------------

int main(int argc, char *argv[],char **envp) {
    runShell();
    return 0;
}