#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "palindromes.h"

typedef struct receivedParameters {
	char* input;
	char* output;
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
FILE* openInputFile(char* input); 
FILE* openOutputFile(char* output); 
parameters_t getParameters(int argc, char **argv);

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

parameters_t getParameters(int argc, char **argv){

    int ch;
    parameters_t receivedParameters;

    receivedParameters.input = NULL;
    receivedParameters.output = NULL;
    receivedParameters.inputBufferByteCount = NULL;
    receivedParameters.outputBufferByteCount = NULL;

    // loop over all of the options
    while ((ch = getopt_long(argc, argv, "hVi:o:I:O:", long_options, NULL)) != -1) {
        // check to see if a single character or long option came through
        switch (ch){
            case 'o':
                receivedParameters.input = optarg;
                break;
            case 'i':
                receivedParameters.output = optarg;
                break;
            case 'V':
                showVersion();
                exit(0);
                break;
            case 'h':
                showHelp();
                exit(0);
                break;
            case 'I':
                receivedParameters.inputBufferByteCount = optarg;
                break;
            case 'O':
                receivedParameters.outputBufferByteCount = optarg;
                break;
            case '?':
                if (optopt == 'i' || optopt == 'o' || optopt == 'I' || optopt == 'O') {
                    fprintf (stderr, "No arguments provided for option -%c .\n", optopt);
                } else if (isprint (optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf (stderr, "Unknown option `\\x%x'.\n", optopt);
                } //Ya se escribió un error a stderr (lo hizo la funcion getopt_long.
            default:
                showHelp();
                exit(1);
        }
    }
    return receivedParameters;

}

int main(int argc, char *argv[]){

    FILE* input;
    FILE* output;

	int result;

	parameters_t receivedParameters = getParameters(argc,argv);
	
	if ((((receivedParameters.input == NULL) && (receivedParameters.output == NULL))) && (argc > 1)) {
        return 0;
    }

    if (receivedParameters.input != NULL) {
        input = openInputFile(receivedParameters.input);
    }
    else {
        input = stdin;
    }

    if (receivedParameters.output != NULL) {
        output = openOutputFile(receivedParameters.output);
    } else {
        output = stdout;
    }

    int inputFileDescriptor = fileno(input);
    int outputFileDescriptor = fileno(output);
	printf("%d\n",inputFileDescriptor);
    //result = findWordsThatArePalindromes(inputFileDescriptor, outputFileDescriptor);
	
	palindrome(inputFileDescriptor,receivedParameters.inputBufferByteCount,outputFileDescriptor,receivedParameters.outputBufferByteCount);
	
    fclose(input);
    fclose(output);

    return result;
}
