#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef unsigned char byte;

struct mem_block_meta{
	int size;
	int MUID;
	struct mem_block_meta* next;
};


static struct mem_block_meta* global_top;


static int top_MUID = 0;

struct mem_block_meta *request_mem_block(int size){


	struct mem_block_meta* meta = sbrk(0);
	sbrk(size + sizeof(struct mem_block_meta));
	
	//TODO: guarentee that sbrk succeeds 

	meta->next = global_top;

	global_top = meta;

	meta->size = size;

	meta->MUID = top_MUID;
	top_MUID++;
	
	return meta;
}
	

int mallocp(int size){

	struct mem_block_meta* block = request_mem_block(size);
	return block -> MUID;

}

struct mem_block_meta *retrieve_memory_meta_block(int MUID){
	struct mem_block_meta* current = global_top;
	while(current && current->MUID != MUID){
		current = current->next;
	}
	return current;
}

void *retrieve_memory(int MUID){
	return retrieve_memory_meta_block(MUID) + 1;
}



int main(void){
	int number1ID = mallocp(sizeof(int));
	*(int*)retrieve_memory(number1ID) = 5;

	int number2ID = mallocp(sizeof(int));
	*(int*)retrieve_memory(number2ID) = 10000;

	int stringID = mallocp(sizeof("TEST"));
	strncpy((char*)retrieve_memory(stringID), "TEST", 5);

	printf("%d\n", *(int*)retrieve_memory(number1ID));
	printf("%s\n", (char*)retrieve_memory(stringID));
	printf("%d\n", *(int*)retrieve_memory(number2ID));
	

	return 0;
}

