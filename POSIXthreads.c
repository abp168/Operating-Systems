#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 16384  //maximum allotted number of bytes to transfer
int sum;  //global variable to record bytes copied. Also apart of critical section
char *dir; //global variable to record user desired directory

void *threadcp(void *args)  //thread copy function that will open file
{                           // read into buffer and write buffer into new file.
    char buf[BUF_SIZE];  //buffer arrary
    char pathFile[260];  //filename array
    char *fd;            // filename pointer
    int num_read;        // variable to record byte value from read()
    int fileIn,fileOut;  //file descriptors
    fd= args;
    sprintf(pathFile,"%s/copy",dir);  //appends directory name to '/copy'
    strcat(pathFile,fd);              // concatenates directory to filename passed in
    pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;  //creates mutex
    fileIn= open(fd,O_RDONLY);        //opens file for read
    fileOut = open(pathFile,O_WRONLY| O_CREAT, 0666);  //opens and creates file for write
    for(;;)    // Repeatedly reads from fileIn and writes to
    {          // fileOUt until all data in fileIn are read
        num_read = read(fileIn,buf,BUF_SIZE);
        if(num_read == -1)  //-1 indicates read error
        {
            fprintf(stderr,"Read Error!\n");
        }
        if(num_read == 0) /* end-of-file */
        {
            break;
        }

        if(write(fileOut,buf,num_read) != num_read) //makes sure data written equals the data number returned from read()
        {
            fprintf(stderr,"Write Error!\n");
        }
        pthread_mutex_lock(&mutex1); //locks critical section
        sum+=num_read;               //sums up bytes copied
        pthread_mutex_unlock(&mutex1); //unlocks critical section
    }
    close(fileIn);  //close files
    close(fileOut);
}
int main(int argc, char *argv[])
{
    int i,iret,maxThreadNum;
    //struct arg_struct file_args;
    char *fileName;
    maxThreadNum = 10;  //arbitrary value for max number of threads
    pthread_t * thread = malloc(sizeof(pthread_t)*maxThreadNum); //http://stackoverflow.com/questions/4964142/how-to-spawn-n-threads
    if(strcmp(argv[1],"threadcp")==0)
    {
        dir = argv[argc-1]; //assigns directory to last input from user
        for (int i = 1; i < argc -2; i++)  //iterates through argv and creates i number of threads
        {                                  //stops before directory name
            fileName = argv[i+1];
            iret = pthread_create(&thread[i], NULL, &threadcp, (void *) fileName);  //creates thread and calls threadcp()
            if (iret)  //pthread_create() returns non-zero number on error
            {
                fprintf(stderr, "Error - pthread_create() return code: %d\n", iret);
            }

        }
        for (int i = 1; i < argc - 2; i++)  //iterates through threads to join back to main process
        {
            pthread_join(thread[i], NULL);
        }
        printf("%d Bytes copied\n", sum);
        exit(EXIT_SUCCESS);

    }
    
}
