# Allocātor Parvulōrum - Allocator of tiny things

## What it does
This is a very simple dynamic memory allocator library for C and C++. It provides all the functionality of glibc's memory allocation, with some slight differences in how memory is stored and accessed. 

## What makes it different
The majority of memory allocators out preemptively `sbrk()` or `mmap()` memory that is not yet used in order to redude the number of system calls to the kernel, as those system calls are relatively time expensive. Allpar however, will at no point hava a larger heap than the memory being actively used(explicitly allocated by the user and not yet freed). This has a few advantages and disadvantages: 
### Advantages
* Saves way more space tan traditional memory allocators, as there are not kilobytes of unused memory stuck in the internal bookkeeping of the allocator
* As a result of this lower memory usage, it can be used to optimize programs to be able to be run on olderr hardware without tons of memory, or microcontrollers with the same issue

### Disadvantages
* Compared to other allocators, it is very slow for a few reasons
  * Far more system calls are made 
  * Finding a piece of memory is an O(n) operation instead of an O(1) operation (This is explained later)
  * Every time a block of memory is freed, another O(n) operation takes place to move all memory above the freed block down.

* The memory access method is UID based, not pointer based making it necessary for a wrapper function.


## How it works
First of all, it is important to note how the heap is adjusted in C and C++. The method used here is a Linux system call `sbrk(intptr_t increment)` that increments the size of the heap by `increment` bytes, and then returns the previous program break. The program break is the top of the heap. It is important to remember here that in general the heap grows up rather than down like the stack does. An example of using this function would be increasing the size of the heap to allow for one `int`, and then storing an `int` there:
```c
int main(){
	int* int_loc = sbrk(sizeof(int));
	*int_loc = 5;
	printf("Number stored: %d\n", *int_loc);
	return 0;
}
```

When a chunk of memory is requested, allpar will `sbrk()` for the number of bytes that you wish, plus the size of a memory metadata block. which holds two pieces of information:
* The size in bytes of the memory being requested
* A Memory Unique IDentifier (MUID)

These two pieces of information allow for traversal of all memmory allocated by this library, as well as access to a specific block of memory regardless of where it actually is. 

Once allpar increases the program break by that much, it populates the memory meta block with the correct information in the following format:
       
```
  To Higher Address
         ^^
         ||
/--------------------\
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|                    |
|--------------------| <-- Actual memory 
|                    |
|        MUID        | 
|                    |
|size of above memory|
\--------------------/ <-- Memory metadata block. The size of this block is 2 times sizeof(int)
         ||
		 VV
     To Lower Address
```

The size of the succeeding memory being present in the memory meta block allows for a few things:
* Being able to free the memory only given the MUID of that memory block
* Being able to jump to the next memory block without the need to store the absolute address of the next block
  * This also allowes for massive chunks of memory to be moved around without losing the ability to access them, as jumping between blocks is a relative operation based on the size of a block. This is done in conjunction with the MUID of the block.

The MUID of the block is a number unique to that block of memory that allows for linear traversal of the heap to find a certain block of memory. This is necessary to allow memory to be moved and rearannged in whichever way it sees fit, so long as every chunk of memmory has a meta block preceeding it, and the size of the memory stored in the metablock is correct. 


