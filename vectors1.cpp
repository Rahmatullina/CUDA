#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

__global__ void sum_vectors(int*A,int*B,int*C,int size){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < size)
        C[i]=A[i]+B[i];
}
__global__ void sum_on_1_1(int*A,int*B,int*C,int size){
    for(int i=0;i < size;i++)
        C[i]=A[i]+B[i];
}
void print(int* a, int n){
    for(int i=0;i<n;i++)
        cout<< a[i] << " ";
    cout << "\n";    
}
int main(){
    srand(time(NULL))
    int size = 24;
    
    int* A=new int[size];
    int* B=new int[size];
    int* C=new int[size];
    for(int i=0;i<size;i++){
        A[i]=rand()%50;
        B[i]=rand()%50;
        C[i]=rand()%50;
    }

    print(A,size);
    print(B,size);

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

    cudaEventRecord(start, 0);
    sum_on_1_1<<<1,1>>>(dev_A,dev_B,dev_C,size);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);
    cout << "Elapsed time for work wth 1 block and 1 thread : " << time << "\n";
    cudaDeviceSynchronize();
    cudaMemcpy(C, dev_C, sizeb, cudaMemcpyDeviceToHost);
    print(C,size);

    cudaEventRecord(start, 0);
    sum_vectors<<<size,1>>>(dev_A,dev_B,dev_C,size);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);
    cout << "Elapsed time for work wth " << size << " blocks and 1 thread : " << time << "\n";
    cudaDeviceSynchronize();
    cudaMemcpy(C, dev_C, sizeb, cudaMemcpyDeviceToHost);
    print(C,size);

    cudaEventRecord(start, 0);
    sum_vectors<<<1,size>>>(dev_A,dev_B,dev_C,size);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);
    cout << "Elapsed time for work wth 1 block and " << size << " threads : " << time << "\n";
    cudaDeviceSynchronize();
    cudaMemcpy(C, dev_C, sizeb, cudaMemcpyDeviceToHost);
    print(C,size);

   
    cudaFree(dev_A);
    cudaFree(dev_B);
    cudaFree(dev_C);

    return 0;

}