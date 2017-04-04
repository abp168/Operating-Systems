#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

/* T. Ritter 10/6/2014

   This is a more useful simple shell -- accepts multi-word commands  'ls -l /usr' or 'cp f1.c f2.c'
   uses strtok with.

*/


char *prompt="%";

int cd(char **args)
{
	int ret;
	char cwd[1024];
	if(strcmp(args[0],"cd")==0)
	{
		ret=chdir(getenv("HOME"));
		getcwd(cwd, sizeof(cwd));
		printf("Return value: %d CWD: %s\n",ret,cwd);
	}
	return 0;
}

int main()
{
  int pid;
  int child_pid;
  char line[81];
  char *token;
  char *separator = " \t\n";
  char **args;
  int i;
  char *built_in[]={"cd"};
  int outFlag;
  int file;
    /* max 80 tokens in line */
    args=(char **) malloc(80*sizeof(char *));

    fprintf(stderr,"%s",prompt);
    while ( fgets(line,80,stdin) != NULL ) {

   /* get the first token */
   token = strtok(line, separator);

   i=0;
   args[i++]=token;  /* build command array */
   outFlag=0;
   while( token != NULL ) /* walk through other tokens */
   {
      /* printf( " %s\n", token ); */
      token = strtok(NULL, separator);
      
      if(strcmp(token,">") == 0)
      {
      	  printf("Recognized output\n");
	  outFlag = 1;
      }
      else
      {
	args[i++]=token;   /* and build command array */
      }
  
  }
    args[i]=(char *) NULL;

    /* got all tokes in the argument vector ready to fork */
    if(strcmp(args[0],built_in[0])==0)
    {
	cd(args);
    }
    switch(pid=fork())
    {
      case 0:  execvp(args[0],args);   /* child */
               fprintf(stderr,"ERROR %s no such program\n",line);
               exit(1);
               break;
      case -1: fprintf(stderr,"ERROR can't create child process!\n");   /* unlikely but possible if hit a limit */
               break;
      default: wait(NULL);   /* too simple to only do 1 wait - not correct */
     }
    fprintf(stderr,"%s",prompt);
    }
    exit(0);
} /* end main */

