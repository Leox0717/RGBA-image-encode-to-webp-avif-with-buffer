#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <webp/encode.h>
#include <string.h>


void *rgba2webp(void* arg){
    int thread_num = *(int *)arg;
    
    uint8_t *rgba = (uint8_t *) malloc(15360*2160*4);
    FILE *pd = NULL;
    pd = fopen("rgba15360x2160.out", "rb");
    FILE *opFile;
    float quality_factor = 50;

    uint8_t* output;

    fread(rgba, sizeof(uint8_t), 15360*2160*4, pd);

    size_t size = WebPEncodeBGRA(rgba, 15360, 2160, 15360*4, quality_factor, &output);

    printf("thread %d writing file out\n", thread_num);
    
    char save_name[256] = {0};
    sprintf(save_name, "thread%d.webp", thread_num);

    opFile=fopen(save_name,"w");
    // opFile=fopen("output.webp","w");
    fwrite(output,1,(int)size,opFile);
}

    
//  usage:
//  ./webpThread [thread_num]  default:thread_num=1 
int main(int argc,char *argv[]){
    
    if(argc > 2){
        printf("参数 error\n");
        return 1;
    }

    struct timespec time1 = {0, 0}; 
    struct timespec time2 = {0, 0};

    int num_of_thread=1;
    if(argc == 2){
        num_of_thread=atoi(argv[1]);
    }

    //read image 
    uint8_t *rgba = (uint8_t *) malloc(15360*2160*4);
    FILE *pd = NULL;
    pd = fopen("rgba15360x2160.out", "rb");
    FILE *opFile;
    float quality_factor = 60;
    fread(rgba, sizeof(uint8_t), 15360*2160*4, pd);

    clock_gettime(CLOCK_REALTIME, &time1);      

    pthread_t tid[num_of_thread];
    for(int i = 0;i < num_of_thread;i++){
        // para thread_para;
        // thread_para.thread_num = i;
        // thread_para.
        pthread_create(&tid[i],NULL,rgba2webp, &i);
    }
    for(int i=0;i<num_of_thread;i++){
        pthread_join(tid[i],NULL);//等待线程结束
    }

    clock_gettime(CLOCK_REALTIME, &time2);   
    float temp = (time2.tv_sec - time1.tv_sec)*1000 + (time2.tv_nsec - time1.tv_nsec) / 1000000;
    printf("Compress Time with %d threads = %f ms\n", num_of_thread, temp);
    return 0;

}
