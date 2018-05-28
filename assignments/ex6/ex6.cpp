#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>  
#include <regex>
#include <cstdlib>  
#include <algorithm>

#define READ_END 0
#define WRITE_END 1
int SAVED_IN = dup(STDIN_FILENO);
int SAVED_OUT = dup(STDOUT_FILENO);
int FILENO;
int SAVED_NUM = -1;
pid_t lastProcess;

using namespace std;

/*  function ParseLine: parse string by delimiter
    input: line to parse (string::line) and delimiter to parse by (char::delimiter)
*/
vector<char *> parseLine(string line, char delimiter){
    // int n_spaces = count(line.begin(), line.end(), ' ');
    vector<string> tokens;
    string token;
    istringstream tokenStream(line);
      
    vector<char*> cstrings;
    

    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
        
    }
    //tokens.push_back(nullptr);
    
    cstrings.reserve(tokens.size());

    for(size_t i = 0; i < tokens.size(); ++i)
        cstrings.push_back(const_cast<char*>(tokens[i].c_str()));
    cstrings.push_back(nullptr);
    
    return cstrings;
}

/* function handleIOErrors: handle getline() errors
    mainly used to catch End-Of-File (Ctrl+D)
*/
void handleIOErrors(){
    if (std::cin.bad()){
        // IO error
    }
    else if (!std::cin.eof()){
        // format error (not possible with getline but possible with operator>>)
    }
    else
    {
        std::cout << "C ya!\n";   
    }
}

/* function expandEnv: replace environment variable $VAR with actual value
    input: user command (string: text) that may contain reference to environment variables
    will search for $VAR using regular expression until no more matches found
    for each match, reconstruct string with new $VAR value
*/
string expandEnv(string text){ 
    static const regex env_re{R"(\$[_a-zA-Z][_a-zA-Z0-9]*)"};
    char *env;
    string s = text, var_name,s_temp, prefi, suffi;
    smatch m; // <-- regular expression match object

    while (regex_search(s, m, env_re)) // <-- use it here to get the match
    {
        // regex_search returns a match $VAR into variable "m"
        var_name = m[0];
        // transform $VAR into VAR and lookup in env
        env = getenv(var_name.substr(1).c_str());
        prefi = m.prefix().str();
        suffi = m.suffix().str();
        // s_temp = <old string prefix> + <$VAR value> + <old string suffix>
        s_temp = prefi;
        // 
        if (env){
            s_temp.append(env);
        }
        else {
            /* 
            uncomment the following line if buggy
            when env is not appended, we get a double-space
            <prefix>+" " + " " + <suffix>
            so perhaps is better to trim one space 
            */

            //suffi = suffi.substr(1);
        }
        s_temp.append(suffi);
        // push back into s
        s = s_temp;
    }
    
    return s;
}

/* function expandTildePromt: expands '~' in prompt and replace '/home/<user>' with '~'
    input: path (string:text) of current directory
    will search for '~' using regular expression and replace with $HOME contents
*/
string expandTildePrompt(string text){ 
    static const regex env_re{R"(\/home\/[_a-zA-Z][_a-zA-Z0-9]*\/)"};
    string s = text, user_home,s_temp, prefi, suffi;
    smatch m; // <-- regular expression match object

    while (regex_search(s, m, env_re)) // <-- use it here to get the match
    {
        // regex_search returns a match $VAR into variable "m"
        user_home = m[0];
        suffi = m.suffix().str();
        // s_temp = <old string prefix> + <$VAR value> + <old string suffix>
        s_temp ="~/";
        // 
        s_temp.append(suffi);
       
        // push back into s
        s = s_temp;
    }
    
    return s;
}

/*  function expandTildeInput: expands '~' into 'home/<user>'
    input: user command (string: text) that may contain '~'
    will search for '~' using regular expression and replace with $HOME contents
*/
string expandTildeInput(string text){ 
    static const regex env_re{R"(\~)"};
    char *user_home;
    string s = text, tilde,s_temp, prefi, suffi;
    smatch m; // <-- regular expression match object

    while (regex_search(s, m, env_re)) // <-- use it here to get the match
    {
        // regex_search returns a match "~"" into variable "m"
        tilde = m[0];
        // transform ~ into full path and lookup in env
        user_home = getenv("HOME");
        prefi = m.prefix().str();
        suffi = m.suffix().str();
        // s_temp = /home/<user>/ + <old string suffix>
        s_temp = prefi;
        
        s_temp.append(user_home);

        s_temp.append(suffi);
        // push back into s
        s = s_temp;
    }
    
    return s;
}

