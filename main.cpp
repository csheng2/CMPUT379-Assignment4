#include "main.h"

struct Resource {
    std::string name;
    int value;
};

struct Resource* LIST_OF_RESOURCES;

void initResources(){
}

void initJob(){

}

void splitString(std::string s, std::string delimiter){
    

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
    }
    std::cout << s << std::endl;
}

int processInputFile(std::string file_name){
    std::fstream newfile;

    //open the .dat file
    newfile.open(file_name, std::ios::in); 
    if (!newfile.is_open()){
        std::cout << "processInputFile() - Failed to open file " << file_name << std::endl;
        exit(0);
    }

    std::string line;
    size_t pos;

    while(getline(newfile, line)){

        pos = s.find(delimiter)

        // only process lines starting with p because that contains packet info
        if(strstr(line.c_str(), "resources") != NULL){
            initResources();
        } else if(strstr(line.c_str(), "job") != NULL){
            initJob();
        }
    }

    newfile.close();
    return -2;
}

int main() {
    
}