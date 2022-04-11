#include <pthread.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct{
    char* filename;
    pthread_mutex_t* mutex;
} sortNumbersInFileArgs;

int getLengthOfFile(FILE* fd){
   fseek(fd, 0, SEEK_END);
   int len = ftell(fd);
   fseek(fd, 0, SEEK_SET);
   return len;
}

int cmpr(const void* a, const void* b){
    return *(int32_t *)a-*(int32_t *)b;
}

int sortNumbersInFile(sortNumbersInFileArgs* args){
    FILE* fd = fopen(args->filename, "rb");
    // int len = getLengthOfFile(fd);
    
    int32_t numbers[100];
    fread(numbers, sizeof(numbers), 1, fd);
    qsort(numbers, sizeof(int32_t), 100, cmpr);
    char ofname[] = {args->filename[0], args->filename[1], '.', 'o', '\0'};  
    FILE* out = fopen(ofname, "w");
    for(int i=0;i<100;i++){
        fprintf(out, "%d\n", numbers[i]);
    }
    fclose(out);
    pthread_mutex_lock(args->mutex);
    pthread_mutex_unlock(args->mutex);
    fclose(fd);
}

int main(int argc, char** argv){
    FILE* fd;
    int number_of_files = argc - 1;
    pthread_t* threads = (pthread_t*) malloc(number_of_files * sizeof(pthread_t));
    sortNumbersInFileArgs* argss = (sortNumbersInFileArgs*) malloc(number_of_files * sizeof(sortNumbersInFileArgs));
    pthread_mutex_t console_mutex;
    pthread_mutex_init(&console_mutex, NULL); 
    for(int i=0;i<number_of_files; i++){
        argss[i].filename = argv[i+1];;;;;;;;;;;;;
        argss[i].mutex = &console_mutex;
        pthread_create(threads + i, NULL, (void *) sortNumbersInFile, &argss[i]);
    }

    for(int i=0;i<number_of_files; i++){
        pthread_join(threads[i], NULL);
    }
    // for(int i=1;i<argc; i++){
    //     fd = fopen(argv[i], "wb");
    //     int32_t numbers[100];
    //     for(int j=0; j<100; j++){
    //         numbers[i] = (int32_t) i;//rand();
    //     }
    //     fwrite(numbers, sizeof(numbers), 1, fd);
    //     fclose(fd);
    // }
    
    
    pthread_mutex_destroy(&console_mutex);
    free(threads);
    free(argss);
}