string expandStatus(string text, int exitStatus){
    static const regex env_re{R"(\$\?)"};
    string s = text, var,s_temp, prefi, suffi;
    smatch m; // <-- regular expression match object

    while (regex_search(s, m, env_re)) // <-- use it here to get the match
    {
        // regex_search returns a match '$?' into variable "m"
        var = m[0];
        prefi = m.prefix().str();
        suffi = m.suffix().str();
        // s_temp = <old string prefix> + <exit status value> + <old string suffix>
        s_temp = prefi;
        // 
        if (m.ready()){
            s_temp.append(to_string(exitStatus));
        }
        else {
            /* 
            uncomment the following line if buggy
            when env is not appended, we get a double-space
            <prefix>+" " + " " + <suffix>
            so perhaps is better to trim one space 
            */

            //suffi = suffi.substr(1);
        }
        s_temp.append(suffi);
        // push back into s
        s = s_temp;
    }
    
    return s;
}


/* function printPrompt: show shell messages
    OSHell: <path> $
*/
void printPrompt(){
    char cwd[1024];
    std::cout << "OS SHell: "; 
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        perror("??");
    else
        std:: cout << expandTildePrompt(cwd) << "> ";
}

/* function changeDir: change current working directory
    input: array (vector:: res) that hold paramters to "cd" command
    prompt user if input is missing parameters
*/
bool changeDir(vector<char*> res){ 
    if (res.size() > 1){
        // if (chdir(res[1].c_str())==0)
        if (chdir(res[1])==0)
            return true;
        else{
            cout << "cd: No such file or directory\n";
            return false;
        }
    }
    // if (res.size() == 1)
    else{
        cout << "cd: Usage: cd [dir]\n";
        return false;
    }
}

/*
    function findIndex: return index of str or -1 if does not exist
*/
int findIndex(vector <char*> res, char const *str){
    int index = -1;
    // skip last element, is always NULL
    for (size_t i =0; i< res.size()-1; i++)
        if (!strncmp(res[i],str,strlen(str)))
            index = i;
    return index;
}

