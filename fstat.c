//
// Created by aaron on 11/27/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{

    int i;
    if(argc < 2)
    {
        fprintf(stderr,"Not enough arguments\n");
    }
    for(i =1; i<argc; i++)
    {
        struct stat fileStat;
        int file=0;
        if((file = open(argv[i],O_RDONLY)) == -1)
        {
            fprintf(stderr,"file open error!");
        }

        if(fstat(file,&fileStat) < 0)
        {
            fprintf(stderr,"fstat() error!");
        }
        printf("File/Directory name: %s\n",argv[i]);
        switch(fileStat.st_mode & S_IFMT)
        {
            case S_IFDIR :
                printf("Directory\n");
                break;
            case S_IFREG:
                printf("Regular File\n");
                break;
            default:
                printf("Unrecognized file\n");
                break;
        }
        printf("Id of device containing file: %ld\n",fileStat.st_dev);
        printf("inode number: %ld\n",fileStat.st_ino);
        printf("Protection: \t");
        /*http://codewiki.wikidot.com/c:system-calls:fstat*/
        printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
        printf("\n\n");
        printf("Number of hard links: %ld\n",fileStat.st_nlink);
        printf("UID: %d GID: %d\n",fileStat.st_uid,fileStat.st_gid);
        printf("Total size: %ld bytes  Preferred I/0 block size: %ld bytes Blocks allocated: %ld\n",fileStat.st_size,fileStat.st_blksize,fileStat.st_blocks);
        printf("Last status change: %s\n",ctime(&fileStat.st_ctime));
        printf("Last file access: %s\n",ctime(&fileStat.st_atime));
        printf("Last file modification: %s\n", ctime(&fileStat.st_mtime));
        close(file);
    }

    exit(EXIT_SUCCESS);
}