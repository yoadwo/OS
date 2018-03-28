#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>  
#include <regex>
#include <cstdlib>  
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>


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

/* 
function expandTildePromt: expands '~' in prompt and replace '/home/<user>' with '~'
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

/* 
function expandTildeInput: expands '~' into 'home/<user>'
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
        // regex_search returns a match $VAR into variable "m"
        tilde = m[0];
        // transform $VAR into VAR and lookup in env
        user_home = getenv("HOME");
        prefi = m.prefix().str();
        suffi = m.suffix().str();
        // s_temp = <old string prefix> + <$VAR value> + <old string suffix>
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
        std:: cout << expandTildePrompt(cwd) << " >";
}

/* function changeDir: change current working directory
    input: array (vector:: res) that hold paramters to "cd" command
    prompt user if input is missing parameters
*/
bool changeDir(vector<char *> res){ 
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

int execute(vector<char *> argv)
{
    pid_t pid;
    int status, lastExitStatus, errsv;

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
            perror("execvp() failed");
            exit(127);
        }
    }
    // for the parent:     
    else
    { 
        //while (wait(&status) != pid) ;      /* wait for completion  */
        if (waitpid(pid, &status, 0) != -1){
            if ( WIFEXITED(status) ) {
                lastExitStatus = WEXITSTATUS(status);
                printf("Exited normally with status %d\n", lastExitStatus);
            }
            else if ( WIFSIGNALED(status) ) {
                lastExitStatus = 128 + WTERMSIG(status);
                printf("Exited due to receiving signal %d\n", lastExitStatus);
            }
            else {
                printf("Something strange just happened.\n");
            }
        }
        else {
            perror("waitpid() failed");
            //exit(EXIT_FAILURE);
            lastExitStatus = EXIT_FAILURE;
        }
    }

    return lastExitStatus;
}

int main(){

    std::cout <<"Welcome to OS SHell\n";
    
    int linelen, exitStatus = 0;
    string line;
    vector<char*> res;

    printPrompt();    
    
    while (getline (cin,line))
    {
        linelen = line.length();
        //if only "return" was pressed
        if (linelen == 0){
            printPrompt();
            continue;
        }
        
        line=expandTildeInput(line);
        line = expandEnv(line);
        line = expandStatus(line, exitStatus);
        res = parseLine(line, ' ');

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
            std::cout << "C ya!\n";
            break;
        }
        else{
            exitStatus = execute(res);
        }
        
        printPrompt();

    }

    handleIOErrors();
    

    std::cout << "Press any key to continue...\n";
    getchar();
    return 0;
}