/*
    function parseRedirect: manage redirections in command
    command extracts "<", ">", points file descriptors accordingly using dupe2
    then pop redirect and its argument
    i.e. echo hello > f1.txt ==> echo hello
*/
vector <char*> parseRedirect(vector <char*> args){
    vector<char*> res, deep_args;
    char *arg;
    int fd;
    string s_temp, prefi, suffi;
    smatch m; // <-- regular expression match object

    // for some cpp-ish reason, args would get deleted after regex c-tor
    // we deep copy it into deep_args
    for (size_t i=0; i< args.size(); i++){
        if (args[i] != NULL ){
            size_t len = strlen(args[i])+1;
            arg = new char [len]; // allocate for string and ending \0
            strncpy(arg,args[i], len);
            deep_args.push_back(arg);
        }
        else
            deep_args.push_back(NULL);
    }

    regex out_re{R"(-?[0-9]+[>])"}, in_re{R"(-?[0-9]+[<])"};    

    //while (regex_search(s, m, env_re)) // <-- use it here to get the match
    
    // regex_search returns a match $VAR into variable m
    // prefi = m.prefix().str();
    // suffi = m.suffix().str();
    // echo hello > f1.txt
    for (size_t i =0; i< deep_args.size() - 1; i++){

        string cmd(deep_args[i]);
        /* if ">" was found, redirect output of program
        * redirect left-hand side program's output to right-hand side programs's input
        * if "<" was found, redirect input of program
        * redirect left-hand side program's input to right-hand side programs's output
        *  */
        
        // i.e. cmd1 > cmd2
        if (!cmd.compare(">")){
            if (deep_args[i+1] == NULL){
                const char* err = "OS SHell: syntax error near unexpected token 'newline'\n";
                throw invalid_argument(err);
            }
            // close stdout, open file
            // don't forget to reopen stdoud! at parent @ executeNoPipe
            fd = open(deep_args[i+1], O_CREAT | O_APPEND | O_RDWR, 0666);
            dup2(fd,STDOUT_FILENO);
            close(fd);
            // increament i to skip ">" and following file
            i++;
        }
        // i.e. [number]>
        else if ( regex_search(cmd, m,out_re) ){
            if (deep_args[i+1] == NULL){
                const char* err = "OS SHell: syntax error near unexpected token 'newline'\n";
                throw invalid_argument(err);
            }
            // close given fd#, open file
            // don't forget to reopen stdoud! at parent @ executeNoPipe
            FILENO = stoi(m[0].str().substr(0, m[0].str().length()-1));
            if (FILENO < 0){
                s_temp.append(deep_args[0]);
                s_temp.append(": invalid input (negative file descriptor used\n");
                const char* err = s_temp.c_str();
                throw invalid_argument(err);
            }
            SAVED_NUM = dup (FILENO);
            fd = open(deep_args[i+1], O_CREAT | O_APPEND | O_RDWR, 0666);
            dup2(fd, FILENO);
            close(fd);
            // increament i to skip ">" and following file
            i++;
        }
        // i.e. cmd1 < cmd2
        else if (!cmd.compare("<")){
            if (deep_args[i+1] == NULL){
                const char* err = "OS SHell: syntax error near unexpected token 'newline'\n";
                throw invalid_argument(err);
            }
            fd = open(deep_args[i+1], O_RDONLY, 0444);
            if (fd == -1){
                s_temp.append("OS Shell: ");
                s_temp.append(deep_args[i+1]);
                s_temp.append(": ");
                s_temp.append(strerror(errno));
                s_temp.append("\n");
                const char* err = s_temp.c_str();
                throw invalid_argument(err);
            }
            dup2(fd,STDIN_FILENO);
            close(fd);
            // increament i to skip ">" and following file
            i++;
            
        }
        // i.e. [number]<
        else if ( regex_search(cmd, m,out_re) )  {
            cout << "[number]<";
            if (deep_args[i+1] == NULL){
                const char* err = "OS SHell: syntax error near unexpected token 'newline'\n";
                throw invalid_argument(err);
            }
            // close given fd#, open file
            // don't forget to reopen stdin! at parent @ executeNoPipe
            FILENO = stoi(m[0].str().substr(0, m[0].str().length()-1));
            if (FILENO < 0){
                s_temp.append(deep_args[0]);
                s_temp.append(": invalid input (negative file descriptor used\n");
                const char* err = s_temp.c_str();
                throw invalid_argument(err);
            }
            SAVED_NUM = dup (FILENO);
            fd = open(deep_args[i+1], O_RDONLY, 444);
            if (fd == -1){
                s_temp.append("OS Shell: ");
                s_temp.append(deep_args[i+1]);
                s_temp.append(": ");
                s_temp.append(strerror(errno));
                s_temp.append("\n");
                const char* err = s_temp.c_str();
                throw invalid_argument(err);
            }
            dup2(FILENO, fd);
            close(FILENO);
            // increament i to skip ">" and following file
            i++;
        }
        else {
            res.push_back(deep_args[i]);
        }
    }
    res.push_back(nullptr);
    return res;
}

