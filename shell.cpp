#include <algorithm>
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#define GREEN "\e[0;32m"

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

    i = input.size() - 1;
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
        // quote handler for echo and other cmds
        if(line[0] == '"'){
            int qpos;
            for(int i = 1; i < line.length(); i++){
                if(line[i] == '"'){
                    qpos = i;
                    break;
                }
            }
            string str_trimmed = line.substr(1, qpos - 1);
            if(line[1] == '{'){
                str_trimmed.erase(remove(str_trimmed.begin(), str_trimmed.end(), ' '), str_trimmed.end());
            }
            str_trimmed = line.substr(qpos + 1);

        }
        int idx = line.find(sep);
        if(idx != string::npos){
            result_args.push_back(line.substr(0, idx));
            line = line.substr(idx + sep.size());
            if(line.size() == 0){
                result_args.push_back(line);
            }
        } else {
            result_args.push_back(line);
            line = "";
        }
    }

    return result_args;
}

char** vec_to_char_array(vector<string> parts){
    char** result_args = new char * [parts.size() + 1];
        for(int i = 0; i < parts.size(); i++){
            result_args[i] = new char [parts[i].size() + 1];
            strcpy(result_args[i], parts[i].c_str());
        }
    result_args[parts.size()] = NULL;
    return result_args;
}

void cmd_execute(string cmd){
    vector<string> arg_vec = split(cmd, " ");

    int fwrite;
    int fread;
    char temp_char1[FILENAME_MAX];
    char temp_char2[FILENAME_MAX];

    
    if(arg_vec[0] == "jobs"){
        // shows what processes are running on the system
        arg_vec[0] = "ps";
    }

    if(arg_vec[0] == "cd"){
        if(arg_vec[1] == "-"){
            getcwd(temp_char1, sizeof(temp_char1));
            //chdir(getenv("HOME"));
            chdir(temp_char2);
            strncpy(temp_char2, temp_char1, sizeof(temp_char1));
        } else {
            getcwd(temp_char2, sizeof(temp_char2));
            chdir(arg_vec[1].c_str());
        }
    }

    for(int i = 0; i < arg_vec.size(); i++){
        if(arg_vec[i] == ">"){
            fwrite = open(arg_vec[i + 1].c_str(), O_CREAT | O_WRONLY | S_IWUSR | S_IROTH | O_TRUNC, S_IRUSR) ;
            dup2(fwrite, 1);
            arg_vec.erase(arg_vec.begin()+i+1);
            arg_vec.erase(arg_vec.begin()+i);
        }
    }

    for(int i = 0; i < arg_vec.size(); i++){
        if(arg_vec[i] == "<"){
            fread = open(arg_vec[i + 1].c_str(), O_RDONLY, S_IRUSR |  S_IRGRP | S_IROTH | S_IWUSR );
            dup2(fread,0);
            arg_vec.erase(arg_vec.begin()+i);
        }
    }

    // running the arguments
    char** args = vec_to_char_array(arg_vec);
    execvp(args[0], args);
}


int main (){
    vector<int> bgs;

    while (true){
        for(int i = 0; i < bgs.size(); i++){
            if(waitpid(bgs[i], 0, WNOHANG) == bgs[i]){
                cout << "Process: " << bgs[i] << " ended" << endl;
                bgs.erase(bgs.begin() + i);
                i--;
            }
        }

        char loc[FILENAME_MAX];
        cout << "Jay's Custom Shell:" << GREEN << getcwd(loc, sizeof(loc)) << "$ ";
        string inputline;
        getline (cin, inputline);   // get a line from standard input
        if (inputline == string("exit")){
            cout << "Bye!! End of custom shell" << endl;
            break;
        }

        bool bg = false;
        inputline = trim(inputline);
        size_t posA = inputline.find("&");
        if (posA != string::npos){
            cout << "Bg process found " << endl;
            bg = true;
            inputline.erase(inputline.begin() + posA);
            inputline = trim(inputline);
            //inputline = inputline.substr(0, inputline.size() - 1);
        }
        if(inputline != ""){
            vector<string> cmds = split(inputline, "|");
            int pid = fork ();

            if (pid == 0){ //child process
                //vector<string> parts = split(inputline);
                //char** args = vec_to_char_array(parts);
                // preparing the input command for execution
                //char* args [] = {(char *) inputline.c_str(), NULL};  
                //execvp (args [0], args);
                cmd_execute(inputline);
            }else{
                if(!bg){
                    waitpid (pid, 0, 0); // wait for the child process 
                    // we will discuss why waitpid() is preferred over wait()
                } else {
                    bgs.push_back(pid);
                }
                
            }
        }
        
    }
}