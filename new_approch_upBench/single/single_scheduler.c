#include "header.h"

struct parameter setting = {.path="no_file", .out="no_file"};
int tour = 0;

struct p programs[MAX_TRACES];

struct metric{
	int arrivee;
	int launch;
	int fin;
} metrics[MAX_TRACES];

void check_launch(int id)
{
	struct timeval end_time;
	long int sec;

	gettimeofday(&end_time, NULL);
	sec = end_time.tv_sec - start_time.tv_sec;
	metrics[id].launch  = sec; 
	metrics[id].arrivee = atoi(programs[id].program[0]);
}

void check_end(int id)
{
	struct timeval end_time;
	long int sec;

	gettimeofday(&end_time, NULL);
	sec = end_time.tv_sec - start_time.tv_sec;
	metrics[id].fin     = sec; 
}

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
    while((opt = getopt(argc, argv, "hf:n:")) >= 0) { 
	    switch(opt) {
		case 'h': usage();  exit(0);
			  break;
		case 'f': setting.path       = optarg; break;
		case 'n': setting.out       = optarg; break;
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
		     loop[MAX_TRACES], 
		     delais[MAX_TRACES];
		sscanf(buffer, "%s %s %s %s", delais, app_name, nr_dpus, loop);

		programs[tour].program[0] = malloc(sizeof(char) * strlen(delais));
		strcpy(programs[tour].program[0], delais);

		programs[tour].program[1] = malloc(sizeof(char) * strlen(app_name));
		strcpy(programs[tour].program[1], app_name);
		
		programs[tour].program[2] = malloc(sizeof(char) * strlen(nr_dpus));
		strcpy(programs[tour].program[2], nr_dpus);

		programs[tour].program[3] = malloc(sizeof(char) * strlen(loop));
		strcpy(programs[tour].program[3], loop);

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

                struct timeval end_time;
		long int sec, millisec, microsec;

                gettimeofday(&end_time, NULL);
		sec      = end_time.tv_sec - start_time.tv_sec;
		microsec = end_time.tv_usec - start_time.tv_usec;
		millisec = (long int)(sec*1000 + microsec/1000);
                printf("%ldms, %d/%d are running %d are wasted\n", sec, nr_dpus_are_running, NR_DPUS_MAX, NR_DPUS_MAX - nr_dpus_are_running);
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
	
	gettimeofday(&start_time, NULL);

	for(int id = 0; id < tour; id++){

		struct timeval end_time;
		long int sec;
		do{
			gettimeofday(&end_time, NULL);
			sec = end_time.tv_sec - start_time.tv_sec;
		}while(sec < atoi(programs[id].program[0]));

		printf("\n++++++++++++++++ Begin round %d ++++++++++++++++\n", id + 1);
		if(atoi(programs[id].program[2]) > NR_DPUS_MAX){
			fprintf(stderr, "The program %s ask more than DPUS we have\n"\
				"We have %d DPUS program ask %s DPUS \n\n", programs[id].program[1], \
	 			NR_DPUS_MAX, programs[id].program[2]);	
			exit(0);
		}
		
		if(!strcmp(programs[id].program[1], "./simple-bench")){
			printf("--------------------- Begin simple-bench --------------------- \n");
			
			check_launch(id);
			simple_bench(atoi(programs[id].program[2]), atoi(programs[id].program[3]));
			check_end(id);

			printf("--------------------- End   simple-bench --------------------- \n");
		}
		else{
			printf("--------------------- UNKNOW %s for the moment --------------------- \n", programs[id].program[1]);
		}


		printf("++++++++++++++++ End round %d ++++++++++++++++\n\n", id + 1);
		sleep(2); // wait a few minutes for dpu_free to finish 
	}

	printf("\n\n################# END Excution of %d programs #################\n\n", tour);
}

void print_metrics(){
	FILE *fichier = fopen(setting.out, "w");
	for(int id = 0; id < tour; id++){
		int nr_dpus = atoi(programs[id].program[2]);
		int loop    = atoi(programs[id].program[3]);
		fprintf(fichier, "%d %s %d %d %d %d\n", metrics[id].arrivee, programs[id].program[1], nr_dpus, loop, metrics[id].launch, metrics[id].fin);
	}
	fclose(fichier);
}

int main(int argc, char **argv){

	check_params(argc, argv);
	if(strcmp(setting.path, "no_file") == 0){
		fprintf(stderr, "\n---> Please specify path to traces <---\n");
		usage();
		exit(0);
	}
	
	single_scheduler();
	
	print_metrics();

	return 0;
}
