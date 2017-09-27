#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static struct option long_options[] =
{
	{"output",required_argument,NULL,'o'},
	{"input",required_argument,NULL,'i'},
	{"help",no_argument,NULL,'h'},
	{"version",no_argument,NULL,'V'},
    {NULL, 0, NULL, 0}
};

void showVersion(){
	printf("%s\n", "TP1 Organizacion de computadoras - VERSION: 1.0");
}

void showHelp(){
	printf("%s\n", "TP1 Organizacion de computadoras - HELP");
	printf("%s\n", "Usage: ");
	printf("%s\n", "tp0 -h");
	printf("%s\n", "tp0 -V");
	printf("%s\n", "tp0 [options] file");
	printf("%s\n", "tp0 Options:");
	printf("%s\n", "-V --version	Print version and quit");
	printf("%s\n", "-h --help		Print this information");
	printf("%s\n", "-o --output		Path to output file");
	printf("%s\n", "-i --input		Path to input file");
	printf("%s\n", "Examples: ");
	printf("%s\n", "tp0 -q -i input.txt -o output.txt");
}

int main(int argc, char *argv[]){
	char* input;
	char* output;
	int ch;
	// loop over all of the options
	while ((ch = getopt_long(argc, argv, "i:o:hv", long_options, NULL)) != -1)
	{
		// check to see if a single character or long option came through
		switch (ch)
		{
			case 'o':
				output = optarg; 
				printf("%s\n",output);
				break;
			case 'i':
				input = optarg; 
				printf("%s\n",input);
				break;
			case 'V':
				showVersion();
				break;
			case 'h':
				showHelp();
				break;
			case '?':
				//Ya se escribió un error a stderr (lo hizo la funcion getopt_long.
				return -1;
		}
	}
}
