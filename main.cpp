#include "main.h"

struct Resource {
    std::string name;
    int max_value;
    int in_use = 0;
};

struct Job {
    std::string job_name;
    int busy_time;
    int idle_time;
    int resources_length = 0;
    Resource* resources = new Resource[resources_length];
};

int RESOURCES_LENGTH = 0;
struct Resource* RESOURCES = new Resource[RESOURCES_LENGTH];
int JOBS_LENGTH = 0;
struct Job* JOBS = new Job[JOBS_LENGTH];

void growGlobalJobsList(){
    Resource* temp = new Resource[RESOURCES_LENGTH + 1];
    std::copy(RESOURCES, RESOURCES + RESOURCES_LENGTH, temp);
    delete [] RESOURCES;
    RESOURCES = temp;
    RESOURCES_LENGTH++;
}

void growJobResourcesList(Job job){
    Resource* temp = new Resource[job.resources_length + 1];
    std::copy(job.resources, job.resources + job.resources_length, temp);
    delete [] job.resources;
    job.resources = temp;
    job.resources_length++;
}

void growGlobalResourcesList(){
    Resource* temp = new Resource[RESOURCES_LENGTH + 1];
    std::copy(RESOURCES, RESOURCES + RESOURCES_LENGTH, temp);
    delete [] RESOURCES;
    RESOURCES = temp;
    RESOURCES_LENGTH++;
}

void printInitializedResources() {
    printf("==========================\n");
    printf("Initialized resources:");

    for (int x = 0; x < RESOURCES_LENGTH; x++){
        std::cout << "name: " << RESOURCES[x].name << " value: " << RESOURCES[x].max_value << std::endl;
    }

    printf("==========================\n");    
}

void initResources(std::string resource_string){
    std::string space_delimiter = " ";
    std::string colon_delimiter = ":";
    size_t pos = 0;
    std::string token;

    try{
        while ((pos = resource_string.find(space_delimiter)) != std::string::npos) {
            growGlobalResourcesList();

            token = resource_string.substr(0, pos);
            resource_string.erase(0, pos + space_delimiter.length());

            pos = token.find(colon_delimiter);;
            RESOURCES[RESOURCES_LENGTH - 1].name = token.substr(0, pos);
            token.erase(0, pos + colon_delimiter.length());
            RESOURCES[RESOURCES_LENGTH - 1].max_value = stoi(token.substr(0, pos));
        }
        
        // last resource:
        growGlobalResourcesList();
        token = resource_string;
        pos = token.find(colon_delimiter);;
        RESOURCES[RESOURCES_LENGTH - 1].name = token.substr(0, pos);
        token.erase(0, pos + colon_delimiter.length());
        RESOURCES[RESOURCES_LENGTH - 1].max_value = stoi(token.substr(0, pos));

        printInitializedResources();
    } catch(const std::exception& e) {
        std::cout << "initResources - Error: " << &e << std::endl;
        exit(0);
    }
}

void initJob(std::string job_string){
    growGlobalJobsList();
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

    while(getline(newfile, line)){
        if(line[0] == '#'){
            continue;
        }

        // only process lines starting with p because that contains packet info
        if(strstr(line.c_str(), "resources") != NULL){
            // erase "resources " from the line before passing it to the function
            std::string keyword = "resources ";
            line.erase(0, keyword.length());
            initResources(line);

        } else if(strstr(line.c_str(), "job") != NULL){
            // erase "job " from the line before passing it to the function
            std::string keyword = "job ";
            line.erase(0, keyword.length());            
            initJob(line);
        }
    }

    newfile.close();
    return -2;
}

int main() {
    processInputFile("example.txt");
}