#include "header.h"

struct parameter setting = {.path="no_file"};
int tour = 0;

struct p programs[MAX_TRACES];


static void usage() {        
    fprintf(stderr,
        "\nUsage:  ./program [options]\n"
        "\nGeneral options:"
	"\n    -f        file who content traces"
	"\n    -h        help"
        "\n");                                                                                           
}

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

	struct dpu_set_t set, dpu;

	int i               = 0;
	int j               = 0;
	int round           = 0;
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
		printf("\n++++++++++++++++ Begin round %d ++++++++++++++++\n", round + 1);	
		DPU_ASSERT(dpu_alloc(nr_dpus_total, NULL, &set));
	
		//++++++++++++++++ Heart of scheduler :-) ++++++++++++++++
		int id = 0;
		DPU_FOREACH(set, dpu, id)
			sub_set_of_dpus[id] = dpu;

		int end, start = 0;
		do{
			end = atoi(programs[i].program[2]) + start - 1;

			if(!strcmp(programs[i].program[0], "./sum_elt_in_vector_dpu")){
				printf("--------------------- Begin load sum_elt_in_vector --------------------- \n");
				sum(start, end);
				printf("--------------------- End   load sum_elt_in_vector --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./factoriel_dpu")){
				printf("--------------------- Begin load factorial --------------------- \n");
				fac(start, end);
				printf("--------------------- End   load factorial --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./BFS")){
				printf("--------------------- Begin load BFS --------------------- \n");
				bfs(start, end);
				printf("--------------------- End   load BFS --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./BS")){
				printf("--------------------- Begin load BS --------------------- \n");
				bs(start, end);
				printf("--------------------- End   load BS --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./GEMV")){
				printf("--------------------- Begin load GEMV --------------------- \n");
				gemv(start, end);
				printf("--------------------- End   load GEMV --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./HST-L")){
				printf("--------------------- Begin load HST-L --------------------- \n");
				hst_l(start, end);
				printf("--------------------- End   load HST-L --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./HST-S")){
				printf("--------------------- Begin load HST-S --------------------- \n");
				hst_s(start, end);
				printf("--------------------- End   load HST-S --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./MLP")){
				printf("--------------------- Begin load MLP --------------------- \n");
				mlp(start, end);
				printf("--------------------- End   load MLP --------------------- \n");
			}
			else{
				printf("--------------------- UNKNOW %s for the moment --------------------- \n", programs[i].program[0]);
				exit(0);
			}

			start = end + 1;
			i += 1;
		}while(start < nr_dpus_total);	
		//++++++++++++++++         end            ++++++++++++++++
		
		DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

		DPU_ASSERT(dpu_free(set));
		printf("\n++++++++++++++++ End round %d ++++++++++++++++\n", round + 1);	
		//i += 1;
		round += 1;
		sleep(2);
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
