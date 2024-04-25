#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  
#include <string.h>


#define SIZE_MAX_APP_NAME 256

struct parameter{
	char *path;
	char *low;
} setting = {.low="zipf", .path="no_file"};

static void usage() {                                                                                    
    fprintf(stderr,
        "\nUsage:  ./program [options]"                                                                  
        "\n"
        "\nGeneral options:"
        "\n    -h        help"
	"\n    -f        list of applications"
	"\n    -l        a low"
        "\n");                                                                                           
}                                                                                                        

static void check_params(int argc, char **argv) {                                                      
    
    int opt;
    if(argc < 2){
        usage();
	exit(0);
    }
    while((opt = getopt(argc, argv, "hl:f:")) >= 0) {                                                
        switch(opt) {                                                                                    
        case 'h':                                                                                        
            usage();                                                                                         
            exit(0);                                                                                         
            break;
        case 'f': setting.path = optarg; break;                                                 
        case 'l': setting.low  = optarg; break;                                                 
        default:
            fprintf(stderr, "\nUnrecognized option!\n");                                                 
            usage();                                                                                     
            exit(0);                                                                                     
        }                                                                                                
    }
}

void generate_file(){
    FILE *desp  = fopen(setting.path, "r");
    FILE *trace = fopen("traces", "w");

    char app[SIZE_MAX_APP_NAME];

    if(desp == NULL){
        fprintf(stderr, "\n!!!    NO SUCH FILE    !!!\n");
        exit(0);
    }
    if(trace == NULL){
	fprintf(stderr, "\n!!!   ERROR to Open traces file   !!!\n");
	exit(0);
    }
    
    fprintf(trace, "app name,number of dpus,number of taskelt,input data\n");
    while(fscanf(desp, "%s", app) == 1){
        fprintf(trace, "%s\n", app);
    }
    
    fclose(desp);
    fclose(trace);
}


int main(int argc, char **argv){
    
    check_params(argc, argv);
    if(strcmp(setting.path, "no_file") == 0){
	    fprintf(stderr, "\n---> Please specify path to application list <---\n");
	    usage();
	    exit(0);
    }

    generate_file(); 
    return 0;
}