int executeNoPipe(vector<char *> argv,int background)
{
    pid_t pid, child;
    int status, lastExitStatus;

    try {
        argv = parseRedirect(argv);
    } catch (const invalid_argument& ia){
        cerr << ia.what();
        return(EXIT_FAILURE);
    }

    // fork a child process
    pid = fork();

    // check if fork failed
    if (pid == -1){
        perror("*** ERROR: forking child process failed");
        exit(EXIT_FAILURE);
    }
    // for the child process:        
    else if (pid == 0){
        // execute the command       
        status = execvp(argv[0], &argv[0]);
        if ( status < 0){ 
            //cout << "*** ERROR: exec failed [" << argv[0] <<"]\n";
            perror("execvp() no pipe failed");
            exit(127);
        }
    }
    // for the parent:     
    else if (pid > 0){
        // have parent reopen stdout & stdin after exec
        dup2(SAVED_OUT, STDOUT_FILENO);
        dup2(SAVED_IN, STDIN_FILENO);
        if (SAVED_NUM != -1)
            dup2(SAVED_NUM, FILENO);
        if (background == 0)
        {
            //while (wait(&status) != pid) ;      /* wait for completion  */
            if (waitpid(pid, &status, 0) != -1)
            {
                if (WIFEXITED(status))
                {
                    lastExitStatus = WEXITSTATUS(status);
                    //printf("Exited normally with status %d\n", lastExitStatus);
                }
                else if (WIFSIGNALED(status))
                {
                    lastExitStatus = 128 + WTERMSIG(status);
                    printf("Exited due to receiving signal %d\n", lastExitStatus);
                }
                else
                {
                    printf("Something strange just happened.\n");
                }
            }
            else
            {
                perror("waitpid() failed");
                //exit(EXIT_FAILURE);
                lastExitStatus = EXIT_FAILURE;
            }
        }
        else if (background == 1)
            cout << "[" << pid << "]" << endl;
    }
    while ((child = waitpid(-1, &status, WNOHANG)) > 0){
        if (WIFSIGNALED(status))        {
            lastExitStatus = WTERMSIG(status) + 128;
        }
        cout << "[" << child << "] : exited, status = " << lastExitStatus << endl;
        
    }
    
    return lastExitStatus;
}

int executePipe(vector <char*> argv, int pipeIndex){
    int fds[2], status, lastExitStatus, bgIndex, bgFlag = 0;
    const char* bgDelimiter= "&";
    pid_t cpid1, cpid2, child;
    vector <char*> leftArg, rightArg;

    pipe(fds);
    while ((bgIndex = findIndex(argv, bgDelimiter)) != -1){
        bgFlag = 1;
        argv.erase (argv.begin()+bgIndex); 
        // update pipe index after erasing "&""
        if (bgIndex < pipeIndex)
            pipeIndex--;
        // remove "&" from argv, (bgIndex+1)-th item
    }

    for (int i=0; i < pipeIndex; i++)
        leftArg.push_back(argv[i]);
    leftArg.push_back(NULL);
    for (size_t i= pipeIndex +1; i < argv.size();i++)
        rightArg.push_back(argv[i]);

    //fork for 1st child (left to pipe)
    cpid1 = fork();
    
    if ( cpid1 == -1 ){
        perror("*** ERROR: forking child process failed");
        exit(EXIT_FAILURE);
    }
    if (cpid1 == 0){
        // stdout now points to pipe write, close unnecessary pipe read end
        dup2(fds[WRITE_END],STDOUT_FILENO);
        close (fds[READ_END]);

        lastExitStatus = execvp(leftArg[0], &leftArg[0]);
        if (lastExitStatus == -1){
            perror("ERROR: left child execvp failed");
            exit(EXIT_FAILURE);
        }
    } 
    // in parent
    //fork for 2nd child (right to pipe)
    cpid2 = fork();
    if ( cpid2 == 0 ){
         //waitpid(cpid1, &status, 0);
        // stdin now points to pipe read, close unnecessary pipe write end
        dup2(fds[READ_END],STDIN_FILENO);
        close(fds[WRITE_END]);
        lastExitStatus = execvp(rightArg[0], &rightArg[0]);
        if (lastExitStatus == -1){
            cerr << "ERROR: right child execvp failed";
            exit(EXIT_FAILURE);
        }
    }

    else if(cpid2>0){
        lastProcess=cpid2;
    }
     close(fds[READ_END]);
     close(fds[WRITE_END]);
    if (bgFlag == 0){
        waitpid(cpid1, NULL, 0);
        //while (wait(&status) != pid) ;      /* wait for completion  */
        if (waitpid(cpid2, &status, 0) != -1){
            if (WIFEXITED(status)){
                lastExitStatus = WEXITSTATUS(status);
                //printf("Exited normally with status %d\n", lastExitStatus);
            }
            else if (WIFSIGNALED(status)){
                lastExitStatus = 128 + WTERMSIG(status);
                cerr << "Exited due to receiving signal " << lastExitStatus << "\n";
            }
            else{
                cerr <<"Something strange just happened.\n";
            }
        }
        else{
            cerr << "waitpid() failed";
            //exit(EXIT_FAILURE);
            lastExitStatus = EXIT_FAILURE;
        }
    }
    else if (bgFlag == 1) {
      // cout << "[" << cpid1 << "]" << endl;
        cout << "[" << cpid2 << "]" << endl;
    }
    
    // while((child = waitpid(-1, &status, WNOHANG)) > 0)
    // {
    //     if(WIFEXITED(status))
    //     {
    //         lastExitStatus = WEXITSTATUS(status); // returns the exit status of the child
    //     }
    //     if (WIFSIGNALED(status))
    //     {
    //         lastExitStatus = WTERMSIG(status) + 128;
    //     }
    //     if(child == lastProcess)
    //     {
    //         cout << "[" << child << "] : exited, status = " << lastExitStatus << endl;
    //     }
        //lastExitStatus = 0;
    //}
   /* while ((child = waitpid(-1, &status, WNOHANG)) > 0){
        if (WIFSIGNALED(status)){
            lastExitStatus = WTERMSIG(status) + 128;
        }
        cout << "[" << child << "] : exited, status = " << lastExitStatus << endl;
    }*/
    // parent must wait for children
    // close both pipe ends for parent
    //close(fds[READ_END]);
    //close(fds[WRITE_END]);
    //waitpid(cpid1, &status, 0);
    //waitpid(cpid2, &status, 0);

    return lastExitStatus;
}


