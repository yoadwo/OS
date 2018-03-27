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
vector<char*> parseLine(string line, char delimiter){
    // int n_spaces = count(line.begin(), line.end(), ' ');
    vector<string> tokens;
    string token;
    istringstream tokenStream(line);
    vector<char*> commandVector;

    while (getline(tokenStream, token, delimiter))
    {
        //tokens.push_back(token);
        commandVector.push_back(const_cast<char*>(token.c_str()));
    }
    //added later, to make it execvp-compatible
    commandVector.push_back(nullptr);

    // charVec is one element bigger than tokens, to include 'nullptr'
    //vector<const char*> charVec(tokens.size()+1,nullptr);
    vector<const char*> charVec(commandVector.size()+1,nullptr);
    /* for (size_t i=0; i<tokens.size();i++) {
        charVec[i]= tokens[i].c_str();
    } */
    for (size_t i=0; i<commandVector.size();i++) {
        charVec[i]= commandVector[i];
    }

    
    // execvp (commandVector[0], &commandVector[0]);
    execvp (charVec[0], charVec.data());
    return commandVector;
    
    //return tokens;
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
function expand tilde expands cwd and replace home directory with tilde
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
function expand tilde expands cwd and replace tilde with home directory
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
bool changeDir(vector<char*> res){ 
    if (res.size() > 1){
        //if (chdir(res[1].c_str())==0)
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

void execute(vector <char*> argv)
{
    pid_t pid;
    int status;
     
    /* fork a child process           */
    if ((pid = fork()) < 0) {     
        cout << "*** ERROR: forking child process failed\n" << endl;
        //exit(1);
     }
    /* for the child process:         */
    else if (pid == 0) {          
        if (execvp(argv[0], &argv[0]) < 0) {     /* execute the command  */
            cout << "*** ERROR: exec failed\n" << endl;
            exit(1);
          }
     }
    /* for the parent:      */
     else {                                  
          while (wait(&status) != pid)       /* wait for completion  */
               ;
     }
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
        //else if (!res[0].compare("exit"))
        else if (!strncmp(res[0],"exit",strlen("exit"))){
            std::cout << "C ya!\n";
            break;
        }
        else{
            //    execute(res);
            cout << res[0] <<": command not found.\n";

        }
        

        printPrompt();

    }

    handleIOErrors();
    

    std::cout << "Press any key to continue...\n";
    getchar();
    return 0;
}