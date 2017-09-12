#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#define BUFFER 100
#define ERROR_INVALID_PARAMETERS 0
#define ERROR_INVALID_INPUT_FILE 1
#define ERROR_EMPTY_INPUT_FILE 2
#define ERROR_INVALID_OUTPUT_FILE 3
#define ERROR_NO_TEXT_GIVEN 4

/*------------------------------ Array dinámico -----------------------------*/
typedef struct {
    char **array;
    size_t used;
    size_t size;
} Array;

void* Malloc(size_t size){
	void* ptr = malloc(size);
	if (!ptr){
		fprintf(stderr, "Falló malloc: %s\n",strerror(errno));
	}
	return ptr;
}

void* Realloc(void* ptr, size_t size){
	ptr = realloc(ptr,size);
	if (!ptr){
		fprintf(stderr, "Falló realloc: %s\n",strerror(errno));
	}
	return ptr;
}

int initArray(Array * a, size_t initialSize) {
    a->array = (char **)Malloc(initialSize * sizeof(char *));
    if (a->array == NULL){
		a->size = -1;
		return -1;
	}
    a->used = 0;
    a->size = initialSize;
    return 0;
}

int insertArray(Array *a, char * element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size++;
        a->array = (char **)Realloc(a->array, a->size * sizeof(char *));
        if (a->array == NULL){
			//Falló realloc, ya se escribió el error en stderr. Devuelvo -1 para cancelar la ejecución.
			return -1;
		}
    }
    a->array[a->used++] = element;
    return 0;
}

