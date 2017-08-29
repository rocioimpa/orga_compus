#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define BUFFER 100
#define ERROR_INVALID_PARAMETERS 0
#define ERROR_INVALID_INPUT_FILE 1
#define ERROR_EMPTY_INPUT_FILE 2
#define ERROR_INVALID_OUTPUT_FILE 3

/*------------------------------ Array dinámico -----------------------------*/
typedef struct {
    char **array;
    size_t used;
    size_t size;
} Array;

void initArray(Array * a, size_t initialSize) {
    a->array = (char **)malloc(initialSize * sizeof(char *));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, char * element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size++;
        a->array = (char **)realloc(a->array, a->size * sizeof(char *));
    }
    a->array[a->used++] = element;
}

void freeDynamicArray(Array *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

typedef struct {
    char *word;
    size_t used;
    size_t size;
} DynamicWord;

void initDymaicWord(DynamicWord * a, size_t initialSize) {
    a->word = (char *)malloc(initialSize * sizeof(char));
    a->used = 0;
    a->size = initialSize;
}

void insertChar(DynamicWord *a, char element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size++;
        a->word = (char *)realloc(a->word, a->size * sizeof(char));
    }
    a->word[a->used++] = element;
}

void freeDynamicWord(DynamicWord *a) {
    free(a->word);
    a->word = NULL;
    a->used = a->size = 0;
}
/*------------------------------------Array dinamico----------------------------*/


void writeOutput(char**,long,char*);
void freeArray(char**,long);
void showHelp();
void showVersion();
void showError(int);
Array findCapicuaWords(char**, long);
int wordIsPalindrome(char*);
int getWordLength(char*);
char* convertWordToLowerCase(char*);
void writeStadarOutput(char**, long);
char * searchArgumentValue(char**, int, char*, char*);
char* getFromStandardInput();
char removeDiacritic(char*);
size_t getFileSize(FILE* file);
void printArray(Array);
int isSeparator(int);


