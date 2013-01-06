#include <stdio.h>
#include <stdlib.h>
#include "fgetopt.h"


#define  MEGABYTE 1048576UL

/*****************************************************************************
* arg_to_int - Convert argument string to integer.
*
* min - Minimum allowed value, inclusive.
* max - Maximum allowed value, inclusive.
* defalt - The default value, in case of an error.
* opt - Option string of this argument.  (ex., "-h");
*/

int arg_to_int(const char* arg, int min, int max, int defalt, const char* opt)
{
	int i = defalt;
	int rv;

	/* no argument means we use the default value */
	if(!arg) goto done;

	/* make sure we got an integer argument */
	rv = sscanf(arg, "%d", &i);
	if(rv != 1) {
		fprintf(stderr, "%s: integer argument required.\n", opt);
		exit(1);
		i = defalt;
		goto done;
	}

	/* make sure the integer argument is within the desired range */
	if(i < min || max < i) {
		fprintf(stderr, "%s: argument out of integer range.\n", opt);
		i = defalt;
		goto done;
	}

done:
	return i;
}


/*****************************************************************************
* help
*/

void help()
{
	printf(
"Usage: ctt [OPTION] \n"
"   -h              help menu (this screen)\n"
"   -s              server mode\n"
"   -t testCount    number of tests (default 5)\n"
"   -k chunkSize    size of a single data chunk in bytes (default 1MB)\n"
"   -n nChunks      Number of data chunks transmited during test (default 100)\n"
"   -T callTimeout  omniORB call timeout in miliseconds (default 2000)\n"

//"   -v[level]       set verbosity level (5 is default; doesn't do anything)\n"
//"   -o filename     set output filename (doesn't do anything)\n"
	);
}



class Options {
	public:
		int verbose;
		int server;
		int testCount;
		int nRetries;
		unsigned long callTimeout;
		unsigned long chunkSize;
		int nChunks;

		Options();
		void getCL(int argc, char** argv);

};


Options::Options(void)
{
//	initialization
	verbose=0;				// verbose display?
	server=0;				// is this server? 
	testCount=5;			// how many tests?
	nRetries=1;
	callTimeout=2000;		// Connection timeout in ms
	chunkSize=MEGABYTE;
	nChunks=100;
}

void Options::getCL(int argc, char** argv)
{
// list of options (option -O required for omniORB)
	char opts[]="-sht:k:n:t:T:O:"; 

	/* check arguments */
	while(1) {
		int c = fgetopt(argc, argv, opts);
		if(c == -1) break;

		switch(c) {
			case 's': server=1; break;

			case 'h': help(); exit(0); break;

			case 't': testCount = arg_to_int(optarg, 0, 10, 5, "t"); break;

			case 'k': chunkSize = arg_to_int(optarg, 0, 10000000UL, MEGABYTE, "k"); break;

			case 'n': nChunks = arg_to_int(optarg, 0, 10000UL, 100, "n"); break;

			case 'T': callTimeout = arg_to_int(optarg, 0, 1000000UL, 10, "T"); break;

			case '?': if(optopt=='t' || optopt=='k' || optopt=='n' || optopt=='t' || optopt=='T')
							 fprintf (stderr, "Option -%c requires an argument.\n", optopt);
						 else if (isprint (optopt))
							 fprintf (stderr, "Unknown option `-%c'.\n", optopt);
						 else
						    fprintf (stderr, "Unknown option character `\\x%x'.\n",  optopt);
					    exit(1);

			#ifdef DEBUG
			default:
				printf("Option '%c' (%d) with '%s'\n", c, c, optarg);
			#endif
		}
	}

	#ifdef DEBUG
		printf("optind at %d; argv[optind] = '%s'\n", optind, argv[optind]);
	#endif
}


/* vim:ts=3:sw=3
*/
