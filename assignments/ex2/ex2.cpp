#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(){
    
    printf("Welcome to OS SHell\n");
    char cwd[1024];
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while(1){
        printf("OS SHell: ");
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("??");
        else
            printf("%s> ", cwd);
        
        if ((nread = getline(&line, &len, stdin)) == -1) 
            continue;
        //if only "return" was pressed
        if (len == 1)
            continue;

        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "exit")){
            printf("C ya!\n");
            break;
        }
        else
            printf ("%s: command not found\n", line);
           
        
    }

    printf("Press any key to continue...\n");
    getchar();
    return 0;
    

}