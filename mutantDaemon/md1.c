/*
* Mutant Daemon: md1.1
*
* Based on https://github.com/pasce/daemon-skeleton-linux-c
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

#define FileName "data.dat"
   
static void skeleton_daemon()
{
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
    openlog ("firstdaemon", LOG_PID, LOG_DAEMON);
}

//char *program_path()
char program_name()
{
    
    
    
    // First get program path which contains names
    //char *path = malloc(PATH_MAX);
    if (path != NULL) {
        if (readlink("/proc/self/exe", path, PATH_MAX) == -1) {
            //free(path);
            path = NULL;
        }
    }
    
    // Now strip off just the program name
    
    }
    
    
    
//    return path;
    return;
}

int main()
{
    int PATH_MAX = 256;
    char path[PATH_MAX];
    
    // Get my name (program name) without using argv
    //path = program_name();
    
    if (readlink("/proc/self/exe", path, PATH_MAX) == -1) {
        path = NULL;
        
    
    puts("Program name (path):\n");
    puts(path);
    
    skeleton_daemon();
    
    while (1)
    {
        //TODO: Insert daemon code here.
        syslog (LOG_NOTICE, "First daemon started.");
        
        sleep (20);
        break;
    }
   
    syslog (LOG_NOTICE, "First daemon terminated.");
    closelog();
    
    return EXIT_SUCCESS;
}
