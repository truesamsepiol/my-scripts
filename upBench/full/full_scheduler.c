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
		DPU_FOREACH(set, dpu){
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


void full_scheduler(){

	printf("\n\n################# start full scheduler #################\n\n");

	parse_argv();
	
	time(&start_time);	
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
		printf("total dpus allocated %d\n", nr_dpus_total);
	
		//++++++++++++++++ Heart of scheduler :-) ++++++++++++++++
		int id = 0;
		DPU_FOREACH(set, dpu, id)
			sub_set_of_dpus[id] = dpu;

		int end, start = 0;
		pthread_t threads[NR_DPUS_MAX];
		struct args threads_args[NR_DPUS_MAX];
		int nr_max_threads = 0;
		do{
			end = atoi(programs[i].program[2]) + start - 1;

			if(!strcmp(programs[i].program[0], "./sum_elt_in_vector_dpu")){
				printf("--------------------- Begin load sum_elt_in_vector --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, sum, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load sum_elt_in_vector --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./factoriel_dpu")){
				printf("--------------------- Begin load factorial --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, fac, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load factorial --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./BFS")){
				printf("--------------------- Begin load BFS --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, bfs, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load BFS --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./BS")){
				printf("--------------------- Begin load BS --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, bs, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load BS --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./GEMV")){
				printf("--------------------- Begin load GEMV --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, gemv, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load GEMV --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./HST-L")){
				printf("--------------------- Begin load HST-L --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, hst_l, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load HST-L --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./HST-S")){
				printf("--------------------- Begin load HST-S --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, hst_s, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load HST-S --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./MLP")){
				printf("--------------------- Begin load MLP --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, mlp, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load MLP --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./NW")){
				printf("--------------------- Begin load NW --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, nw, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load NW --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./RED")){
				printf("--------------------- Begin load RED --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, red, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load RED --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./SCAN-RSS")){
				printf("--------------------- Begin load SCAN-RSS --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, scan_rss, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load SCAN-RSS --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./SCAN-SSA")){
				printf("--------------------- Begin load SCAN-SSA --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, scan_ssa, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load SCAN-SSA --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./SEL")){
				printf("--------------------- Begin load SEL --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, sel, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load SEL --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./SpMV")){
				printf("--------------------- Begin load SpMV --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, spmv, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load SpMV --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./TRNS")){
				printf("--------------------- Begin load TRNS --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, trns, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load TRNS --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./TS")){
				printf("--------------------- Begin load TS --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, ts, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load TS --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./UNI")){
				printf("--------------------- Begin load UNI --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, uni, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load UNI --------------------- \n");
			}
			else if(!strcmp(programs[i].program[0], "./VA")){
				printf("--------------------- Begin load VA --------------------- \n");
				threads_args[nr_max_threads].start = start;
				threads_args[nr_max_threads].end   = end;
				pthread_create(&threads[nr_max_threads], NULL, va, (void *) &threads_args[nr_max_threads]);
				printf("--------------------- End   load VA --------------------- \n");
			}
			else{
				printf("--------------------- UNKNOW %s for the moment --------------------- \n", programs[i].program[0]);
				exit(0);
			}

			start = end + 1;
			i += 1;
			nr_max_threads += 1;
		}while(start < nr_dpus_total);	
		//++++++++++++++++         end            ++++++++++++++++
		for(int tid = 0; tid < nr_max_threads; tid++)
			pthread_join(threads[tid], NULL);

		DPU_ASSERT(dpu_launch(set, DPU_ASYNCHRONOUS));

		pthread_t thread;
		pthread_create(&thread, NULL, check_dpus_running, NULL);
		
		DPU_ASSERT(dpu_sync(set));
		
		pthread_join(thread, NULL);

		DPU_ASSERT(dpu_free(set));
		printf("\n++++++++++++++++ End round %d ++++++++++++++++\n", round + 1);	
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
