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

void single_scheduler(){
	printf("\n\n################# start single scheduler #################\n\n");

	parse_argv();
	
	for(int id = 0; id < tour; id++){
		printf("\n++++++++++++++++ Begin round %d ++++++++++++++++\n", id + 1);
		if(atoi(programs[id].program[2]) > NR_DPUS_MAX){
			fprintf(stderr, "The program %s ask more than DPUS we have\n"\
				"We have %d DPUS program ask %s DPUS \n\n", programs[id].program[0], \
	 			NR_DPUS_MAX, programs[id].program[2]);	
			exit(0);
		}
		
		if(!strcmp(programs[id].program[0], "./BFS")){
			printf("BFS\n");
		}
		else if(!strcmp(programs[id].program[0], "./BS")){
			printf("BS\n");
		}
		else if(!strcmp(programs[id].program[0], "./GEMV")){
			printf("GEMV\n");
		}
		else if(!strcmp(programs[id].program[0], "./HST-L")){
			printf("HST-L\n");
		}
		else if(!strcmp(programs[id].program[0], "./HST-S")){
			printf("HST-S\n");
		}
		else if(!strcmp(programs[id].program[0], "./MLP")){
			printf("MLP\n");
		}
		else if(!strcmp(programs[id].program[0], "./NW")){
			printf("NW\n");
		}
		else if(!strcmp(programs[id].program[0], "./RED")){
			printf("RED\n");
		}
		else if(!strcmp(programs[id].program[0], "./SCAN-RSS")){
			printf("SCAN-RSS\n");
		}
		else if(!strcmp(programs[id].program[0], "./SCAN-SSA")){
			printf("SCAN-SSA\n");
		}
		else if(!strcmp(programs[id].program[0], "./SEL")){
			printf("SEL\n");
		}
		else if(!strcmp(programs[id].program[0], "./SpMV")){
			printf("SpMV\n");
		}
		else if(!strcmp(programs[id].program[0], "./TRNS")){
			printf("TRNS\n");
		}
		else if(!strcmp(programs[id].program[0], "./TS")){
			printf("TS\n");
		}
		else if(!strcmp(programs[id].program[0], "./UNI")){
			printf("UNI\n");
		}
		else if(!strcmp(programs[id].program[0], "./VA")){
			printf("VA\n");
		}
		else{

			DPU_ASSERT(dpu_alloc(atoi(programs[id].program[2]), NULL, &set));
			DPU_ASSERT(dpu_load(set, programs[id].program[0], NULL));
			DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

			DPU_FOREACH(set, dpu)
				DPU_ASSERT(dpu_log_read(dpu, stdout));
			DPU_ASSERT(dpu_free(set));
		}
		printf("++++++++++++++++ End round %d ++++++++++++++++\n\n", id + 1);
		sleep(2); // wait a few minutes for dpu_free to finish 
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
	
	single_scheduler();

	return 0;
}
