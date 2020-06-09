#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <vector>
#include <string.h>
#include <unistd.h>

using namespace std;

string trim (string input){
    int i = 0;

    while(i < input.size() && input[i] == ' '){
        i += 1;
    }
    if(i < input.size()){
        input = input.substr(i);
    } else {
        return "";
    }

    int i = input.size() - 1;
    while(i >= 0 && input[i] == ' '){
        i -= 1;
    }
    if(i >= 0){
        input = input.substr(0, i + 1);
    } else {
        return "";
    }

    return input;

}

vector<string> split (string line, string sep=" "){
    vector<string> result_args;

    while(line.size()){
                    
    }
    return result_args;
}

int main (){
    while (true){
        cout << "My Shell$ ";
        string inputline;
        getline (cin, inputline);   // get a line from standard input
        if (inputline == string("exit")){
            cout << "Bye!! End of shell" << endl;
            break;
        }
        int pid = fork ();
        if (pid == 0){ //child process
            // preparing the input command for execution
            char* args [] = {(char *) inputline.c_str(), NULL};  
            execvp (args [0], args);
        }else{
            waitpid (pid, 0, 0); // wait for the child process 
            // we will discuss why waitpid() is preferred over wait()
        }
    }
}