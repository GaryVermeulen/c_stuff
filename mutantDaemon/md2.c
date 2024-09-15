/*
* Mutant Daemon: md1.1
*
* Based on https://github.com/pasce/daemon-skeleton-linux-c
* Attempting to relicatelike a cell divides...
* Using simple proucer/consumer file IPC
*
*/
    
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include <string.h>
#include <fcntl.h>

#define FileName "data.dat"

char name[10];
   
static void skeleton_daemon(int myCount)
{
    printf("myCount: %d\n", myCount);
    
    pid_t pid;
    
    /* Fork off the parent process */
    pid = fork();
    
    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    
     /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    
    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);
    
    /* Catch, ignore and handle signals */
    /*TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    
    /* Fork off for the second time*/
    pid = fork();
    
    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    
    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    
    /* Set new file permissions */
    umask(0);
    
    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");
    
    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }
    
    /* Open the log file */
    openlog (name, LOG_PID, LOG_DAEMON);
}


void report_and_exit(const char* msg) {
    perror(msg);
    exit(-1); /* EXIT_FAILURE */
}


void reverse(char* str) {

    // Initialize first and last pointers
    int first = 0;
    int last = strlen(str) - 1;
    char temp;

    // Swap characters till first and last meet
    while (first < last) {
      
        // Swap characters
        temp = str[first];
        str[first] = str[last];
        str[last] = temp;

        // Move pointers towards each other
        first++;
        last--;
    }
}

int intToAscii(int number) {
   return '0' + number;
}


int main()
{
    int PATH_MAX = 256;
    int COPIES_MAX = 4;
    char copiesOfSelf[] = "4";
    char path[PATH_MAX];
    
    struct flock lock;
    lock.l_type = F_WRLCK;      /* read/write (exclusive) lock */
    lock.l_whence = SEEK_SET;   /* base for seek offsets */
    lock.l_start = 0;           /* 1st byte in file */
    lock.l_len = 0;             /* 0 here means 'until EOF' */
    lock.l_pid = getpid();      /* process id */
    
    int fd; /* file descriptor to identify a file within a process */
    
    // Get my name (program name) without using argv
    if (readlink("/proc/self/exe", path, PATH_MAX) == -1) {
        printf("readlink error\n");
    }
    
    // Now extract the program name from the path
    int len = strlen(path);
    for (int i = len; i > -1; i--) {
        if (path[i] == '/')
            break;
        strncat(name, &path[i], 1);
    }
    reverse(name);
    puts("Program name:");
    puts(name);
    
    // This is just for debugging
    pid_t mypid;
    mypid = getpid();
    printf("pid: %d\n", mypid); 
    
    
    // Hokey file handling...
    //if (access(fname, F_OK) == 0) {
    if (access(FileName, F_OK) == 0) {
        // file exists
        // consumer
        //int fd; /* file descriptor to identify a file within a process */
        if ((fd = open(FileName, O_RDONLY)) < 0)    /* -1 signals an error */
            report_and_exit("open to read failed...");  
        
        /* If the file is write-locked, we can't continue. */
        fcntl(fd, F_GETLK, &lock); /* sets lock.l_type to F_UNLCK if no write lock */
        if (lock.l_type != F_UNLCK)
            report_and_exit("file is still write locked...");
        
        lock.l_type = F_RDLCK; /* prevents any writing during the reading */
        if (fcntl(fd, F_SETLK, &lock) < 0)
            report_and_exit("can't get a read-only lock...");

        /* Read the bytes (they happen to be ASCII codes) one at a time. */
        int c; /* buffer for read bytes */
        puts("reading file...");
        while (read(fd, &c, 1) > 0) /* 0 signals EOF */
            write(STDOUT_FILENO, &c, 1); /* write one byte to the standard output */

        puts("\nread complete.");
        /* Release the lock explicitly. */
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLK, &lock) < 0)
            report_and_exit("explicit unlocking failed...");
    
        close(fd);

    } else {
        // file doesn't exist
        // producer
        //int fd; /* file descriptor to identify a file within a process */
        if ((fd = open(FileName, O_RDWR | O_CREAT, 0666)) < 0)  /* -1 signals an error */
            report_and_exit("open failed...");
        
        if (fcntl(fd, F_SETLK, &lock) < 0)  /** F_SETLK doesn't block, F_SETLKW does **/
            report_and_exit("fcntl failed to get lock...");
        else {
            write(fd, copiesOfSelf, strlen(name)); /* populate data file */
            fprintf(stderr, "Process %d has written to data file...\n", lock.l_pid);
        }
    
        /* Now release the lock explicitly. */
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLK, &lock) < 0)
            report_and_exit("explicit unlocking failed...");

        close(fd);  /* close the file: would unlock if needed */
    }
        
        
    int i = 1;
    printf("Before first call: i: %d\n", i);
    skeleton_daemon(i);
    
    
    while (1)
    {
        //TODO: Insert daemon code here.
        syslog (LOG_NOTICE, "First daemon started.");
        syslog (LOG_NOTICE, "%s", name);
        
        i = i + 1;
        
        skeleton_daemon(i);
        
        sleep (60);
        break;
    }
   
    syslog (LOG_NOTICE, "First daemon terminated.");
    closelog();
    
    return EXIT_SUCCESS;
}
