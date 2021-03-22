#include <stdlib.h>
#include "input.h"
#include <pthread.h>

#define NUM_THREADS                        4

#define NUM_NODES                          100
#define NONE                               9999

#define TASKS_PER_THREAD (NUM_NODES + NUM_THREADS - 1) / NUM_THREADS

int tasks[NUM_THREADS][TASKS_PER_THREAD][2];

pthread_t threads[NUM_THREADS-1];
pid_t pid[NUM_THREADS-1];

struct _NODE
{
  int iDist;
  int iPrev;
};
typedef struct _NODE NODE;

struct _QITEM
{
  int iNode;
  int iDist;
  int iPrev;
  struct _QITEM *qNext;
};
typedef struct _QITEM QITEM;

QITEM allocated[100000];
QITEM *qHead = NULL;

int g_qCount = 0;
NODE rgnNodes[NUM_NODES];
int ch;
int iPrev, iNode;
int i, iCost, iDist;


void print_path (NODE *rgnNodes, int chNode)
{
  if (rgnNodes[chNode].iPrev != NONE)
    {
      print_path(rgnNodes, rgnNodes[chNode].iPrev);
    }
  //printf (" %d", chNode);
}

void enqueue (int iNode, int iDist, int iPrev)
{
  static int notAll = 0;
  QITEM *qNew = &allocated[notAll];
  notAll++;
  QITEM *qLast = qHead;
  
  if (!qNew) 
    {
      //printf("Out of memory.\n");
      exit(1);
    }
  qNew->iNode = iNode;
  qNew->iDist = iDist;
  qNew->iPrev = iPrev;
  qNew->qNext = NULL;
  
  if (!qLast) 
    {
      qHead = qNew;
    }
  else
    {
      qLast = &allocated[notAll-2];
      qLast->qNext = qNew;
    }
  g_qCount++;
  //               ASSERT(g_qCount);
}


void dequeue (int *piNode, int *piDist, int *piPrev)
{
  QITEM *qKill = qHead;
  
  if (qHead)
    {
      //                 ASSERT(g_qCount);
      *piNode = qHead->iNode;
      *piDist = qHead->iDist;
      *piPrev = qHead->iPrev;
      qHead = qHead->qNext;
      //free(qKill);
      g_qCount--;
    }
}


int qcount (void)
{
  return(g_qCount);
}

int dijkstra(int chStart, int chEnd) 
{
  

  for (ch = 0; ch < NUM_NODES; ch++)
    {
      rgnNodes[ch].iDist = NONE;
      rgnNodes[ch].iPrev = NONE;
    }
  if (chStart == chEnd) 
    {
      //printf("Shortest path is 0 in cost. Just stay where you are.\n");
    }
  else
    {
      rgnNodes[chStart].iDist = 0;
      rgnNodes[chStart].iPrev = NONE;
      enqueue (chStart, 0, NONE);

     while (qcount() > 0)
	{
	  dequeue (&iNode, &iDist, &iPrev);
	  for (i = 0; i < NUM_NODES; i++)
	    {
	      if ((iCost = AdjMatrix[iNode][i]) != NONE)
		{
		  if ((NONE == rgnNodes[i].iDist) || 
		      (rgnNodes[i].iDist > (iCost + iDist)))
		    {
		      rgnNodes[i].iDist = iDist + iCost;
		      rgnNodes[i].iPrev = iNode;
		      enqueue (i, iDist + iCost, iNode);
		    }
		}
	    }
	}
      
      //printf("Shortest path is %d in cost. ", rgnNodes[chEnd].iDist);
      //printf("Path is: ");
      //print_path(rgnNodes, chEnd);
      //printf("\n");
    }
    return 0;
}

//void thread_main(int *thread_id) {
void thread_main(int id) {
    int i;
    int a,b;
    //int id = *thread_id;
    printf("%d started\n", id);

    for(i = 0; i < TASKS_PER_THREAD; i++) {
        a = tasks[id][i][0]; 
        b = tasks[id][i][1]; 
        if(a == -1) {
            break;
        }
        printf("%d: %d %d\n", id, a, b);
        dijkstra(a,b);
    }
    printf("%d end\n", id);
    exit(0);
}

int main(int argc, char *argv[]) {
  int i,j;
  int thread_id = 0, task_id = 0;
   /* make a fully connected matrix */
   // see input.h
  /* finds 10 shortest paths between nodes */
  for (i=0,j=NUM_NODES/2;i<NUM_NODES;i++,j++) {
			j=j%NUM_NODES;
      //dijkstra(i,j);
        tasks[thread_id][task_id][0] = i;
        tasks[thread_id][task_id][1] = j;
        
        thread_id = (thread_id + 1) % NUM_THREADS;
        if(thread_id == 0) {
            task_id++;
        }
  }

  while(task_id < TASKS_PER_THREAD) {
      for(i = thread_id; i < NUM_THREADS; i++) {
        tasks[i][task_id][0] = -1; 
        tasks[i][task_id][1] = -1; 
      }
      task_id++;
  }

  for(i = 0; i < NUM_THREADS; i++) {
      for(j = 0; j < TASKS_PER_THREAD; j++) {
          printf("(%d, %d) ", tasks[i][j][0], tasks[i][j][1]);
      }
      printf("\n");
  }
  
  for(i = 1; i < NUM_THREADS; i++) {
      pid[i] = fork();
      if(pid[i] == 0) {
          thread_main(i);
          break;
      }
  }
  thread_main(0);
/*
  for(i = 1; i < NUM_THREADS; i++) {
      int *arg = malloc(sizeof(*arg));
    printf("loop%d \n", i);
      if(arg == NULL) {
          printf("Thread argument failure.");
          exit(0);
      }
      *arg = i;
      pthread_create(&threads[i-1], 0, thread_main, arg);
      //thread_main(i);
      printf("%d \n", i);
  }
    int *arg = malloc(sizeof(*arg));
    *arg = 0;
    */
  //thread_main(arg);

  return 0;
}
