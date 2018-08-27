//TODO: convert all comments that span multiple lines into block comments
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef unsigned char byte;

struct mem_block_meta{
	int size;
	int MUID;
	//struct mem_block_meta* next;
};


static struct mem_block_meta* global_top;
static struct mem_block_meta* global_bot;

static int top_MUID = 1;

/**
 * This function places a mem_block_meta of all 0s on top of the heap, to act as
 * a NUL terminator for the heap's memory during traversal.
 *
 * @return: a pointer to this new mem_block_meta
 */
struct mem_block_meta* push_zero_block(){
	struct mem_block_meta* zero_block = sbrk(0);
	sbrk(sizeof(struct mem_block_meta));
	zero_block->size = 0;
	zero_block->MUID = 0;
	return zero_block;
}

struct mem_block_meta *request_mem_block(int size){

	// Creates the very first 0 block in order to act as a base. Is only run the
	// very first allocation. 
	if(global_top == NULL){
		global_top = push_zero_block();
		global_bot = global_top;
	}

	// Moves the program break back to erase the current zero block
	sbrk(-sizeof(struct mem_block_meta));

	// Places the new mem_block where the old zero block used to be
	struct mem_block_meta* meta = sbrk(0);
	sbrk(size + sizeof(struct mem_block_meta));
	
	//TODO: guarentee that sbrk succeeds 

	// Sets everything about the new mem_block
	global_top = meta;
	meta->size = size;
	meta->MUID = top_MUID;
	top_MUID++;
	
	// Pushes a new zero block to act as NUL terminator
	global_top = push_zero_block();

	return meta;
}
	

int mallocp(int size){

	struct mem_block_meta* block = request_mem_block(size);
	return block -> MUID;

}

struct mem_block_meta *retrieve_memory_meta_block(int MUID){
	struct mem_block_meta* current = global_bot;
	while(current != global_top && current->MUID != MUID){

		//Sets the current ptr to look at the next mem_block_meta. This will not
		//overflow into unallocated memory as the while loop makes sure that
		//current is not the global_top.
		current = (struct mem_block_meta*)((char*)current + current->size + \
				sizeof(struct mem_block_meta));
	}
	
	// Returns NULL if traversing the heap did not find the MUID asked for, i.e.
	// current points to the NUL terminator of the memory, i.e. global_top 
	return current == global_top ? NULL : current;
}

void *retrieve_memory(int MUID){
	return retrieve_memory_meta_block(MUID) + 1;
}


int main(void){
	int number1ID = mallocp(sizeof(int));
	*(int*)retrieve_memory(number1ID) = 1;

	int number2ID = mallocp(sizeof(int));
	*(int*)retrieve_memory(number2ID) = 10000;

	int stringID = mallocp(sizeof("HELLO WORLD"));
	strncpy((char*)retrieve_memory(stringID), "HELLO WORLD",255);

	printf("%d\n", *(int*)retrieve_memory(number1ID));
	printf("%s\n", (char*)retrieve_memory(stringID));
	printf("%d\n", *(int*)retrieve_memory(number2ID));
	

	return 0;
}