int main(int argc, char *argv[]){
	if (argc == 2){
		if (((strcmp(argv[1], "-V") == 0)) || ((strcmp(argv[1], "--version") == 0))){
			showVersion();
		} else if (((strcmp(argv[1], "-h") == 0)) || ((strcmp(argv[1], "--help") == 0))){
			showHelp();
		} else {
			showError(ERROR_INVALID_PARAMETERS);
		}
	} else if (3 <= argc <= 5){
		int validFile = 1;
        char* input = searchArgumentValue(argv, argc, "-i", "--input");
        char* output = searchArgumentValue(argv, argc, "-o", "--output");

        if(!input){
            input = getFromStandardInput();
            if(!input){
                showError(ERROR_INVALID_INPUT_FILE);
                return 0;
            }
        }
		size_t inputFileSize;
		FILE *inputFile;
		char str[BUFFER+1];
		Array array;
		long pos = 0;
		long numberOfWords = 0;
		inputFile = fopen(input,"r");

		if (inputFile){
			inputFileSize = getFileSize(inputFile);
			if(inputFileSize > 0){
				char* p;
				char* text = (char *) malloc(sizeof(char) * inputFileSize + 1);
				memset(text,'\0', inputFileSize);
				while(fgets(str, sizeof(str), inputFile)!= NULL)
				{
					int index;
					char aChar;
					for (index = 0; index < strlen(str); index++){
						aChar = str[index];
						if (isSeparator(aChar) == 1)
            {
							str[index] = '|';
						}
					}
					strcat(text,str);
				}

				char* word;
				p = strtok(text,"|");
				while(p != NULL)
				{
					word = malloc((strlen(p) + 1)*sizeof(char));
					memset(word,'\0', strlen(word));
					strcpy(word, p);
          insertArray(&array, word);
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
			} else {
				showError(ERROR_INVALID_INPUT_FILE);
				validFile = 0;
			}

			if(validFile == 1){
				printf("%s\n", "Going to validate capicua words");
        printArray(array);
        Array result;
				//for(int i = 0; i < array.size; i++){
					//if(wordIsPalindrome(array.array[i])){
						//insertArray(&result,array.array[i]);
						//printf("%s es PALINDROMO\n",array.array[i]);
					//}
				//}

				//result = findCapicuaWords(array.array, array.size); //Adentro de esta funcion da el seg fault. No le gusta cuando hace initArray(&output, 0)

				/*writeOutput(result.array, result.size, output);
				freeDynamicArray(&result);
                freeDynamicArray(&array);                */
			}
        } else {
	           showError(ERROR_INVALID_PARAMETERS);
        }
	return 0;
}

int isSeparator(int character){
  if (((character > 0) && (character < 48)) || ((character > 57) && (character < 65))  || ((character > 90) && (character < 97)) || (character >= 123))
    return 1;
  else return 0;
}

void printArray(Array array){
  printf("%s\n","Printing array");
  for (int i = 0; i < array.size; i++) {
    printf("%s",array.array[i]);
    printf("%s\n","\n");
  }
}


void writeOutput(char** array, long pos, char* fileName){
    printf("%s\n", "Writing output");
    if(fileName){
        FILE *outputFile = fopen(fileName,"w+");

        if(outputFile != NULL){
            long i;
            for (i = 0; i < pos; i++)
            {
                fputs(array[i], outputFile);
                fputs("\n", outputFile);
            }
            fclose(outputFile);
        } else {
            showError(ERROR_INVALID_OUTPUT_FILE);
            return;
        }
    } else {
        writeStadarOutput(array, pos);
    }
    printf("%s\n", "Sorting finished successfully. Execution finished.");
}

void writeStadarOutput(char ** result, long resultLength){
    for(int i = 0; i < resultLength; i++){
        printf("%s\n", result[i]);
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

Array findCapicuaWords(char** array, long numberOfWords){
  Array output;
  //initArray(&output, 0); //Esta linea no le gusta
  /*
  for(int i = 0; i < numberOfWords; i++){
    if(wordIsPalindrome(array[i])){
      insertArray(&output, array[i]);
    }
  }*/
  return output;
}

int wordIsPalindrome(char * word){
	int numberOfLetters;
	numberOfLetters = getWordLength(word);

    if(numberOfLetters <= 1){
        return 0;
    }

    char* lowerCaseWord = convertWordToLowerCase(word);
    for(int i = 0; i < numberOfLetters; i++){
		if(lowerCaseWord[i] != lowerCaseWord[numberOfLetters - i - 1]){
			return 0;
		}
	}

    for(int i = 0; i < numberOfLetters; i++){
		if(word[i] != word[numberOfLetters - i - 1]){
			return 0;
		}
	}

	return 1;
}


char* convertWordToLowerCase(char*originalWord){
  char* lowerCaseWord;
  for(int i = 0; originalWord[i]; i++){
    //printf("%c\n", originalWord[i]);
    lowerCaseWord[i] = tolower(originalWord[i]);
    //lowerCaseWord[i] = removeDiacritic(lowerCaseWord[i]);
  }
  return lowerCaseWord;
}

//char removeDiacritic(char* letter){
//  printf("%s\n", letter);
//  switch ((char)letter) {
//    case 'à': letter = 'a'; break;
//    case 'è': letter = 'e'; break;
//    case 'é': letter = 'e'; break;
//    case 'ì': letter = 'i'; break;
//    case 'ò': letter = 'o'; break;
//    case 'ù': letter = 'u'; break;
//  }
//  return letter;
//}

int getWordLength(char * word){
	int i = 0;
	while(word[i]){
		i++;
	}
	return i;
}


char* searchArgumentValue(char** argv, int arg, char* arg1, char* arg2){
    for(int i = 1; i < arg; i++){
        if((strcmp(argv[i], arg1) == 0)||(strcmp(argv[i], arg2) == 0)){
            //45 == -, comparison to avoid getting empty arg
            if(argv[i+1] && argv[i+1][0] != 45){
                return argv[i+1];
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}

char* getFromStandardInput(){
    DynamicWord auxInput;
    initDymaicWord(&auxInput, 0);
    char ch;
    // 0 = standar input, 1 tamanio buffer
    while(read(0, &ch, 1) > 0){
        if(ch != 10){
            insertChar(&auxInput, ch);
        }
    }
    return auxInput.word;
}




size_t getFileSize(FILE* file)
{
	size_t pos = ftell(file);    // Current position
	fseek(file, 0, SEEK_END);    // Go to end
	size_t length = ftell(file); // read the position which is the size
	fseek(file, pos, SEEK_SET);  // restore original position
	return length;
}
