#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>

#define MAX_TRACES 10000
#define NR_PARAMETERS 7
#define SIZE_OF_COMMAND_PARAMETERS 30


struct parameter{
	char *sched_name;
	char *path;
}setting = {.sched_name="single", .path="no_file"};

struct p{
	char *program[NR_PARAMETERS];
}programs[MAX_TRACES];

static void usage() {                                                                                    
    fprintf(stderr,
        "\nUsage:  ./program [options]"                                                                         "\n"
        "\nGeneral options:"
	"\n    -f        file who content traces"
	"\n    -h        help"
	"\n    -s        {full,single}"
        "\n");                                                                                           
}                                                                                                        

static void check_params(int argc, char **argv) {                                                      
    
    int opt;
    if(argc < 2){
        usage();
	exit(0);
    }
    while((opt = getopt(argc, argv, "hs:f:")) >= 0) { 
	    switch(opt) {
		case 'h': usage();  exit(0);
			  break;
		case 'f': setting.path       = optarg; break;
		case 's': setting.sched_name = optarg; break;
		default :
			  fprintf(stderr, "\nUnrecognized option!\n");
			  usage();
			  exit(0);
	}                                                                                             
    }
}

void single_scheduler(){
	printf("\n\n################# start single scheduler #################\n\n");
	
	FILE *desp = fopen(setting.path, "r");
	if(desp == NULL){
		fprintf(stderr, "\n!!!		NO SUCH FILE		!!!\n");
		exit(0);
	}
	
	char buffer[MAX_TRACES];
	int round = 0;	
	while(fgets(buffer, MAX_TRACES, desp)){
		char app_name[MAX_TRACES];
		char nr_dpus[MAX_TRACES], 
		     nr_tasklets[MAX_TRACES], 
		     data[MAX_TRACES];
		sscanf(buffer, "%s %s %s %s", app_name, nr_dpus, nr_tasklets, data);

		programs[round].program[0] = malloc(sizeof(char) * strlen(app_name));
		strcpy(programs[round].program[0], app_name);
		
		programs[round].program[1] = malloc(sizeof(char) * SIZE_OF_COMMAND_PARAMETERS);
		strcpy(programs[round].program[1], "-NR_DPUS");

		programs[round].program[2] = malloc(sizeof(char) * strlen(nr_dpus));
		strcpy(programs[round].program[2], nr_dpus);

		programs[round].program[3] = malloc(sizeof(char) * SIZE_OF_COMMAND_PARAMETERS);
		strcpy(programs[round].program[3], "-NR_TASKLETS");

		programs[round].program[4] = malloc(sizeof(char) * strlen(nr_tasklets));
		strcpy(programs[round].program[4], nr_tasklets);

		programs[round].program[5] = malloc(sizeof(char) * SIZE_OF_COMMAND_PARAMETERS);
		strcpy(programs[round].program[5], "-d");

		programs[round].program[6] = malloc(sizeof(char) * strlen(data));
		strcpy(programs[round].program[6], data);
		
		round += 1;
	}
	
	for(int id = 0; id < round; id++){
		char *program[NR_PARAMETERS];

		for(int j = 0; j < NR_PARAMETERS; j++){
			program[j] = malloc(sizeof(char) * strlen(programs[id].program[j]));
			strcpy(program[j], programs[id].program[j]);
		}

		int pid = fork();
		if(pid == 0)
			execv(program[0], program);
		else
			wait(NULL);
	}

	printf("\n\n################# END Excution of %d programs #################\n\n", round);
}

void full_scheduler(){
	int round = 0;
	printf("\n\n################# start full scheduler #################\n\n");
	printf("\n\n################# END Excution of %d programs #################\n\n", round);
}


int main(int argc, char **argv){

	check_params(argc, argv);
	if(strcmp(setting.path, "no_file") == 0){
		fprintf(stderr, "\n---> Please specify path to traces <---\n");
		usage();
		exit(0);
	}
	if(strcmp(setting.sched_name, "full") == 0)
		full_scheduler();
	else if(strcmp(setting.sched_name, "single") == 0)
		single_scheduler();
	else
		usage();

	return 0;
}