void freeDynamicArray(Array *a) {
	for (int i = 0; i < a->used; i++){
		free(a->array[i]);
	}
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void freeDynamicArrayResultado(Array *a){
	free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

typedef struct {
    char *word;
    size_t used;
    size_t size;
} DynamicWord;

int initDynamicWord(DynamicWord * a, size_t initialSize) {
    a->word = (char *)Malloc(initialSize * sizeof(char));
    if (a->word == NULL){
		return -1;
	}
    a->used = 0;
    a->size = initialSize;
    return 0;
}

int insertChar(DynamicWord *a, int element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size++;
        a->word = (char *)Realloc(a->word, a->size * sizeof(char));
        if (a->word == NULL){
			return -1;
		}
    }
    a->word[a->used++] = element;
    return 0;
}

void freeDynamicWord(DynamicWord *a) {
    free(a->word);
    a->word = NULL;
    a->used = a->size = 0;
}
/*------------------------------------Array dinamico----------------------------*/


int writeOutput(char**,long,char*);
void writeStandardOutput(char**, long);
void freeArray(char**,long);
void showHelp();
void showVersion();
void showError(int);
int wordIsPalindrome(char*);
int getWordLength(char*);
char* convertWordToLowerCase(char*);
char * searchArgumentValue(char**, int, char*, char*);
int validateArgumentAppeareance(char**, int, char*, char*);
Array getFromStandardInput();
Array getWordsFromFile(FILE* file);
size_t getFileSize(FILE* file);
Array arrayOfWordsCorrectlyInitialized(int, Array);
DynamicWord wordCorrectlyInitialized(int, DynamicWord);
Array wordCorrectlyInserted(int, DynamicWord, Array);
void printStoredWords(Array);
int isSeparator(int);

int main(int argc, char *argv[]){

    char* input = NULL;
    char* output = NULL;
    int validFile = 1;

    if(argc > 1){
        if (validateArgumentAppeareance(argv, argc, "-v", "--version")){
			showVersion();
            return 0;
		} else if (validateArgumentAppeareance(argv, argc, "-h", "--help")){
			showHelp();
            return 0;
		}
        input = searchArgumentValue(argv, argc, "-i", "--input");
        output = searchArgumentValue(argv, argc, "-o", "--output");
    }

    Array array;
    if(!input){
        array = getFromStandardInput();
        if (array.size == -1){
            //Ocurrió un error al recibir por stdin, debo cancelar la ejecución
            return -1;
        }
        if(array.size == 0){
            showError(ERROR_NO_TEXT_GIVEN);
            return 0;
        }
        validFile = 1;
    } else {
        size_t inputFileSize;
        FILE *inputFile;
        int arrayOk = initArray(&array, 0);
        if (arrayOk < 0){ //Error del malloc del array. No hay memoria para liberar asique cancelo la ejecución.
            return 1;
        }
        inputFile = fopen(input,"rb+");
        printStoredWords(array);
        if (inputFile == NULL){ //Agregada corrección al manejo de errores. Ahora si da error revisa el errno para obtener el mensaje.
            fprintf(stderr, "Error al intentar abrir el archivo INPUT: %s.\n", strerror(errno));
            validFile = 0;
        } else {
            inputFileSize = getFileSize(inputFile);
            if (inputFileSize < 0){
                fprintf(stderr, "Error al intentar obtener el size del archivo INPUT: %s.\n", strerror(errno));
            } else {
                array = getWordsFromFile(inputFile);

                printStoredWords(array);
                int closeOk = fclose(inputFile);
                if (closeOk == EOF){ //Agregada corrección al manejo de errores. Ahora si da error revisa el errno para obtener el mensaje.
                    fprintf(stderr, "Error al intentar cerrar el archivo INPUT: %s.\n", strerror(errno));
                }
            }
        }
    }

    if(validFile == 1){
        Array result;
        int arrayOk = initArray(&result, 0);
        if (arrayOk < 0){
            freeDynamicArray(&array);
            return -1;
        }
        for(size_t i = 0; i < array.size; i++){
            if(wordIsPalindrome(array.array[i])){
                insertArray(&result,array.array[i]);
            }
        }
        writeOutput(result.array, result.size, output);
        freeDynamicArrayResultado(&result);
        freeDynamicArray(&array);
    } else {
        showError(ERROR_INVALID_PARAMETERS);
    }

    return 0;
}


int writeOutput(char** array, long pos, char* fileName){
    if(fileName){
        FILE *outputFile = fopen(fileName,"w+");
        if (outputFile == NULL){
			//Agregada corrección al manejo de errores. Ahora si da error revisa el errno para obtener el mensaje.
			fprintf(stderr, "Error al intentar abrir el archivo OUTPUT: %s.\n", strerror(errno));
			return -1;
		}
        else{
            long i;
            int error = 0;
            for (i = 0; i < pos; i++)
            {
                int bytesWritten = fputs(array[i], outputFile);
                if (bytesWritten < 0){
					fprintf(stderr, "Error al intentar escribir en el archivo OUTPUT: %s.\n", strerror(errno));
					error = 1;
					break;
				}
                bytesWritten = fputs("\n", outputFile);
                if (bytesWritten < 0){
					fprintf(stderr, "Error al intentar escribir en el archivo OUTPUT: %s.\n", strerror(errno));
					error = 1;
					break;
				}
            }
            int closeOk = fclose(outputFile);
            if (closeOk == EOF){
				//Agregada corrección al manejo de errores. Ahora si da error revisa el errno para obtener el mensaje.
				fprintf(stderr, "Error al intentar cerrar el archivo OUTPUT: %s.\n", strerror(errno));
				return -1;
			}
			if (error == 1){
				return -1;
			}
        }
    } else {
        writeStandardOutput(array, pos);
    }
    return 0;
}

void writeStandardOutput(char ** result, long resultLength){
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
		fprintf(stderr,"%s\n","Invalid input file. Type 'tp0 -h' for help. Program terminated");
	}
	if(errorCode == ERROR_EMPTY_INPUT_FILE){
		fprintf(stderr,"%s\n", "Input file is empty. Type 'tp0 -h' for help. Program terminated");
	}
	if(errorCode == ERROR_INVALID_OUTPUT_FILE){
		fprintf(stderr,"%s\n", "Invalid output file, check path. Type 'tp0 -h' for help. Program terminated");
	}
	if(errorCode == ERROR_INVALID_PARAMETERS){
		fprintf(stderr,"%s\n","Invalid arguments. Type 'tp0 -h' for help. Program terminated");
	}
    if(errorCode == ERROR_NO_TEXT_GIVEN){
        fprintf(stderr,"%s\n", "No text passed to verify");
    }
}

int wordIsPalindrome(char * word){
	int numberOfLetters;
	numberOfLetters = getWordLength(word);
    for(int i = 0; i < numberOfLetters; i++){
		if(tolower(word[i]) != tolower(word[numberOfLetters - i - 1])){
			return 0;
		}
	}

	return 1;
}

int getWordLength(char * word){
	int i = 0;
	while(word[i]){
		i++;
	}
	return i;
}

