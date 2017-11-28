/*
    Name: Caden Sanders
    Major: Electrical Engineering
    Classification: Senior
    Net ID: cjs163

    Project 4
    Operating Systems [CS 4328]

    This is a program that replicates a memory system. It will take an
    input of a reference string and size of the main memory then run
    two different algorithms to test the efficiency of each algorithm.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int optimal(int ref[], int length, int mm);
int lru(int ref[], int length, int mm);
int future(int mem[], int ref[], int mm, int length, int start);

//PAGE TABLE STRUCTURE FOR MORE REALISTIC SIMULATION (see optimal function description)
struct PageTables {
  int pageNumber[30];
  int frameNumber[30];
  int validBit[30];
  int referenceBit[30];
};

/*
*   The main function will organize the input from the user and error check the input.
*   Once the input is verified to be within a reasonable range, the program sends
*   the data to each algorithm's function to be evaluated.
*/

int main() {
  int MMSize;
  char refString[30];
  int refBreak = 0;
  int refSize = 0;
  int ref[30];

  printf("\n\n\nHello,\nI am a program that simulates some common page replacement algorithms!\nPlease enter the number of frames allowed (9 or less): ");
  while(1) {
    scanf("%d", &MMSize);
    if(MMSize>9 || MMSize<0) {
      printf("\nThe frame size should be less than 9!\n\n");
    }
    else
      break;
  }

  printf("\n\nNow we will create a random reference string for the algorithms to use.\nEnter a string of integers with values between 1 and 9: ");

  while(1) {
    scanf("%s", refString);

    if(strlen(refString)>30) {
      printf("The reference string must be less than 30 values. Please try again.\n");
    }

    else {
      for(int i=0; i<strlen(refString);) {
        if(
          refString[i]=='1' ||
          refString[i]=='2' ||
          refString[i]=='3' ||
          refString[i]=='4' ||
          refString[i]=='5' ||
          refString[i]=='6' ||
          refString[i]=='7' ||
          refString[i]=='8' ||
          refString[i]=='9') {
            i++;
            refBreak = 1;
        }
        else {
          printf("You must enter integers from 1 to 9. Please try again.\n");
          refBreak = 0;
          break;
        }
      }
    }
    if(refBreak==1) {
      refSize = strlen(refString);
      break;
    }
  }
  for(int i=0; i<refSize; i++) {
    ref[i] = refString[i] - '0';
  }


  printf("\nTesting Optimal Page Replacement Algorithm...");
  int opFaults = optimal(ref, refSize, MMSize);

  printf("\n\nTesting LRU Page Replacement Algorithm...");
  int lruFaults = lru(ref, refSize, MMSize);

  printf("\n\n****\nOptimal had %d page faults.\nLRU had %d page faults.\n****\n\n", opFaults, lruFaults);

  return 0;
}


/*
*   This is the optimal function. It creates a page table and updates
*   the valid bit depending on whether the data is in main memory or not.
*   The reference string represents which page in the page table is being
*   accessed. The reference bit is not used for this algorithm.
*/
int optimal(int ref[], int length, int mm) {
  int pageFaults = 0;
  int choice = 0;
  int mainMemory[mm];

  struct PageTables OpPageTable;
  for(int i=0; i<30; i++) {
    OpPageTable.validBit[i] = 0;
  }

  int memCount = 0;

  for(int i=0; i<mm; i++) {
    if(OpPageTable.validBit[ref[i]] != 1) {
      mainMemory[memCount] = ref[i];
      pageFaults++;
      OpPageTable.frameNumber[ref[i]] = memCount;
      OpPageTable.validBit[ref[i]] = 1;
      memCount++;
    }
  }

  for(int i=mm; i<length; i++) {
    if(OpPageTable.validBit[ref[i]] != 1) {
      pageFaults++;
      if(memCount<mm) {
        choice = memCount;
        memCount++;
      }
      else {
        choice = future(mainMemory, ref, mm, length, i);
        OpPageTable.validBit[mainMemory[choice]] = 0;
      }
      OpPageTable.frameNumber[ref[i]] = choice;
      mainMemory[choice] = ref[i];
      OpPageTable.validBit[ref[i]] = 1;
    }
  }
  return pageFaults;
}

/*
*   This is the Least Recently Used algortithm. This does the same thing as
*   the Optimal algorithm but it can not see into the future and instead
*   uses past values to estimate the best swap in/swap out. The reference bit
*   is only a single bit and the algorithm will swap out the first spot in main
*   memory that it can find with a reference bit set low.
*/

int lru(int ref[], int length, int mm) {
  int pageFaults = 0;
  int choice = 0;
  int k = 0;
  int mainMemory[mm];

  struct PageTables lruPageTable;
  for(int i=0; i<30; i++) {
    lruPageTable.validBit[i] = 0;
    lruPageTable.referenceBit[i] = 0;
  }

  int memCount = 0;

  for(int i=0; i<mm; i++) {
    if(lruPageTable.validBit[ref[i]] != 1) {
      mainMemory[memCount] = ref[i];
      pageFaults++;
      lruPageTable.frameNumber[ref[i]] = memCount;
      lruPageTable.validBit[ref[i]] = 1;
      memCount++;
    }
    else {
      lruPageTable.referenceBit[ref[i]] = 1;
    }
  }

  for(int i=mm; i<length; i++) {
    if(lruPageTable.validBit[ref[i]] != 1) {
      pageFaults++;

      if(memCount<mm) {
        choice = memCount;
        memCount++;
      }
      else {
        for(int j=1; j<length; j++) {
          if(lruPageTable.referenceBit[j]==0 && lruPageTable.validBit[j]==1) {
            lruPageTable.validBit[j] = 0;
            choice = lruPageTable.frameNumber[j];
            k = j;
            break;
          }
        }
        lruPageTable.validBit[k] = 0;
        lruPageTable.referenceBit[k] = 0;
      }
      lruPageTable.frameNumber[ref[i]] = choice;
      mainMemory[choice] = ref[i];
      lruPageTable.validBit[ref[i]] = 1;
    }
    else {
      lruPageTable.referenceBit[ref[i]] = 1;
    }
  }
  return pageFaults;
}

/*
*   This is the function used for the Optimal algorithm so that it can "look
*   into the future". It will tell the algorithm which values will not be needed
*   furthest in the future.
*/

int future(int mem[], int ref[], int mm, int length, int start) {
  int distance = 0;
  int bestDist = 0;
  int best = 0;

  for(int i=0; i<mm; i++) {
    distance = 0;
    for(int j=start; j<length; j++) {
      if(ref[j] == mem[i]) {
        distance = j;
        break;
      }
      else if(j==(length-1)) {
        best = i;
        distance = length+1;
        break;
      }
    }
    if(distance>bestDist) {
      best = i;
      bestDist = distance;
    }
  }

  return best;
}
