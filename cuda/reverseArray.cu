#include <stdio.h>
#include <assert.h>

#define THREADS_PER_BLOCK 768
#define ARRAY_SIZE THREADS_PER_BLOCK * 1024
#define OPTIM 0

static void HandleError(cudaError_t error, const char *file, int line) {
    if (error != cudaSuccess) {
        printf("%s in %s at line %d\n", cudaGetErrorString(error), file, line);
        exit( EXIT_FAILURE );
    }
}

#define HANDLE_ERROR(err) (HandleError(err, __FILE__, __LINE__ ))

__global__ void reverseArray(int *inArray, int *outArray)
{
    int inOffset = blockDim.x * blockIdx.x;

//		int inIndex = inOffset + blockIdx.x;
//                             ^
//                             print'ujac inIndex zauwazylem w gdb, ze
//                             w obrebie bloku z threadu na thread nie
//                             zmienienia sie jego wartosc, a co wiecej:
//                             wychodzimy poza adres pamieci (cuda-memcheck)
//                             np. dla bloku 1023.
//														

		int inIndex = inOffset + threadIdx.x; 
//                            ^ poprawne przesuniecie

//    int outOffset = blockDim.x * (gridDim.x - 1 - blockIdx.x);
//		int outIndex = outOffset + (blockDim.x - 1 - blockIdx.x);
//                                                  ^ analogicznie jak wyzej.
		int outOffset = blockDim.x * (gridDim.x - 1 - blockIdx.x);
		int outIndex = outOffset + (blockDim.x - 1 - threadIdx.x);

    outArray[outIndex] = inArray[inIndex];
}

__global__ void reverseArrayOptim(int *inArray, int *outArray) {
		__shared__ int shared_memory[THREADS_PER_BLOCK];
    int inOffset = blockDim.x * blockIdx.x;
		int inIndex = inOffset + threadIdx.x; 
		shared_memory[blockDim.x - 1 - threadIdx.x] = inArray[inIndex];
//                                   ^ reverse
		__syncthreads();
		int outOffset = blockDim.x * (gridDim.x - 1 - blockIdx.x);
		int outIndex = outOffset + threadIdx.x; 
    outArray[outIndex] = shared_memory[threadIdx.x];
}

void print_vec(int n, int *array) {
	printf(" array =  { ");
	for (int i = 0; i < n; ++i) {
		printf("%d, ", array[i]);
	}
	printf("}\n");
}

int main(void)
{
    int *hostArray;
    int *devInArray, *devOutArray;
    int numBlocks = ARRAY_SIZE / THREADS_PER_BLOCK;
    size_t memSize = ARRAY_SIZE * sizeof(int);
    hostArray = (int *)malloc(memSize);
		for (int i = 0; i < ARRAY_SIZE; i++) {
        hostArray[i] = i;
    }

		cudaEvent_t start, stop;
		cudaEventCreate(&start);
		cudaEventCreate(&stop);

		cudaEventRecord(start);

    HANDLE_ERROR(cudaMalloc((void **)&devInArray, memSize));
    HANDLE_ERROR(cudaMalloc((void **)&devOutArray, memSize));
    HANDLE_ERROR(cudaMemcpy(devInArray, hostArray, memSize, cudaMemcpyHostToDevice));
    dim3 dimGrid(numBlocks);
    dim3 dimBlock(THREADS_PER_BLOCK);

		#if OPTIM
    	reverseArrayOptim<<<dimGrid, dimBlock>>>  (devInArray, devOutArray);
		#else 
    	reverseArray<<<dimGrid, dimBlock>>>  (devInArray, devOutArray);
		#endif

    HANDLE_ERROR(cudaMemcpy(hostArray, devOutArray, memSize, cudaMemcpyDeviceToHost));

		cudaEventRecord(stop);
		cudaEventSynchronize(stop);

		float exec_time = 0;

		cudaEventElapsedTime(&exec_time, start, stop);
		#if OPTIM
			printf("[GPU/OPTIM] Execution time (ms): %3.1f\n", exec_time);
		#else 
			printf("[GPU] Execution time (ms): %3.1f\n", exec_time);
		#endif

    for (int i = 0; i < ARRAY_SIZE; i++) {
        assert(hostArray[i] == ARRAY_SIZE - 1 - i);
    }

    HANDLE_ERROR(cudaFree(devInArray));
    HANDLE_ERROR(cudaFree(devOutArray));

    free(hostArray);

    printf("Correct!\n");
    return 0;
}

