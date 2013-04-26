/* This program sorts a array with randomly generated
 * long integers into increasing sequence using quick sort
 * algorithm. User may also specified the number of threads
 * to to use to speed up the process.
 * 
 * Author: Zhengyi Liang
 *
 * Course: COMPT360 - Lab No.3
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _qs_data {
  long *arr;    /* array of long integers */
  long left;    /* left-most idx used in cur. inovation */
  long right;   /* right-most idx used in cur. inovation */
  int depth;    /* depth of recursion */
} qs_data;

/* Partition algorithm for quicksort */
int partition(long *data, long left, long right, int pivotIndex)
{
  long pivotValue, tempValue, storeIndex;
  int i;
  
  pivotValue = data[pivotIndex];    
  
  storeIndex = left;

  tempValue = data[pivotIndex];
  
  data[pivotIndex] = data[right];

  data[right] = tempValue;
  
  for (i = left; i < right; i++)
    {
      if (data[i] <= pivotValue)    /* if number smaller than the pivot, swap it with the one that's bigger */
	{
	  tempValue = data[storeIndex];
	  
	  data[storeIndex] = data[i];

	  data[i] = tempValue;

	  storeIndex++;    /* increment storeIndex to move forward */
	}
    }
  
  /* swap the pivot to the correct location */
  tempValue = data[storeIndex];   

  data[storeIndex] = data[right];

  data[right] = tempValue;
  
  return storeIndex;
}

void *quicksort(void* qsdata)
{
  long pivot, newPivot;
  qs_data firstQsdata, secondQsdata;
  int tCreateError, tKillError;
  pthread_t threadOne, threadTwo;

  /* initialize the firstQsdata structure to match the parameter that got passed in */
  firstQsdata.left = ((qs_data*) qsdata)->left;
  firstQsdata.right = ((qs_data*) qsdata)->right;
  firstQsdata.depth = ((qs_data*) qsdata)->depth;
  firstQsdata.arr = ((qs_data*) qsdata)->arr;

  /* initialize the secondQsdata structure to match the parameter that got passed in */
  secondQsdata.left = ((qs_data*) qsdata)->left;
  secondQsdata.right = ((qs_data*) qsdata)->right;
  secondQsdata.depth = ((qs_data*) qsdata)->depth;
  secondQsdata.arr = ((qs_data*) qsdata)->arr;
  
  if (firstQsdata.right > firstQsdata.left)
    {
      pivot = firstQsdata.left + ((firstQsdata.right - firstQsdata.left) / 2);

      newPivot = partition(firstQsdata.arr, firstQsdata.left, firstQsdata.right, pivot);

      if (((qs_data*) qsdata)->depth > 0)
	{/* decrement the depth number to control how many threads are going to be used */
	  firstQsdata.depth--;
	  secondQsdata.depth--;
	  
	  /* set the right indext */
	  firstQsdata.right = newPivot - 1;

	  /* create a thread of the first hafl of the data */
	  tCreateError = pthread_create(&threadOne, NULL, quicksort, &firstQsdata);
	  
	  /* if thread creation fails, terminate the program */
	  if (tCreateError != 0)
	    {
	      fprintf(stderr, "in quicksort(), pthread_create failed: ");
	      perror("");
	      exit(1);
	    }
	  
	  /* set the left index */
	  secondQsdata.left = newPivot + 1;
	  
	  /* create a thread of the second hafl of the data */
	  tCreateError = pthread_create(&threadTwo, NULL, quicksort, &secondQsdata);
	  
	  /* if thread creation fails, terminate the program */
	  if (tCreateError != 0)
	    {
	      fprintf(stderr, "in quicksort(), pthread_create failed: ");
	      perror("");
	      exit(1);
	    }

	  /* terminate threadOne */
	  tKillError = pthread_join(threadOne, NULL);

	  /* terminate program if error occurrs */
	  if (tKillError != 0)
	    {
	      fprintf(stderr, "int quicksort(), pthread_join failure: ");
	      perror("");
	      exit(1);
	    }
	  
	  /* terminate threadTwo */
	  tKillError = pthread_join(threadTwo, NULL);

	  /* terminate program if error occurrs */
	  if (tKillError != 0)
	    {
	      fprintf(stderr, "int quicksort(), pthread_join failure Line 137");
	      perror("");
	      exit(1);
	    }
	}
      else    /* if the depth of recursion is 0, stop using threads */
	{
	  firstQsdata.right = newPivot - 1;

	  quicksort(&firstQsdata);

	  secondQsdata.left = newPivot + 1;

	  quicksort(&secondQsdata);
	}
    }
  
  return(NULL);
}

int main (int argc, char** argv)
{
  qs_data *sortArray;
  long arraySize;
  int i;

  if (argc != 4)
    {
      fprintf(stderr, "Not enough/too many arguments, ./qs n s d\n");
      fprintf(stderr, "n: the number of random numbers to generate.\n");
      fprintf(stderr, "s: the seed to use for random number generator (RNG).\n");
      fprintf(stderr, "d: the depth to which the application creates new thread.\n");
    }
  else if(atoi(argv[1]) == 0 || atoi(argv[3]) < 0 || atoi(argv[3]) > 6)
    {
      fprintf(stderr, "n must be larger or equal to 1.\n");
      fprintf(stderr, "and d must be larger or equal to 0 and smaller than 7.\n");
    }
  else
    {
      srand(atoi(argv[2]));

      arraySize = atoi(argv[1]);

      /* alocate memory for sortAarry pointer */
      sortArray = (qs_data*) malloc(sizeof(qs_data));

      /* terminate program if error occurrs */
      if (sortArray == NULL)
	{	  
	  fprintf(stderr, "in main(), memory allocation failure: ");
	  perror("");
	  exit(1);
	}
      sortArray->right = arraySize - 1;     /* initialize the right most index */

      sortArray->left = 0;      /* initialize the left most index */

      sortArray->depth = atoi(argv[3]);      /* initialize the depth of recursion */

      /* alocate memory for the integer array */
      sortArray->arr = (long*) malloc(arraySize * sizeof(long));
 
      /* terminate program if error occurrs */
      if (sortArray->arr == NULL)
	{
	  fprintf(stderr, "in main(), memory allocation failure: ");
	  perror("");
	  exit(1);
	}

      /* generate random long integers to sort */
      for (i = 0; i < arraySize; i++)
	{
	  sortArray->arr[i] = random();
	}
      
      /* sort the array */
      quicksort(sortArray);

      /* display the sorted array if the array has size of below or equal to 10 */
      if (arraySize <= 10)
	{
	  for (i = 0; i < arraySize; i++)
	    {
	      printf("%ld\n",sortArray->arr[i]);
	    }
	}
      
      /* free the memory for array and the pointer to the structure */
      free(sortArray->arr);
      free(sortArray);
    }

  return 0;
}
