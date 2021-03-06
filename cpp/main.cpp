#include "../headers/main.h"

struct Resource {
    std::string name;
    int value;
};

struct Job {
    std::string job_name;
    int busy_time;
    int idle_time;
    int resources_length = 0;
    Resource* resources = new Resource[resources_length];
    int run_counter = 0;
    int wait_time = 0;
};

int RESOURCES_LENGTH = 0;
struct Resource* RESOURCES = new Resource[RESOURCES_LENGTH];
int JOBS_LENGTH = 0;
struct Job* JOBS = new Job[JOBS_LENGTH];
sem_t SEMAPHORE;
pthread_t* TID;

void growGlobalJobsList(){
    Job* temp = new Job[JOBS_LENGTH + 1];
    std::copy(JOBS, JOBS + JOBS_LENGTH, temp);
    delete [] JOBS;
    JOBS = temp;
    JOBS_LENGTH++;
}

void growJobResourcesList(Job& job){
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
    printf("Initialized resources:\n");

    for (int x = 0; x < RESOURCES_LENGTH; x++){
        std::cout << "name: " << RESOURCES[x].name << " value: " << RESOURCES[x].value << std::endl;
    }

    printf("==========================\n");    
}

void printParsedJobs() {
    printf("==========================\n");
    printf("Parsed jobs from file:\n");

    for (int x = 0; x < JOBS_LENGTH; x++){
        std::cout << "jobName: " << JOBS[x].job_name << " | busyTime: " << JOBS[x].busy_time << " | idleTime: " << JOBS[x].idle_time << std::endl;
        std::cout << "\tresources:";

        for (int i = 0; i < JOBS[x].resources_length; i++){
            std::cout << " | name: " << JOBS[x].resources[i].name << " value: " << JOBS[x].resources[i].value;
        }
        printf("\n\n");
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
            // remove extra white spaces
            if(resource_string[pos - 1] == ' '){
                resource_string.erase(0, pos + space_delimiter.length());
                continue;
            }

            growGlobalResourcesList();

            token = resource_string.substr(0, pos);
            resource_string.erase(0, pos + space_delimiter.length());

            pos = token.find(colon_delimiter);
            RESOURCES[RESOURCES_LENGTH - 1].name = token.substr(0, pos);
            token.erase(0, pos + colon_delimiter.length());
            RESOURCES[RESOURCES_LENGTH - 1].value = stoi(token);
        }
        
        // last resource:
        growGlobalResourcesList();
        token = resource_string;
        pos = token.find(colon_delimiter);
        RESOURCES[RESOURCES_LENGTH - 1].name = token.substr(0, pos);
        token.erase(0, pos + colon_delimiter.length());
        RESOURCES[RESOURCES_LENGTH - 1].value = stoi(token);

        printInitializedResources();
    } catch(const std::exception& e) {
        std::cout << "initResources - Error: " << &e << std::endl;
        exit(0);
    }
}

Job initJob(std::string job_string){
    struct Job job;
    std::string space_delimiter = " ";
    std::string colon_delimiter = ":";
    size_t pos = 0;
    std::string token;
    int index;

    try{
        // Job Name
        pos = job_string.find(space_delimiter);
        job.job_name = job_string.substr(0, pos);
        job_string.erase(0, pos + space_delimiter.length());

        // Busy Time
        pos = job_string.find(space_delimiter);
        job.busy_time = stoi(job_string.substr(0, pos));
        job_string.erase(0, pos + space_delimiter.length());

        // Idle Time
        pos = job_string.find(space_delimiter);
        job.idle_time = stoi(job_string.substr(0, pos));
        job_string.erase(0, pos + space_delimiter.length());

        // Resources
        while ((pos = job_string.find(space_delimiter)) != std::string::npos) {
            growJobResourcesList(job);
            index = job.resources_length - 1;

            token = job_string.substr(0, pos);
            job_string.erase(0, pos + space_delimiter.length());

            pos = token.find(colon_delimiter);
            job.resources[index].name = token.substr(0, pos);
            token.erase(0, pos + colon_delimiter.length());

            job.resources[index].value = stoi(token);
        }
        
        // last resource:
        growJobResourcesList(job);
        index = job.resources_length - 1;
        token = job_string;

        pos = token.find(colon_delimiter);
        job.resources[index].name = token.substr(0, pos);
        token.erase(0, pos + colon_delimiter.length());
        job.resources[index].value = stoi(token);

        return job;
    } catch(const std::exception& e) {
        std::cout << "initJob - Error: " << &e << std::endl;
        exit(0);
    }
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
            struct Job job = initJob(line);

            growGlobalJobsList();
            JOBS[JOBS_LENGTH - 1] = job;
        }
    }

    printParsedJobs();

    newfile.close();
    return -2;
}

void* getResources(void* args){
    sem_wait(&SEMAPHORE);

    printf("getting resources\n");

    Job job = *(Job*) args;

    job.run_counter++;

    std::cout << "job: " << job.job_name << " (tid= 140214559033088, " << " iter= " << job.run_counter << ", time= 600 msec)" << std::endl;

    sleep(job.busy_time / 1000);

    sem_post(&SEMAPHORE);
}

void iterJobs(){
    sem_init(&SEMAPHORE, 0, 1);
    int ret;

    for(int i = 0; i < JOBS_LENGTH; i++){

        Job* job = &JOBS[i];
        ret = pthread_create(&TID[i], NULL, &getResources, job);

        if(ret != 0){
            std::cout << "Error: failed to create thread for job: " <<  JOBS[i].job_name << std::endl;
        }

    }

    sem_destroy(&SEMAPHORE);
}

int main(int argc, char *argv[]) {

    if(argc < 4){
        printf("Invalid command execution of file\n");
        return 1;
    }

    std::string input_file = argv[1];
    //int monitor_time = atoi(argv[2]);
    int NITER = atoi(argv[3]);

    processInputFile(input_file);
    TID = new pthread_t[JOBS_LENGTH];

    for(int i = 0; i < NITER; i++){
        iterJobs();
    }
}