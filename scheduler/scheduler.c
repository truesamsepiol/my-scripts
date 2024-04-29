#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>


struct parameter{
	char *sched_name;
	char *path;
} setting = {.sched_name="single", .path="no_file"};


static void usage() {                                                                                    
    fprintf(stderr,
        "\nUsage:  ./program [options]"                                                                  
        "\n"
        "\nGeneral options:"
	"\n    -f        file who content traces"
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
	
	int round = 0;	
	char *program[] = {"./program", "-s", "single", "-f", "traces", NULL};	
	while(round < 10){
		int pid = fork();
		if(pid == 0)
			execv(program[0], program);
		else{
			wait(NULL);
			round += 1;
		}
	}
	printf("\n\n################# END Excution of %d programs #################\n\n", round);
}

void full_scheduler(){
	printf("\n\n################# start full scheduler #################\n\n");
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
	else
		single_scheduler();

	return 0;
}
