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

void *check_dpus_running(void *arg){

 	while(true){

                int nr_dpus_are_running = 0;
                printf("\n++++++++++++++++ Begin checking ++++++++++++++++\n");
                DPU_FOREACH(dpu_set, dpu){
                        bool dpu_is_running = false;
                        bool dpu_is_fault   = false;

                        dpu_error_t status = dpu_poll_dpu(dpu.dpu, &dpu_is_running, &dpu_is_fault);

                        if(dpu_is_running == true)
                                nr_dpus_are_running += 1;
                }
                time_t end_time;
                time(&end_time);
                printf("%lds, %d/%d are running %d are wasted\n", (end_time - start_time), nr_dpus_are_running, NR_DPUS_MAX, NR_DPUS_MAX - nr_dpus_are_running);
                printf("++++++++++++++++ End   checking ++++++++++++++++\n");

                usleep(MICROSECONDES);
                if(nr_dpus_are_running == 0)
                        break;
 	}
	pthread_exit(EXIT_SUCCESS);
}

void single_scheduler(){
	int argc;
	char **argv;
	printf("\n\n################# start single scheduler #################\n\n");

	parse_argv();
	
	time(&start_time);

	for(int id = 0; id < tour; id++){
		printf("\n++++++++++++++++ Begin round %d ++++++++++++++++\n", id + 1);
		if(atoi(programs[id].program[2]) > NR_DPUS_MAX){
			fprintf(stderr, "The program %s ask more than DPUS we have\n"\
				"We have %d DPUS program ask %s DPUS \n\n", programs[id].program[0], \
	 			NR_DPUS_MAX, programs[id].program[2]);	
			exit(0);
		}
		
		if(!strcmp(programs[id].program[0], "./BFS")){
			printf("--------------------- Begin BFS --------------------- \n");
			bfs(atoi(programs[id].program[2]));
			printf("--------------------- End   BFS --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./BS")){
			printf("--------------------- Begin BS --------------------- \n");
			bs(atoi(programs[id].program[2]));
			printf("--------------------- End   BS --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./GEMV")){
			printf("--------------------- Begin GEMV --------------------- \n");
			gemv(atoi(programs[id].program[2]));
			printf("--------------------- End   GEMV --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./HST-L")){
			printf("--------------------- Begin HST-L --------------------- \n");
			hst_l(atoi(programs[id].program[2]));
			printf("--------------------- End   HST-L --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./HST-S")){
			printf("--------------------- Begin HST-S --------------------- \n");
			hst_s(atoi(programs[id].program[2]));
			printf("--------------------- End   HST-S --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./MLP")){
			printf("--------------------- Begin MPL --------------------- \n");
			mpl(atoi(programs[id].program[2]));
			printf("--------------------- End   MPL --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./NW")){
			printf("--------------------- Begin NW --------------------- \n");
			nw(atoi(programs[id].program[2]));
			printf("--------------------- End   NW --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./RED")){
			printf("--------------------- Begin RED --------------------- \n");
			red(atoi(programs[id].program[2]));
			printf("--------------------- End   RED --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./SCAN-RSS")){
			printf("--------------------- Begin SCAN-RSS --------------------- \n");
			scan_rss(atoi(programs[id].program[2]));
			printf("--------------------- End   SCAN-RSS --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./SCAN-SSA")){
			printf("--------------------- Begin SCAN-SSA --------------------- \n");
			scan_ssa(atoi(programs[id].program[2]));
			printf("--------------------- End   SCAN-SSA --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./SEL")){
			printf("--------------------- Begin SEL --------------------- \n");
			sel(atoi(programs[id].program[2]));
			printf("--------------------- End   SEL --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./SpMV")){
			printf("--------------------- Begin SpMV --------------------- \n");
			spmv(atoi(programs[id].program[2]));
			printf("--------------------- End   SpMV --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./TRNS")){
			printf("--------------------- Begin TRNS --------------------- \n");
			trns(atoi(programs[id].program[2]));
			printf("--------------------- End   TRNS --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./TS")){
			printf("--------------------- Begin TS --------------------- \n");
			ts(atoi(programs[id].program[2]));
			printf("--------------------- End   TS --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./UNI")){
			printf("--------------------- Begin UNI --------------------- \n");
			uni(atoi(programs[id].program[2]));
			printf("--------------------- End   UNI --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./VA")){
			printf("--------------------- Begin VA --------------------- \n");
			va(atoi(programs[id].program[2]));
			printf("--------------------- End   VA --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./AES")){
			printf("--------------------- Begin AES --------------------- \n");
			aes(atoi(programs[id].program[2]));
			printf("--------------------- End   AES --------------------- \n");
		}
		else if(!strcmp(programs[id].program[0], "./UPIS")){
			printf("--------------------- Begin UPIS --------------------- \n");
			upis(atoi(programs[id].program[2]));
			printf("--------------------- End   UPIS --------------------- \n");
		}
		else{
			printf("--------------------- UNKNOW %s for the moment --------------------- \n", programs[id].program[0]);
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
