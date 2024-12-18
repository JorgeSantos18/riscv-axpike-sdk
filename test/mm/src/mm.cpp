#include <stdio.h>
#include <stdlib.h>
#include <numa.h>
#include <numaif.h>
#include <fcntl.h>
#include <csignal>

#ifndef DATATYPE
#define DATATYPE int
#endif

#define NUMA_NODE 1

int main(int argc, char* argv[]) {
  int size;
  DATATYPE *A, *B, *C;
  int i, j, k;

  scanf("%d", &size);

  //Allocate memory for input matrices
  // A = calloc(size * size, sizeof(DATATYPE));
  A = (DATATYPE * ) numa_alloc_onnode(size * size * sizeof(DATATYPE), NUMA_NODE);
  memset(A, 0, size * size * sizeof(DATATYPE));
  // B = calloc(size * size, sizeof(DATATYPE));
  B = (DATATYPE * ) numa_alloc_onnode(size * size * sizeof(DATATYPE), NUMA_NODE);
  memset(B, 0, size * size * sizeof(DATATYPE));

  //Allocate memory for resulting matrix
  // C = calloc(size * size, sizeof(DATATYPE));
  C = (DATATYPE * ) numa_alloc_onnode(size * size * sizeof(DATATYPE), NUMA_NODE);
  memset(C, 0, size * size * sizeof(DATATYPE));

  // Read input A
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      scanf("%d", &(A[i*size + j]));
    }
  }

  // Read input B
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      scanf("%d", &(B[i*size + j]));
    }
  }

  // Multiply
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      C[i*size + j] = 0;
      for (k = 0; k < size; k++) {
        C[i*size + j] += A[i*size + k] * B[k*size + j];
      }
    }
  }


  raise(SIGTSTP);

  // Print output C
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      printf("%d\n", C[i*size + j]);
    }
  }

  raise(SIGTSTP);

  return 0;
}