//zombie handler function
void handle_zombie(int sig) {
    pid_t pid;
    int status;
    int saved_errno = errno;
    while (1) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid <= 0) /* No more zombie children to reap. */
            break;
       
        cout<<"\n["<< pid<<"]: exited, status=" << status + 128 << "\n";
     }
    errno = saved_errno;
    //sleep(1);
} 

int main(){

    int linelen, exitStatus = 0, pipeIndex;
    char *background;
    string line;
    vector<char*> res;
    int lastExitStatus;
    int status;
    pid_t child;
    
    cout <<"Welcome to OS SHell\n";
    printPrompt();    
    
    while (getline (cin,line))
    {
        while((child = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if(WIFEXITED(status))
        {
            lastExitStatus = WEXITSTATUS(status); // returns the exit status of the child
        }
        if (WIFSIGNALED(status))
        {
            lastExitStatus = WTERMSIG(status) + 128;
        }
       if(child == lastProcess)
        {
            cout << "[" << child << "] : exited, status = " << lastExitStatus << endl;
        }
        //lastExitStatus = 0;
    }
        linelen = line.length();
        //if only "return" was pressed
        if (linelen == 0){
            printPrompt();
            continue;
        }
        
        line = expandTildeInput(line);
        line = expandEnv(line);
        line = expandStatus(line, exitStatus);
        res = parseLine(line, ' ');
        /* try {
        res = parseRedirect(res);
        } catch (const invalid_argument& ia){
            cerr << ia.what();
            return(EXIT_FAILURE);
        } */

        if (res.empty()){

        }
        //else if (!res[0].compare("cd")){
        else if (!strncmp(res[0],"cd",strlen("cd"))){
            if (changeDir(res))
                exitStatus = 0;
            else
                exitStatus = 1;
        }
        // else if (!res[0].compare("exit")){
        else if (!strncmp(res[0],"exit",strlen("exit"))){
            cout << "C ya!\n";
            break;
        }
        else{
            char const *delimiter = "|";
            pipeIndex = findIndex(res, delimiter);
            if (pipeIndex == -1)
            {
                // assumes "&" is on last index
                background = res.end()[-2];
                if (!strncmp(background, "&", strlen("&")))
                {
                    // take out "NULL" and "&", return "NULL"
                    res.pop_back();
                    res.pop_back();
                    res.push_back(NULL);
                    exitStatus = executeNoPipe(res, 1);
                }
                else
                {
                    exitStatus = executeNoPipe(res, 0);
                }
            }
            else{
                executePipe(res, pipeIndex);
            }
        }
        printPrompt();

    }

    handleIOErrors();
    

    std::cout << "Press any key to continue...\n";
    getchar();
    return 0;
}