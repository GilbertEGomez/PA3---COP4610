#include "pa1.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define FRAME_NO 10
#define lrand() rand() * RAND_MAX + rand()
int locate(int *frames, int size, int page_no) {
  for (int i = 0; i < size; i++)
    if (frames[i] == page_no)
      return i;
  return -1;
}
int optimal(int *ref_str, int size, int limit) {
  // size is the # of frames allocated for the process
  // limit is the max # of cells that we look ahead in the RS to implement the
  // optimal algorithm initialize
  int page_faults = size;
  int frames[size], i, cur, page_no, frame_no;
  for (i = 0; i < size; i++)
    frames[i] = -1; // empty
  for (i = 0, cur = 0; i < size; i++, cur++) {
    // filling out the whole physical memory (frames array)
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no + 1) // already exists
      i--;
    else
      frames[i] = page_no;
  }
  // main loop
  for (; cur < 1000000; cur++) {
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no != -1) // already exists
      continue;
    // look at ref_str[cur+1:cur+limit] to see which one has not been referred
    // to the longest in the future
    page_faults++;
    // This is the very first line that is differnet in other algorithms
    unsigned unused = (1 << size) - 1;
    int victim;
    for (int k = 1; k <= limit && unused && cur + k < 1000000; k++) {
      victim = locate(frames, size, ref_str[cur + k]);
      if (victim == -1)
        continue;
      unused &= ~(1 << victim);
    }
    if (!unused)
      frames[victim] = page_no;
    else {
      victim = 0;
      while (unused % 2 == 0)
        unused = unused >> 1, victim++;
      frames[victim] = page_no;
    }
  }

  return page_faults;
}

int FIFO(int *ref_str, int size, int limit) {
  // size is the # of frames allocated for the process
  // limit is the max # of cells that we look ahead in the RS to implement the
  // optimal algorithm initialize
  int page_faults = size;
  int frames[size], i, cur, page_no, frame_no;
  lnklst_queue queue = create_queue2();
  for (i = 0; i < size; i++) {
    frames[i] = -1;      // empty
    enqueue2(&queue, i); // Enqueue all indexes to frame array (FIFO)
  }
  for (i = 0, cur = 0; i < size; i++, cur++) {
    // filling out the whole physical memory (frames array)
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no + 1) // already exists
      i--;
    else
      frames[i] = page_no;
  }
  // main loop
  for (; cur < 1000000; cur++) {
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no != -1) // already exists
      continue;
    page_faults++;
    int index = dequeue2(&queue); // Dequeue the index of the page fault
    frames[index] =
        page_no; // use the temp variable index which is the last item in FIFO,
                 // to reference the frame array and index the page #.
    enqueue2(&queue, index); // Enqueue index
  }

  return page_faults;
}

int lru(int *ref_str, int size, int limit) {
  // size is the # of frames allocated for the process
  // limit is the max # of cells that we look ahead in the RS to implement the
  // optimal algorithm initialize
  int page_faults = size;
  int frames[size], i, cur, page_no, frame_no;
  for (i = 0; i < size; i++)
    frames[i] = -1; // empty
  for (i = 0, cur = 0; i < size; i++, cur++) {
    // filling out the whole physical memory (frames array)
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no + 1) // already exists
      i--;
    else
      frames[i] = page_no;
  }
  // main loop
  for (; cur < 1000000; cur++) {
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no != -1) // already exists
      continue;
    // look at ref_str[cur+1:cur+limit] to see which one has not been referred
    // to the longest in the future
    page_faults++;
    // This is the very first line that is differnet in other algorithms
    unsigned unused = (1 << size) - 1;
    int victim;
    for (int k = 1; k <= limit && unused && cur - k >= 0; k++) {
      victim = locate(frames, size, ref_str[cur - k]);
      if (victim == -1)
        continue;
      unused &= ~(1 << victim);
    }
    if (!unused)
      frames[victim] = page_no;
    else {
      victim = 0;
      while (unused % 2 == 0)
        unused = unused >> 1, victim++;
      frames[victim] = page_no;
    }
  }

  return page_faults;
}