//1 if the argument is present, 0 if not
int validateArgumentAppeareance(char** argv, int arg, char* arg1, char* arg2){
    for(int i = 1; i < arg; i++){
        if((strcmp(argv[i], arg1) == 0)||(strcmp(argv[i], arg2) == 0)){
            return 1;
        }
    }
    return 0;
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

Array getWordsFromFile(FILE* inputFile){
  DynamicWord currentWord;
  Array storedWords;
  int arrayOk = initArray(&storedWords,0);
  if(arrayOk < 0){
    return storedWords;
  }

  int wordOk = initDynamicWord(&currentWord,0);
  if(wordOk < 0){
    freeDynamicWord(&currentWord);
    currentWord.size = -1;
    return storedWords;
  }

  int currentCharacter;

  while ((currentCharacter = fgetc(inputFile)) > 0){
    printf("%i\n", currentCharacter);
    if(currentCharacter < 0){
      fprintf(stderr,"Error al leer del archivo: %s.\n",strerror(errno));
      freeDynamicArray(&storedWords);
      freeDynamicWord(&currentWord);
      storedWords.size = -1;
      return storedWords;
    }
    if(isSeparator(currentCharacter) == 1){
        char * aWord = (char *)Malloc(sizeof(char) * currentWord.size);
        strncpy(aWord, currentWord.word, currentWord.size);
        insertArray(&storedWords, aWord);
        freeDynamicWord(&currentWord);
        int wordOk = initDynamicWord(&currentWord, 0);
        if(wordOk < 0){
          freeDynamicWord(&currentWord);
          storedWords.size = -1;
          return storedWords;
        }
    } else {
        int insertionOk = insertChar(&currentWord, currentCharacter);
        if(insertionOk < 0){
          freeDynamicArray(&storedWords);
          freeDynamicWord(&currentWord);
          storedWords.size = -1;
          return storedWords;
        }
    }
  }
  return storedWords;
}

int isSeparator(int character){
 	if (((character > 0) && (character < 45)) || ((character > 45) && (character < 48)) || ((character > 57) && (character < 65))  || ((character > 90) && (character < 95)) || ((character > 95) && (character < 97)) || (character >= 123))
 		return 1;
 	else
 		return 0;
}

void printStoredWords(Array storedWords){
  for(size_t i = 0; i < storedWords.size; i++){
    printf("%s\n", storedWords.array[i]);
  }
}

Array getFromStandardInput(){
    DynamicWord auxInput;
    Array words;
    int arrayOk = initArray(&words, 0);
    if (arrayOk < 0){
		//Falló el inicio del Array.
		return words; //Devuelvo words con size = -1
	}
    int wordOk = initDynamicWord(&auxInput, 0);
    if (wordOk < 0){
		//Falló el dynamicword
		freeDynamicArray(&words);
		words.size = -1;
		return words;  //Devuelvo words con size = -1
	}
    char ch;
    // 0 = standar input, 1 tamanio buffer
    struct pollfd mypoll = {STDIN_FILENO, POLLIN|POLLPRI};
    if(poll(&mypoll, 1, 1)){
        while(read(0, &ch, 1) > 0){
            if((ch == 32) || (ch == 10)){
                char * auxString = (char *)Malloc(sizeof(char) * auxInput.size);
                strncpy(auxString, auxInput.word, auxInput.size);
                insertArray(&words, auxString);
                freeDynamicWord(&auxInput);
                int wordOk = initDynamicWord(&auxInput, 0);
				if (wordOk < 0){
					//Falló el dynamicword
					freeDynamicArray(&words);
					words.size = -1;
					return words;  //Devuelvo words con size = -1
				}
            } else {
                int insertOk = insertChar(&auxInput, ch);
                if (insertOk < 0){
					//Error del realloc.
					freeDynamicArray(&words);
					freeDynamicWord(&auxInput);
					words.size = -1;
					return words;  //Devuelvo words con size = -1
				}
            }
        }
		if (ch < 0){
			//Error al leer de stdin.
			fprintf(stderr,"Error al leer de stdin: %s.\n",strerror(errno));
			freeDynamicArray(&words);
            freeDynamicWord(&auxInput);
			words.size = -1;
			return words;  //Devuelvo words con size = -1
		}
    }
    return words;
}

size_t getFileSize(FILE* file)
{
	size_t pos = ftell(file);    // Current position
	if (pos < 0){
		return -1;
	}
	size_t fseekok = fseek(file, 0, SEEK_END);    // Go to end
	if (fseekok < 0){
		return -1;
	}
	size_t length = ftell(file); // read the position which is the size
	if (length < 0){
		return -1;
	}
	fseekok = fseek(file, pos, SEEK_SET);  // restore original position
	if (fseekok < 0){
		return -1;
	}
	return length;
}
