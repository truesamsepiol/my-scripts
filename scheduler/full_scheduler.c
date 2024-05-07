#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>

#define MAX_TRACES 10000
#define NR_PARAMETERS 7
#define SIZE_OF_COMMAND_PARAMETERS 30
#define NR_DPUS_MAX 13

struct parameter{
	char *path;
}setting = {.path="no_file"};

struct p{
	char *program[NR_PARAMETERS];
}programs[MAX_TRACES];

static void usage() {        
    fprintf(stderr,
        "\nUsage:  ./program [options]\n"
        "\nGeneral options:"
	"\n    -f        file who content traces"
	"\n    -h        help"
        "\n");                                                                                           
}

int tour            = 0;

static void check_params(int argc, char **argv) {                                                      
    
    int opt;
    if(argc < 2){
        usage();
	exit(0);
    }
    while((opt = getopt(argc, argv, "hf:")) >= 0) { 
	    switch(opt) {
		case 'h': usage();  exit(0);
			  break;
		case 'f': setting.path       = optarg; break;
		default :
			  fprintf(stderr, "\nUnrecognized option!\n");
			  usage();
			  exit(0);
	}                                                                                             
    }
}

void parse_argv(){
	
	FILE *desp = fopen(setting.path, "r");
	if(desp == NULL){
		fprintf(stderr, "\n!!!		NO SUCH FILE		!!!\n");
		exit(0);
	}
	
	char buffer[MAX_TRACES];

	while(fgets(buffer, MAX_TRACES, desp)){
		char app_name[MAX_TRACES];
		char nr_dpus[MAX_TRACES], 
		     nr_tasklets[MAX_TRACES], 
		     data[MAX_TRACES];
		sscanf(buffer, "%s %s %s %s", app_name, nr_dpus, nr_tasklets, data);

		programs[tour].program[0] = malloc(sizeof(char) * strlen(app_name));
		strcpy(programs[tour].program[0], app_name);
		
		programs[tour].program[1] = malloc(sizeof(char) * SIZE_OF_COMMAND_PARAMETERS);
		strcpy(programs[tour].program[1], "-NR_DPUS");

		programs[tour].program[2] = malloc(sizeof(char) * strlen(nr_dpus));
		strcpy(programs[tour].program[2], nr_dpus);

		programs[tour].program[3] = malloc(sizeof(char) * SIZE_OF_COMMAND_PARAMETERS);
		strcpy(programs[tour].program[3], "-NR_TASKLETS");

		programs[tour].program[4] = malloc(sizeof(char) * strlen(nr_tasklets));
		strcpy(programs[tour].program[4], nr_tasklets);

		programs[tour].program[5] = malloc(sizeof(char) * SIZE_OF_COMMAND_PARAMETERS);
		strcpy(programs[tour].program[5], "-d");

		programs[tour].program[6] = malloc(sizeof(char) * strlen(data));
		strcpy(programs[tour].program[6], data);
		
		tour += 1;
	}
}

void full_scheduler(){

	printf("\n\n################# start full scheduler #################\n\n");

	parse_argv();

	int i               = 0;
	int j               = 0;
	while(j < tour){
		int nr_dpus_total = 0;
		while(j < tour){
			if(atoi(programs[j].program[2]) > NR_DPUS_MAX){
					fprintf(stderr, "The program %s ask more than DPUS we have\n"\
							"We have %d DPUS program ask %s DPUS \n\n", programs[j].program[0], \
						       NR_DPUS_MAX, programs[j].program[2]);	
					exit(0);
			}
			if(nr_dpus_total + atoi(programs[j].program[2]) <= NR_DPUS_MAX)
				nr_dpus_total += atoi(programs[j].program[2]);
			else
				break;
			j++;
		}
		
		struct dpu_set_t set, dpu;
		DPU_ASSERT(dpu_alloc(nr_dpus_total, NULL, &set));
	
		int sum_size = 0;
		int nr_dpus  = atoi(programs[i].program[2]);
		DPU_FOREACH(set, dpu){
			sum_size += 1;
			if(sum_size > nr_dpus){
				i += 1;
				nr_dpus += atoi(programs[i].program[2]);
			}
			DPU_ASSERT(dpu_load(dpu, programs[i].program[0], NULL));
		}

		DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

		DPU_FOREACH(set, dpu)
			DPU_ASSERT(dpu_log_read(dpu, stdout));
	
	
		DPU_ASSERT(dpu_free(set));
		i += 1;
	}

	printf("\n\n################# END Excution of %d programs #################\n\n", tour);
}

int main(int argc, char **argv){

	check_params(argc, argv);
	if(strcmp(setting.path, "no_file") == 0){
		fprintf(stderr, "\n---> Please specify path to traces <---\n");
		usage();
		exit(0);
	}

	full_scheduler();

	return 0;
}
