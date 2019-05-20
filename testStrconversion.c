#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <slcurses.h>
#include <fcntl.h>

void single_command(char * command);

int contains_character(char *command, char tocheck);

bool handle_redirection(char*);

int main() {
    int pid = getpid();
    //fd 0 is stdin 1 for stdout and 2 for stderr
//    char command[200]="cat filetoCheck.txt | wc -l | wc -l";
    //can
//    char *command="cat /bin/ls | sort | exit | wc -l 2>&1 1>output.txt";
    char command[200] = "cat /bin/ls | sort | uniq | wc -l 2>&1 1>output.txt";
//    fgets(command,200,stdin);
//    gets(command);
//   printf("%s\n",pch);
//    if (pch == NULL)
//       printf("Checking null %d\n",pch == NULL);
//    while (pch!=NULL){
//        ptr = strtok(NULL, delim);
//    }
//    printf("Contains pipe: %d",contains_character(command));
    while (1){
//        printf(">");
//        free(command);
        strcpy(command,"");
        scanf("%[^\n]%*c", command);

        char *pch = malloc(200);
        strcpy(pch, command);

        while (contains_character(pch, '|')) {
            pch = strtok(pch, "|");
            //        printf ("%s\n",pch);

            int pid;
            int fd[2];

            if (pipe(fd) < 0) {
                printf("error\n");
                exit(-1);
            }

            pid = fork();
            if (pid == 0) {
                //stdin is 0
                close(1);//removes stdout
                dup(fd[1]);//opens out for pipe
                close(fd[1]);// ensures other doesnt write on this
                close(fd[0]);
                //            printf("Child pch %s\n",pch);
                single_command(pch);
                exit(-1);
                //            pch = strtok (NULL, "|");
                //            execv();
            } else if (pid > 0) {
                pch = strtok(NULL, "");
                close(0);
                dup(fd[0]);
                close(fd[0]);
                close(fd[1]);
                //            wait(NULL);
                //            printf("Parent pch %s\n",pch);

            } else {
                //            printf("Unable to fork\n");
                exit(-1);
            }

            //        single_command(pch);
        }

        int id;
        if((id = fork()) == 0){

            single_command(pch);
            exit(-1);
        }
        waitpid(id,NULL,0);
        if (strcmp(pch,"exit")==0){
            exit(0);
        }
    }
//    printf("%s\n",getenv("PATH"));
//    printf("of command: %zu .",strlen(command));
//    char temp[200] ;
//    strcpy(temp,command);
//    char *args[]={"/bin/ls",NULL};
//    printf("%d\n",execv(args[0],args));
//
//    printf("Hello, World!\n");
    return 0;
}
void single_command(char *command){

    char * pch = malloc(100);
    strcpy(pch,command);
    char *args[100];
    char *allargs[100];

    int cnt=0;
    pch = strtok(pch, " ");

    while (pch != NULL){
        //possible error
        //if(!handle_redirection(pch)){
        allargs[cnt++] = pch;

        // cat file > file2
        // ghghj cfile1 > file2
        pch = strtok(NULL, " ");

    }
    int cnt1 = 0;

    for(int i = 0; i < cnt; i++){
        //possible error
        if(!handle_redirection(allargs[i])) {
            args[cnt1++] = allargs[i];
        }
    }

//    char *args[100];
//    char delim[] = " ";
//    char command2[200];
//    strcpy(command2,command);
//
//    char *ptr;
//    ptr = strtok(command2, delim);
//    while(ptr != NULL)
//    {
//        args[cnt++] = ptr;
//        ptr = strtok(NULL, delim);
//    }
//    char *args2[100] = {"/usr/bin/whereis",args[0],NULL};
    char type1[200]="";
    strcat(type1, "/usr/bin/");
    strcat(type1, args[0]);

    char type2[200]="";
    strcat(type2, "/bin/");
    strcat(type2, args[0]);

//    printf("%s\n%s",type1,type2);

//    for (int i = 0; i < cnt; ++i) {
//        printf("%s\n",args[i]);
//    }
    args[cnt1] = NULL;

    execv(type1,args);
    execv(type2,args);

}

bool handle_redirection(char* sptr2) {
    char *sptr = malloc(100);

    strcpy(sptr,sptr2);

    bool b_lt = contains_character(sptr,'<');
    bool b_gt = contains_character(sptr,'>');

    if(!(b_lt||b_gt)){
        return 0;
    }
    char *firstptr = malloc(100),*secondptr = malloc(100);
    if(*sptr == '>' || *sptr == '<'){
        strcpy(firstptr,"");
        sptr++;
        strcpy(secondptr,sptr);
    }
    else{
        sptr = strtok(sptr,"><");
        strcpy(firstptr,sptr);
        sptr = strtok(NULL,"");
        strcpy(secondptr,sptr);
    }


    if(b_gt){
        /*
         * Implements redirections
         *
         */
        if (strcmp(firstptr,"")==0){
            close(1);
        }
        else{
            close(atoi(firstptr));
        }
        int fdno=0;
        if(*secondptr == '>'){
            //append to the file
//            fopen(secondptr,"a");
//            secondptr = strtok(NULL,"");
//            char strarr[100];
//            for (int i = 1; secondptr[i] != '\0'; i++) {
//                strarr[i-1] = secondptr[i];
//            }
            secondptr++;
            open(secondptr,O_CREAT|O_RDWR|O_APPEND,0666);

        } else{
//            creat(secondptr,0666);
//            fdopen(secondptr,"w");
            if (*secondptr == '&'){
                dup(*(++secondptr)-'0');
            } else{
                open(secondptr,O_CREAT|O_RDWR|O_TRUNC,0666);
            }
        }
    }
    if (b_lt){
        close(0);
        open(secondptr,O_RDONLY,0666);
    }
    return 1;
}

bool contains_character(char *command, char tocheck){
    for (int i = 0; i < strlen(command); ++i) {
//        printf("elem is: %c\n",command[i]);
        if (command[i] == tocheck){
            return 1;
        }
    }
    return 0;
}