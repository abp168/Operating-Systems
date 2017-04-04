#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <errno.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/wait.h>

#include <signal.h>

/* T. Ritter 10/6/2014



   This is a more useful simple shell -- accepts multi-word commands  'ls -l /usr' or 'cp f1.c f2.c'

   uses strtok with.



*/





char *prompt="%";
/* builtin function to change directory to home directory path after
 * cd command inserted to shell. Utilizes getcwd() in order to verify
 * the directory successfully changed to home directory.*/
FILE *fp, *temp;
int count =0;
char *logArray[10];
int cd(char **args)
{
    int ret;
    char cwd[1024];
    if(args[0] != NULL && args[1]==NULL )
    {
        ret=chdir(getenv("HOME")); // if chdir returns 0, chidr successfully changed
        args[0]= NULL; // replaces cd command with NULL, successfully deleting command from execution in child
        getcwd(cwd, sizeof(cwd)); //gets current working directory
        printf("Return value: %d CWD: %s\n",ret,cwd);  //reassurance that chdir() was successful
    }
    if(args[0] != NULL && args[1] !=NULL)
    {
        if(chdir(args[1]) == 0)
        {
            args[0]= NULL; // replaces cd command with NULL, successfully deleting command from execution in child
            args[1] = NULL;// replaces filename command with NULL, successfully deleting command from execution in child
            getcwd(cwd, sizeof(cwd)); //gets current working directory
            printf("CWD: %s\n",cwd); //reassurance that chdir() was successful
        }
        else
        {
            fprintf(stderr, "%s is not a directory\n",args[1]); //error checking if argument is not a directory
        }
    }


    return 0; //return value
}

void execpipe(char **args,char **args2,char line[])
   {
       int child_pid;
       #define BUF_SIZE 1024
       char buf[BUF_SIZE];
       int num_read;
       int pfd[2];
       if(pipe(pfd) == -1)
       {
           fprintf(stderr,"Pipe Creation Error\n");
       }
       switch(child_pid=fork())
       {
           case 0:
               printf("Entered child\n");
               if(close(pfd[1])==-1)
               {
                   fprintf(stderr,"Close Error of write end of pipe in Child\n");
               }
                if(dup2(pfd[0],0) < 0)
                {
                    perror("dup2");
                }
               printf("STDIN redirected to read pipe\n");
               if(close(pfd[0]) == -1)
               {
                   fprintf(stderr, "Close Error of read ond of pipe in Parent\n");
               }

               execvp(args2[0], args2); /* child */
               fprintf(stderr, "ERROR %s no such program \n", line);
               break;
           case -1: fprintf(stderr,"ERROR can't create child process!\n");   /* unlikely but possible if hit a limit */

               break;
           default:
                printf("Entered parent\n");
               if(close(pfd[0]) == -1)
               {
                   fprintf(stderr, "Close Error of read ond of pipe in Parent\n");
               }
               if(dup2(pfd[1],1) < 0)
               {
                   perror("dup2");
               }
               printf("STDOUT redirected to write pipe\n");

               if(close(pfd[1])== -1)
               {
                   fprintf(stderr, "Close Error of write end of pipe in Parent\n");
               }
               execvp(args[0], args);
               fprintf(stderr, "ERROR %s no such program \n", line);
               while((child_pid=wait(NULL)) != -1)
               {
                   continue;
               }

                break;






       }


   }
void sigHandler(int signo) //http://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code
{
    if(signo == SIGUSR1)
    {
        printf("Received SIGUSR1\n");
        fp= fopen("audit.log","a+");



    }
    if(signo == SIGINT)
    {
        if(kill(getpid(),SIGCONT) != 0)  //sends SIGCONT signal to maintain shell
        {
            fprintf(stderr,"Error in sending SIGCONT signal\n");
        }
    }
}

int main()

