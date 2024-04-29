// compilation --> gcc generate_trace.c -lm 

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define MAX_DPUS_PER_RANK 244 
#define MAX_TASKLETS 24
#define SIZE_MAX_APP_NAME 256
#define MAX_APPS 1024
#define FALSE 0
#define TRUE 1

struct parameter{
	char *path;
	char *low;
	int seed;
	double alpha;
	int nr_line;
} setting = {.low="ZIPF", .path="no_file", .seed=1, .alpha=1.0, .nr_line=10};

unsigned int max_data_per_apps[MAX_APPS];
char apps_name[MAX_APPS][SIZE_MAX_APP_NAME];

static void usage() {                                                                                    
    fprintf(stderr,
        "\nUsage:  ./program [options]"                                                                  
        "\n"
        "\nGeneral options:"
	"\n    -a        a alpha value"
	"\n    -f        list of applications"
        "\n    -h        help"
	"\n    -l        a low"
	"\n    -n        a number of trace line"
	"\n    -s        a seed"
        "\n");                                                                                           
}                                                                                                        

static void check_params(int argc, char **argv) {                                                      
    
    int opt;
    if(argc < 2){
        usage();
	exit(0);
    }
    while((opt = getopt(argc, argv, "ha:f:l:n:s:")) >= 0) { 
	    switch(opt) {
		case 'h': usage();  exit(0);
			  break;
		case 'a': setting.alpha     = atof(optarg); break;
		case 'f': setting.path      = optarg; break;
		case 'l': setting.low       = optarg; break; 
		case 'n': setting.nr_line   = atoi(optarg); break;
		case 's': setting.seed      = atoi(optarg); break;
		default :
			  fprintf(stderr, "\nUnrecognized option!\n");
			  usage();
			  exit(0);
	}                                                                                             
    }
}

// This part of code if source from 
// https://cse.usf.edu/~kchriste/tools/genzipf.c
// start here
double rand_val(int seed){
    const long  a =      16807;  // Multiplier
    const long  m = 2147483647;  // Modulus
    const long  q =     127773;  // m div a
    const long  r =       2836;  // m mod a
    static long x;               // Random int value
    long        x_div_q;         // x divided by q
    long        x_mod_q;         // x modulo q
    long        x_new;           // New x value

    // Set the seed if argument is non-zero and then return zero
    if (seed > 0)
    {
        x = seed;
        return (0.0);
    }

    // RNG using integer arithmetic
    x_div_q = x / q;
    x_mod_q = x % q;
    x_new = (a * x_mod_q) - (r * x_div_q);
    if (x_new > 0)
        x = x_new;
    else
        x = x_new + m;

    // Return a random value between 0.0 and 1.0
    return((double) x / m);
}

int zipf(double alpha, int n){
    static int first = TRUE;      // Static first time flag
    static double c = 0;          // Normalization constant
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value;            // Computed exponential value to be returned
    int    i;                     // Loop counter

    // Compute normalization constant on first call only
    if (first == TRUE)
    {
        for (i=1; i<=n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = FALSE;
    }

    // Pull a uniform random number (0 < z < 1)
    do
    {
        z = rand_val(0);
    }
    while ((z == 0) || (z == 1));

    // Map z to the value
    sum_prob = 0;
    for (i=1; i<=n; i++)
    {
        sum_prob = sum_prob + c / pow((double) i, alpha);
        if (sum_prob >= z)
        {
            zipf_value = i;
            break;
        }
    }

    // Assert that zipf_value is between 1 and N
    assert((zipf_value >=1) && (zipf_value <= n));

    return(zipf_value);
}
// and end here

void generate_file(){
    FILE *desp  = fopen(setting.path, "r");
    FILE *trace = fopen("traces", "w");

    if(desp == NULL){
        fprintf(stderr, "\n!!!    NO SUCH FILE    !!!\n");
        exit(0);
    }
    if(trace == NULL){
	fprintf(stderr, "\n!!!   ERROR to Open traces file   !!!\n");
	exit(0);
    }
    
    fprintf(trace, "app name,number of dpus,number of taskelt,input data\n");
    int nr_apps = 1;
    char buffer[SIZE_MAX_APP_NAME];
    while(fgets(buffer, SIZE_MAX_APP_NAME, desp)){
       sscanf(buffer, "%s %u", apps_name[nr_apps], &max_data_per_apps[nr_apps]);
       nr_apps += 1; 
    }
     
    nr_apps -= 1;
    rand_val(setting.seed);
    srand(time(NULL));
    for(int line = 0; line < setting.nr_line; line++){
	    int zipf_app     = zipf(setting.alpha, nr_apps);
	    int zipf_dpu     = (int)rand() % (int)MAX_DPUS_PER_RANK + 1;
	    int zipf_tasklet = (int)rand() % (int)MAX_TASKLETS + 1;
	    int zipf_data    = (int)rand() % (int) max_data_per_apps[zipf_app] + 1;

	    fprintf(trace, "%s %d %d %d\n", apps_name[zipf_app], zipf_dpu, zipf_tasklet, zipf_data);
    }

    printf("\n\n------> output file \"traces\" is generated <------\n");
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
