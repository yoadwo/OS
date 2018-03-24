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


using namespace std;

/*  function ParseLine: parse string by delimiter
    input: line to parse (string::line) and delimiter to parse by (char::delimiter)
*/
vector<string> parseLine(string line, char delimiter){
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(line);
   while (std::getline(tokenStream, token, delimiter)){
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
        s_temp.append(env);
        s_temp.append(suffi);
        // push back into s
        s = s_temp;
    }
    
    return s;
}

int main(){

/*     std::cout << expandEnv("my $HOME is where the heart is\n "
    "but $PWD looks the same,\n"
    "but $USER rules it all\n"); */

    std::cout <<"Welcome to OS SHell\n";
    
    int linelen;
    std::string line;
    std::vector<std::string> res;

    printPrompt();    
    
    while (std::getline (std::cin,line))
    {
        
        linelen = line.length();
        //if only "return" was pressed
        if (linelen == 0){
            printPrompt();
            continue;
        }
        line = expandEnv(line);
        res = parseLine(line, ' ');

        if (!res[0].compare("cd")){
            chdir(res[1].c_str());
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