{

  int pid;

  int child_pid;

  char line[81];

  char *token;

  char *separator = " \t\n";

  char **args;
    char **args2;

  int i;
    int cdFlag; //flag to signal change directory call
    int outFlag; //flag to signal output redirection
    int inFlag; //flag to signal input redirection
    int appendFlag; //flag to signal append output redirection
    int pipeFlag;
    int daemonFlag;
    int fileOut,fileIn,file1,file2; //return variable for file open
    int filecnt = 1;
    int clearCountFlag =0;
    int deleteLineFlag =0;

  char *built_in[]={"cd"};  //builtin command array
  char *fd,*fd1,*fd2; //variable to store file name
   char c;



    /* max 80 tokens in line */

    args=(char **) malloc(80*sizeof(char *));



    fprintf(stderr,"%s",prompt);

    while ( fgets(line,80,stdin) != NULL ) {

        logArray[count] = line;

        if(fp != NULL)
        {
            if(clearCountFlag)
            {
                count=0;
                clearCountFlag=0;
            }
            /*if(deleteLineFlag)
            {

                fp= fopen("audit.log","r");  //http://www.w3schools.in/c-tutorial/program/delete-a-specific-line-from-a-text-file/
                c = getc(fp);
                while(c != EOF)
                {
                    c = getc(fp);
                }
                rewind(fp);
                temp= fopen("auditCopy.log","w");
                c= getc(fp);
                while(c != EOF)
                {
                    c= getc(fp);
                    if ( c == '\n')
                        filecnt++;
                    if(filecnt != 1)
                    {
                        putc(c,temp);
                    }
                }
                fclose(fp);
                fclose(temp);
                remove("audit.log");
                rename("auditCopy.log","audit.log");
            }*/
            if(count < 10)
            {
                fp = fopen("audit.log", "a+");
                fprintf(fp, "%s", logArray[count]);
                fclose(fp);
            }

            if(count >= 10)
            {
                fp= fopen("audit.log","r");  //http://www.w3schools.in/c-tutorial/program/delete-a-specific-line-from-a-text-file/
                c = getc(fp);
                while(c != EOF)
                {
                 c = getc(fp);
                }
                rewind(fp);
                temp= fopen("auditCopy.log","w");
                c= getc(fp);
                while(c != EOF)
                {
                    c= getc(fp);
                    if ( c == '\n')
                        filecnt++;
                    if(filecnt != 1)
                    {
                        putc(c,temp);
                    }
                }
                fclose(fp);
                fclose(temp);
                remove("audit.log");
                rename("auditCopy.log","audit.log");
                fp = fopen("audit.log", "a+");
                fprintf(fp, "%s", line);
                fclose(fp);
                clearCountFlag=1;
                deleteLineFlag =1;
            }
            count++;

        }

   /* get the first token */

   token = strtok(line, separator);



   i=0;

   args[i++]=token;  /* build command array */
        outFlag = 0;
        inFlag=0;
        appendFlag=0;
        pipeFlag=0;
        daemonFlag=0;
        cdFlag=0;
        threadcpFlag=0;
   while( token != NULL ) /* walk through other tokens */

   {

       /*printf( "%s\n", token ); */

       /* Conditional statement identifies output redirection
        * sets outputFlag and deletes > from command array.
        * strcmp() compares two constant strings and returns 0 a
        * if varibales match. strtok(NULL, separator) looks for NULL terminator;
        * thereby separating commands to eventually put into command array args[].*/
       if(strcmp(token,">")== 0)
       {
           outFlag = 1;
           args[i-1]=NULL;
           fd = strtok(NULL, separator); //
       }
       if(strcmp(token,"<")==0)
       {
           inFlag = 1;
           args[i-1]=NULL;
           fd = strtok(NULL, separator);
       }
       if(strcmp(token, ">>")== 0)
       {
           appendFlag = 1;
           args[i-1]=NULL;
           fd1= args[i-2];
           fd2 = strtok(NULL, separator);

       }
       if(strcmp(token,"|") == 0)
       {
           pipeFlag =1;
           args[i-1] = NULL;
            args2=&args[i];
       }
       if(strcmp(token, "&") == 0)
       {
           daemonFlag= 1;
           args[i-1] = NULL;
       }
       if(strcmp(token,built_in[0])==0)
       {
           cdFlag=1;
       }

       token = strtok(NULL, separator);
       args[i++]=token;   /* and build command array */

   }


    args[i]=(char *) NULL; //terminates command array arg[] with NULL

        //fprintf(stderr,"pid=%d\n",getpid()); uncomment in order to test USR1 signal

        if(cdFlag)
        {
            cd(args);
            cdFlag =0;
        }

    /* got all tokes in the argument vector ready to fork */

        /* compares commands array to builtin command array.
         * Sets change directory flag if command matches cd
         * command in builtin array*/


        if(signal(SIGUSR1,sigHandler) == SIG_ERR)
        {
            fprintf(stderr,"Error in handling USR1\n");
        }
        if(signal(SIGINT,sigHandler) == SIG_ERR)
        {
            fprintf(stderr,"Error in handling SIGINT\n");
        }







    switch(pid=fork())

    {

      case 0:

          if(pipeFlag)
          {
              pipeFlag=0;
              execpipe(args,args2,line);
              exit(0);
          }
        if(outFlag)  //checks if output redirection is present in shell
        {
            fileOut = open(fd, O_WRONLY | O_CREAT, 0644); //opens file given by shell insert from user
            if (dup2(fileOut, 1) < 0) //directs STDOUT (numeric value of 1) to file
            {
                perror("dup2"); //error check
            }
            close(fileOut);  //closes file
            outFlag = 0;
        }
        if(inFlag) //checks if input redirection is present in shell
        {
            fileIn = open(fd, O_RDONLY); //opens file given by shell insert from user
            if (dup2(fileIn, 0) < 0) //directs STDOUT (numeric value of 1) to file
            {
                perror("dup2"); //error check
            }
            if(appendFlag)
            {
                fd1=fd;
            }
            close(fileIn);  //closes file
            inFlag = 0;
        }
        if(appendFlag)
        {

            file1 = open(fd1,O_RDONLY);
            file2 = open(fd2, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (dup2(file2, 1) < 0) //directs STDOUT (numeric value of 1) to file
            {
                perror("dup2"); //error check
            }
            close(file1);
            close(file2);

            if(args[0] == fd1)
            {
                args[0]= NULL;
            }
            appendFlag = 0;

        }
            if(daemonFlag)
            {
                execvp(args[0], args); /* child */
                fprintf(stderr, "ERROR %s no such program \n", line);
                if(daemon(0,0) == -1)
                {
                    fprintf(stderr,"Error in daemon\n");
                }
            }
        execvp(args[0], args); /* child */
        fprintf(stderr, "ERROR %s no such program \n", line);
        exit(1);
        break;


      case -1: fprintf(stderr,"ERROR can't create child process!\n");   /* unlikely but possible if hit a limit */

               break;

      default:
          while((child_pid=wait(NULL)) != -1)
          {
              continue;
          }



     }

    fprintf(stderr,"%s",prompt);

    }


    exit(0);

} /* end main */
