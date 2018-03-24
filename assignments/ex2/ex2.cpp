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


using namespace std;

/*  function ParseLine: parse string by delimiter
    input: line to parse (string::line) and delimiter to parse by (char::delimiter)
*/
vector<string> parseLine(string line, char delimiter){
    // int n_spaces = count(line.begin(), line.end(), ' ');
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
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
        std:: cout << cwd << " >";
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

/* function changeDir: change current working directory
    input: array (vector:: res) that hold paramters to "cd" command
    prompt user if input is missing parameters
*/
bool changeDir(vector<string> res){ 
    if (res.size() > 1){
        if (chdir(res[1].c_str())==0)
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

int main(){

    std::cout <<"Welcome to OS SHell\n";
    
    int linelen, exitStatus = 0;
    string line;
    vector<string> res;

    printPrompt();    
    
    while (getline (cin,line))
    {
        linelen = line.length();
        //if only "return" was pressed
        if (linelen == 0){
            printPrompt();
            continue;
        }
        line = expandEnv(line);
        line = expandStatus(line, exitStatus);
        res = parseLine(line, ' ');

        if (res.empty()){

        }
        else if (!res[0].compare("cd")){
             if (!changeDir(res))
                exitStatus = 1;
        }
        else if (!res[0].compare("exit"))
        {
            std::cout << "C ya!\n";
            break;
        }
        else
            std::cout << res[0] <<": command not found\n";

        printPrompt();

    }

    handleIOErrors();
    

    std::cout << "Press any key to continue...\n";
    getchar();
    return 0;
}