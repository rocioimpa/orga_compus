#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER 100
#define ERROR_INVALID_PARAMETERS 0
#define ERROR_INVALID_INPUT_FILE 1
#define ERROR_EMPTY_INPUT_FILE 2
#define ERROR_INVALID_OUTPUT_FILE 3

long getNumberOfWords(FILE*);
void writeOutput(char**,long,char*);
void freeArray(char**,long);
void showHelp();
void showVersion();
void showError(int);

int main(int argc, char *argv[]){
	if (argc == 2){
		if (((strcmp(argv[1], "-V") == 0)) || ((strcmp(argv[1], "--version") == 0))){
			showVersion();
		} else if (((strcmp(argv[1], "-h") == 0)) || ((strcmp(argv[1], "--help") == 0))){
			showHelp();
		} else {
			showError(ERROR_INVALID_PARAMETERS);
		}
	} else if (argc == 6){
		int validFile = 1;
		if (((strcmp(argv[2], "-i") == 0)||((strcmp(argv[2], "--input") == 0))) && ((strcmp(argv[4], "-o") == 0) || (strcmp(argv[4], "--output") == 0))){
			FILE *inputFile;
			char str[BUFFER+1];
			char** array;
			long pos = 0;
			long numberOfWords = 0;
			inputFile = fopen(argv[3],"r");
			if (inputFile)
			{
				numberOfWords = getNumberOfWords(inputFile);
				fseek(inputFile, 0L, SEEK_END);
				long inputFileSize = ftell(inputFile);
				if(inputFileSize > 0){
					fseek(inputFile, 0L, SEEK_SET);
					array = malloc((numberOfWords+1)*sizeof(char*));
						char* p;
						char* text = (char *) malloc(sizeof(char) * inputFileSize + 1);
						memset(text,'\0', inputFileSize);

						int i;
						for (i = 0; i < numberOfWords-1; i++)
						{
							array[i] = NULL;
						}

						while(fgets(str, sizeof(str), inputFile)!= NULL)
						{
							char aWord[BUFFER+1];
							memset(aWord,'\0', BUFFER);
							char *temp = malloc(sizeof(char)*BUFFER+1);
							memset(temp,'\0', BUFFER);
							int index;
							int j;
							char aChar;
							for (index = 0, j = 0; index < strlen(str); index++)
							{
									aChar = str[index];
									if (aChar != 39)
									{
										temp[j] = tolower(aChar);
      							j++;
									}
							}
							temp[j] = '\0';
							for (index = 0; index < strlen(temp); index++)
							{
								aChar = temp[index];
								if (((aChar > 0) && (aChar < 97)) || (aChar >= 123))
								{
									temp[index] = '|';
								}
							}
							strcpy(aWord, temp);
							free(temp);
							strcat(text, aWord);
						}
						char* word;
						p = strtok(text,"|");
						while(p != NULL)
						{
							word = malloc((strlen(p) + 1)*sizeof(char));
							memset(word,'\0', strlen(word));
							strcpy(word, p);
							array[pos] = word;
							p = strtok(NULL, "|");
							pos++;
						}
						free(text);
						fclose(inputFile);
						printf("%s\n", "Words have been saved successfully into an array");
				} else {
					showError(ERROR_EMPTY_INPUT_FILE);
					validFile = 0;
				}
			}
			else {
				showError(ERROR_INVALID_INPUT_FILE);
				validFile = 0;
			}
			if(validFile == 1){
				printf("%s\n", "Start of sorting");
				if ((strcmp(argv[1], "-b") == 0) || (strcmp(argv[1], "--bsort") == 0)){
					bubbleSort(array, pos);
					writeOutput(array,pos,argv[5]);
				} else if ((strcmp(argv[1], "-q") == 0) || (strcmp(argv[1], "--qsort") == 0)){
					quickSort(array, pos);
					writeOutput(array,pos,argv[5]);
				} else {
					showError(ERROR_INVALID_PARAMETERS);
				}
				freeArray(array,numberOfWords);
			}
		} else {
			showError(ERROR_INVALID_PARAMETERS);
		}
	} else {
		showError(ERROR_INVALID_PARAMETERS);
	}
	return 0;
}

long getNumberOfWords(FILE *aFile){
	char aChar;
	long numberOfWords = 0;
		while ((aChar = fgetc(aFile)) != EOF){
    		if((aChar == 10) || (aChar == 32)){
					numberOfWords++;
				}
    }
		return numberOfWords;
}

void writeOutput(char** array, long pos, char* fileName){
	FILE *outputFile = fopen(fileName,"w+");

	if(outputFile != NULL){
		printf("%s\n", "Writing output file");
		long i;
		for (i = 0; i < pos; i++)
		{
			fputs(array[i], outputFile);
			fputs("\n", outputFile);
		}
		fclose(outputFile);
		printf("%s\n", "Sorting finished successfully. Execution finished.");
	} else {
		showError(ERROR_INVALID_OUTPUT_FILE);
	}
}

void freeArray(char** array,long numberOfWords){
	long i;
	for (i = 0; i < numberOfWords+1; i++) {
		free(array[i]);
	}
	free(array);
}

void showHelp(){
	printf("%s\n", "TP0 Organizacion de computadoras - HELP");
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

void showVersion(){
	printf("%s\n", "TP0 Organizacion de computadoras - VERSION: 1.0");
}

void showError(int errorCode){
	if(errorCode == ERROR_INVALID_INPUT_FILE){
		printf("%s\n","Invalid input file. Type 'tp0 -h' for help. Program terminated");
	}
	if(errorCode == ERROR_EMPTY_INPUT_FILE){
		printf("%s\n", "Input file is empty. Type 'tp0 -h' for help. Program terminated");
	}
	if(errorCode == ERROR_INVALID_OUTPUT_FILE){
		printf("%s\n", "Invalid output file, check path. Type 'tp0 -h' for help. Program terminated");
	}
	if(errorCode == ERROR_INVALID_PARAMETERS){
		printf("%s\n","Invalid arguments. Type 'tp0 -h' for help. Program terminated");
	}
}
