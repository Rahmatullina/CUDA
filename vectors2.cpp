#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

__global__ void sum_vectors(int*A,int*B,int*C,int size){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < size)
        C[i]=A[i]+B[i];
}
void print(int* a, int n){
    for(int i=0;i<n;i++)
        cout<< a[i] << " ";
    cout << "\n";    
}
int main(){
    int size = 1024;
    
    int* A=new int[size];
    int* B=new int[size];
    int* C=new int[size];
    for(int i=0;i<size;i++){
        A[i]=rand()%50;
        B[i]=rand()%50;
        C[i]=rand()%50;
    }

    print(A,5);
    print(B,5);

    int* dev_A,*dev_B,*dev_C;
    int sizeb = sizeof(int)*size;
    //выделение память на устройстве для массивов  A, B, C.
    cudaMalloc((void**)&dev_A,sizeb);
    cudaMalloc((void**)&dev_B,sizeb);
    cudaMalloc((void**)&dev_C,sizeb);

    //копирование даннных на устройство
    cudaMemcpy(dev_A,A,sizeb,cudaMemcpyHostToDevice);
    cudaMemcpy(dev_B,B,sizeb,cudaMemcpyHostToDevice);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    float time;
    
    int M[5]={4,8,16,32,64};
    for(int i=0;i<5;i++){
        cudaEventRecord(start, 0);
        sum_vectors<<<size/M[i],M[i]>>>(dev_A,dev_B,dev_C,size);
        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&time, start, stop);
        cout << "Elapsed time for work wth " << size/M[i] << "blocks and " << M[i] << "threads : " << time << "\n";
        cudaDeviceSynchronize();
        cudaMemcpy(C, dev_C, sizeb, cudaMemcpyDeviceToHost);
        print(C,5);
    }


   
    cudaFree(dev_A);
    cudaFree(dev_B);
    cudaFree(dev_C);

    return 0;

}