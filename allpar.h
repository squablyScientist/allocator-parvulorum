struct mem_block_meta{
	int size;
	int MUID;
};
#define MEM_BLOCK_META_DEF


/**
 * Allocates size bytes of memory for the caller to use. This method of memory
 * allocation is different from libc's malloc as it does not do much bookkeeping
 * about whether or not a memory block has been freed or not. When an allocation
 * is performed, the following information is placed into memory at the top of
 * the heap in the form of a metadata block:
 *
 *		- The size in bytes of the succeeding memory block
 *		- A unique identifier for this block of memory.
 *
 * The program break is then in total is moved up the number of bytes 
 * requested plus the size of the metadata block. 
 *
 * The function then returns the MUID of the block for use in retrieving the
 * memory as well as freeing it. 
 *
 * @param size: The size in bytes of memory being requested.
 *
 * @return: A M(emory) UID that corresponds to the memory just allocated.
 */
int mallocp(int size);

/**
 * Releases the memory block with MUID back to the kernel, destroying its
 * contents. This implementation of free moves all memory above the memory block
 * specified down to overwrite the memory to be freed. This allows a simple
 * reduction of the program break without destorying any in use information. 
 *
 * A side effect of this method of freeing memory is that there is no wasted or
 * fragmented memory in the heap, since every free is an automatic defrag. The
 * program break will always be as small as possible as well, thus reducing
 * uneeded memory being held by the process. All memory that would be marked as
 * freed is simply returned to the kernel.
 *
 * @param MUID: The M(emory)UID of the memory block to be freed.
 */
void freep(int MUID);

/**
 * This function retrieves the memory address of a block of memory allocated by
 * mallocp(...). Since each block of memory has a meta block directly preceeding
 * it in memory, & that memory block contains a UID for that block of memory as
 * well as the size of the succeeding memory, it is trivial to traverse memory
 * to find the correct block given its MUID.
 *
 * @param MUID: The M(emory)UID of the memory block to be found. This is
 * obtained from mallocp.
 *
 * @return: A pointer to the block of memory which has a metablock containing
 * MUID preceeding it.
 */
 void *retrieve_memory(int MUID);
