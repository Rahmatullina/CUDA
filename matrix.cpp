#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

__global__ void sum(int*A,int*B,int*C,int*size){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    if (i < size[0] && j<size[1]){
        int index = i*size[1]+j;
        C[index]=A[index]+B[index];
    }
}
void print(int** A, int* size){
    for(int i=0;i<size[0];i++){
        for(int j=0;j<size[1];j++)
            cout<< A[i][j] << " ";
        cout<< "\n";    
    }   
}
int main(){
    srand(time(NULL));
    int size[2] = {64,128};
    
    int** A=new int*[size[0]];
    int** B=new int*[size[0]];
    int** C=new int*[size[0]];
    A[0] = new int[size[0] * size[1]];
    B[0] = new int[size[0] * size[1]];
    C[0] = new int[size[0] * size[1]];

    for (int i = 1; i < size[0]; ++i) {
        A[i] = A[i - 1] + size[1];
        B[i] = B[i - 1] + size[1];
        C[i] = C[i - 1] + size[1];
    }
    for(int i=0;i<size[0];i++){
        for(int j=0;j<size[1];j++){
            A[i][j]=rand()%50;
            B[i][j]=rand()%50;
            C[i][j]=rand()%50;
        }
    }

    int* array = new int[2];
    array[0]=3;array[1]=3;
    print(A,array);
    print(B,array);

    int* dev_A,*dev_B,*dev_C;
    int sizeb = sizeof(int)*size[0]*size[1];
    //выделение память на устройстве для массивов  A, B, C.
    cudaMalloc((void**)&dev_A,sizeb);
    cudaMalloc((void**)&dev_B,sizeb);
    cudaMalloc((void**)&dev_C,sizeb);

    //копирование даннных на устройство
    cudaMemcpy(dev_A,A[0],sizeb,cudaMemcpyHostToDevice);
    cudaMemcpy(dev_B,B[0],sizeb,cudaMemcpyHostToDevice);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    float time;
  
    int M[4]={2,4,8,16};

    for(int i=0;i<4;i++){
        dim3 blocks(size[0]/M[i],size[1]/M[i]);
        dim3 threads(M[i],M[i]);

        cudaEventRecord(start, 0);
        sum<<<blocks,threads>>>(dev_A,dev_B,dev_C,size);
        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&time, start, stop);
        cout << "Elapsed time for work wth " << size[0]/M[i]<<"x"<< size[1]/M[i] << "blocks and " << M[i]<<"x"<< M[i]<< "threads : " << time << "\n";
        cudaDeviceSynchronize();
        cudaMemcpy(C, dev_C, sizeb, cudaMemcpyDeviceToHost);
        print(C,array);
    }
    
    cudaFree(dev_A);
    cudaFree(dev_B);
    cudaFree(dev_C);
    delete[] A[0];
    delete[] B[0];
    delete[] C[0];
    delete[] A;
    delete[] B;
    delete[] C;

    return 0;

}