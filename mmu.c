#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define NUM_FRAMES 100
#define PHYSICAL_MEMORY_SIZE (NUM_FRAMES * PAGE_SIZE)
#define PROC_PAGES (500)


#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

enum algorithm
{
    FIFO = 0,
    LRU, 
    OPT
};

enum algorithm pra = FIFO;

int page_faults = 0;

int firstIdx = 0; // for FIFO algorithm

typedef struct 
{
    int valid;
    int frame_number;
} PageTableEntry;

typedef struct 
{
    PageTableEntry entries[PROC_PAGES];
} PageTable;

typedef struct 
{
    int pageNumber;
    int valid;
    uint8_t data[PAGE_SIZE];
    // Additional data for the page
} Page;

typedef struct 
{
    Page frame[NUM_FRAMES];
} PhysicalMemory;

typedef struct 
{
    PageTable page_table;
    PhysicalMemory physical_memory;
} MMU;

void initializeMMU(MMU* mmu) 
{
    // Initialize MMU and associated data structures
    for (int i = 0; i < PROC_PAGES; i++)
    {
        mmu->page_table.entries[i].valid = -1;
    }

    for (int i = 0; i < NUM_FRAMES; i++)
    {
        mmu->physical_memory.frame[i].pageNumber = -1;
        mmu->physical_memory.frame[i].valid = -1;
    }
}

int findFreeFrame(MMU* mmu)
{
    // Find the first free frame in physical memory
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        if (mmu->physical_memory.frame[i].valid == -1)
        {
            return i;
        }
    }
    return -1; // No free frames available
}

void fifoPageReplacement(MMU* mmu, int pageNum)
{
    // Handle page fault and add the page to physical memory
    int freeFrame = findFreeFrame(mmu);
    if (freeFrame != -1)
    {
        LOG("Found a free frame for page number = %d, free frame = %d\n", pageNum, freeFrame);
        mmu->page_table.entries[pageNum].valid = 1;
        mmu->page_table.entries[pageNum].frame_number = freeFrame;
        mmu->physical_memory.frame[freeFrame].pageNumber = pageNum;
        mmu->physical_memory.frame[freeFrame].valid = 1;

    }
    else
    {
        int pageNumDel = mmu->physical_memory.frame[firstIdx].pageNumber;
        LOG("Did not find a free frame for page number = %d, so replacing page = %d\n", pageNum, pageNumDel);
        mmu -> page_table.entries[pageNumDel].valid = -1;
        mmu -> page_table.entries[pageNum].frame_number = firstIdx;
        mmu->physical_memory.frame[firstIdx].pageNumber = pageNum;
        firstIdx = (firstIdx) + 1;
        firstIdx = firstIdx % NUM_FRAMES;
    }
}


void LRUPageReplacement()

void addPage(MMU* mmu, int pageNum)
{
    if(pra == FIFO)
    {
        fifoPageReplacement(mmu, pageNum);
    }
    else if(pra == LRU)
    {
        LRUPageReplacement(mmu, pageNum);
    }
}

int main() 
{
    MMU mmu;
    int numPageFaults = 0;
    int numPageHits = 0;
    initializeMMU(&mmu);
    int refString[PROC_PAGES];

    for (int i = 0; i < PROC_PAGES; i++)
    {
        refString[i] = rand() % PROC_PAGES;
    }

    LOG("Reference string is:\n");

    for(int i = 0; i < PROC_PAGES; i++)
    {
        LOG("%d\t", refString[i]);
    }

    LOG("\n");

    for (int i = 0; i < PROC_PAGES; i++)
    {
        int pageNum = refString[i];
        if (mmu.page_table.entries[pageNum].valid == -1)
        {
            LOG("Page fault for %d\n", pageNum);
            numPageFaults++;
            addPage(&mmu, pageNum);
        }
        else
        {
            LOG("YAY: Page hit for %d\n", pageNum);
            numPageHits++;
        }
    }

    LOG("Ultimately, what's there is:\n");

    for(int i = 0; i < NUM_FRAMES; i++)
    {
        LOG("%d\t",mmu.physical_memory.frame[i].pageNumber);
    }

    LOG("\n");

    printf("Page Faults: %d\n", numPageFaults);
    printf("Page Hits: %d\n", numPageHits);

    return 0;
}