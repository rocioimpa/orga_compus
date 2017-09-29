#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct receivedParameters {
	char* inputFilePath;
	char* outputFilePath;
	char* inputBufferByteCount;
	char* outputBufferByteCount;
} parameters_t;

static struct option long_options[] =
{
	{"output",required_argument,NULL,'o'},
	{"input",required_argument,NULL,'i'},
	{"help",no_argument,NULL,'h'},
	{"version",no_argument,NULL,'V'},
	{"ibuf-bytes",required_argument,NULL,'I'},
	{"obuf-bytes",required_argument,NULL,'O'},
    {NULL, 0, NULL, 0}
};

/* Declaracion de funciones */

void showVersion();
void showHelp();
FILE* openInputFile(char* pathToInputFile); 
FILE* openOutputFile(char* pathToOutputFile); 

/* ------------------------ */

void showVersion(){
	printf("%s\n", "TP1 Organizacion de computadoras - VERSION: 2.0");
	exit(0);
}

void showHelp(){
	printf("%s\n", "TP1 Organizacion de computadoras - HELP");
	printf("%s\n", "");
	printf("%s\n", "Usage: ");
	printf("%s\n", "tp1 -h");
	printf("%s\n", "tp1 -V");
	printf("%s\n", "tp1 [options] file");
	printf("%s\n", "tp1 Options:");
	printf("%s\n", "-V --version	Print version and quit");
	printf("%s\n", "-h --help		Print this information");
	printf("%s\n", "-o --output		Path to output file");
	printf("%s\n", "-i --input		Path to input file");
	printf("%s\n", "-I, --ibuf-bytes		Byte-count of the input buffer.");
	printf("%s\n", "-O, --obuf-bytes		Byte-count of the output buffer.");
	
	printf("%s\n", "");
	printf("%s\n", "Examples: ");
	printf("%s\n", "tp1 -i input.txt -o output.txt");
	exit(0);
}

FILE* openInputFile(char* pathToInputFile) {
	FILE* file;
	file = fopen(pathToInputFile, "rb");
	if (file == NULL) {
		fprintf(stderr, "Unable to open input file %s\n", pathToInputFile);
		exit(1);
	}
	return file;
}

FILE* openOutputFile(char* pathToOutputFile) {
	FILE* file;
	file = fopen(pathToOutputFile, "wb");
	if (file == NULL) {
		fprintf(stderr, "Unable to open output file %s\n", pathToOutputFile);
		exit(1);
	}
	return file;
}

int main(int argc, char *argv[]){

	int status;

    FILE* input;
    FILE* output;

	char* ibytes;
	char* obytes;

	int ch;
	parameters_t receivedParameters;

	// loop over all of the options
	while ((ch = getopt_long(argc, argv, "hVi:o:I:O:", long_options, NULL)) != -1)
	{
		// check to see if a single character or long option came through
		switch (ch)
		{
			case 'o':
				receivedParameters.outputFilePath = optarg; 
				break;
			case 'i':
				receivedParameters.inputFilePath = optarg; 
				break;
			case 'V':
				showVersion();
				break;
			case 'h':
				showHelp();
				break;
			case 'I':
				ibytes = optarg;
				//getInputBufferSize();
				break;
			case 'O':
				obytes = optarg;
				//getOutputBufferSize();
				break;
			case '?':
				//Ya se escribió un error a stderr (lo hizo la funcion getopt_long.
				return -1;
		}
	}
	
	if ((((receivedParameters.inputFilePath == NULL) && (receivedParameters.outputFilePath == NULL))) && (argc > 1)) {
        return 0;
    }

    if (receivedParameters.inputFilePath != NULL) {
        input = openInputFile(receivedParameters.inputFilePath);
    } else {
        input = stdin;
    }

    if (receivedParameters.outputFilePath != NULL) {
        output = openOutputFile(receivedParameters.outputFilePath);
    } else {
        output = stdout;
    }

    if (status != 0) {
        //fprintf(stderr, "An error has occurred %s\n");
    }

    fclose(input);
    fclose(output);

    return status;

}
