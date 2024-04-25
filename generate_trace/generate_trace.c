#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  
#include <string.h>

struct parameter{
	char *path;
	char *low;
} setting = {.low="zipfian", .path="no_file"};

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

int main(int argc, char **argv){
    
    check_params(argc, argv);
    printf("path ->> %s\nlow ->> %s\n", setting.path, setting.low);
    printf("All rigth\n");
    return 0;
}