int secondChance(int *ref_str, int size) {
  // size is the # of frames allocated for the process
  int page_faults = size;
  int frames[size], secChance[size], i, cur, page_no, frame_no;
  lnklst_queue queue = create_queue2();
  lnklst_queue tempQueue = create_queue2();
  for (i = 0; i < size; i++) {
    frames[i] = -1; // empty
    secChance[i] = 0; // this stores the second chance bit for each frame
    enqueue2(&queue, i); // Enqueue all indexes to frame array
  }
  for (i = 0, cur = 0; i < size; i++, cur++) {
    // filling out the whole physical memory (frames array)
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no + 1) // already exists
    {
      secChance[frame_no] = 1;//the second chance bit of the hit index is set to 1
      i--;
    } else
      frames[i] = page_no;
  }
  // main loop
  for (; cur < 1000000; cur++) {
    page_no = ref_str[cur];
    frame_no = locate(frames, size, page_no);
    if (frame_no != -1) // already exists
    {
      secChance[frame_no] = 1;//the second chance bit of the hit index is set to 1
      continue;
    }
    page_faults++;
    int index = dequeue2(&queue); // Dequeue the front index in the queue
    while (secChance[index] == 1)//will keep going until we find an index without second chance bit set to 1
    {
      secChance[index] = 0;//second chance bit will now be reset to 0
      enqueue2(&tempQueue, index);//we put the second chance survivors in a temporary queue
      if (!is_empty2(queue))
        index = dequeue2(&queue);
      else
        index = dequeue2(&tempQueue);//if queue is empty, that means all indexes/frames had second chance bit set to 1, so now we just take out the first index from the tempQueue which was the first one we had dequeued from the queue (second chance bit will definitely be set to 0 as we manually set it to 0 before putting it in the tempQueue)
    }
    frames[index] =
        page_no; // now we just replace the page
    while (!is_empty2(queue))//we stuff the remeining queue elements into tempQueue
    {
      enqueue2(&tempQueue, dequeue2(&queue));
    }
    while (!is_empty2(tempQueue))//and then we stuff the whole tempQueue into the queue again
    {
      enqueue2(&queue, dequeue2(&tempQueue));
    }
    enqueue2(&queue, index); // Enqueue index
  }

  return page_faults;
}

int main(int argc, char **argv) {
  int e, m, P;
  double t;
  srand(time(NULL));
  while (*++argv) {
    //./main -P 1048576 -e 10 -m 20 -t 1000 (Scenario #1)
    if (**argv != '-')
      return 1;
    switch ((*argv)[1]) {
    case 'P':
      P = atoi(*++argv);
      break;
    case 'e':
      e = atoi(*++argv);
      break;
    case 'm':
      m = atoi(*++argv);
      break;
    case 't':
      t = 1.0 / atoi(*++argv);
      break;
    default: // error
      return 1;
    }
  }
  printf("CLAs are P = %d, locus size = %d, m = %d, transition prob. = %f\n", P,
         e, m, t);
  // initially locus: 0...e-1
  int locus_position = 0; // s
  int ref_count = 0;
  int *ref_str = (int *)malloc(1000000 * sizeof(int));
  while (ref_count <= 1000000) {
    int next_ref = rand() % e + locus_position;
    if (!ref_count || next_ref != ref_str[ref_count - 1])
      ref_str[ref_count++] = next_ref;
    else // try again
      continue;
    if (ref_count % m == 0) {    // let's displace the locus
      if (rand() < t * RAND_MAX) // jump
        locus_position = lrand() % (P - e + 1);
      else
        locus_position = (locus_position + 1) % (P - e + 1);
    }
  }
  int optimal_page_fault = optimal(ref_str, FRAME_NO, e * m);
  int FIFO_page_fault = FIFO(ref_str, FRAME_NO, e * m);
  int lru_page_fault = lru(ref_str, FRAME_NO, e * m);
  int secondChance_page_fault = secondChance(ref_str, FRAME_NO);
  printf("Optimal page replacement causes %d page faults\n",
         optimal_page_fault);
  printf("FIFO page replacement causes %d page faults\n", FIFO_page_fault);
  printf("LRU page replacement causes %d page faults\n", lru_page_fault);
  printf("Second Chance page replacement causes %d page faults\n", secondChance_page_fault);
  return 0;
}
// Scenario 1-5
//./main -P 1048576 -e 10 -m 20 -t 1000
//./main -P 4194304 -e 10 -m 20 -t 2000
//./main -P 1048576 -e 15 -m 20 -t 1000
//./main -P 4194304 -e 8 -m 50 -t 1000
//./main -P 262144 -e 15 -m 7 -t 2000
