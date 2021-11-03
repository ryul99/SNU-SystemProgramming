//--------------------------------------------------------------------------------------------------
// System Programming                       Memory Lab                                   Fall 2021
//
/// @file
/// @brief dynamic memory manager
/// @author Changmin Choi
/// @studid 2017-19841
//--------------------------------------------------------------------------------------------------


// Dynamic memory manager
// ======================
// This module implements a custom dynamic memory manager.
//
// Heap organization:
// ------------------
// The data segment for the heap is provided by the dataseg module. A 'word' in the heap is
// eight bytes.
//
// Implicit free list:
// -------------------
// - minimal block size: 32 bytes (header +footer + 2 data words)
// - h,f: header/footer of free block
// - H,F: header/footer of allocated block
//
// - state after initialization
//
//         initial sentinel half-block                  end sentinel half-block
//                   |                                             |
//   ds_heap_start   |   heap_start                         heap_end       ds_heap_brk
//               |   |   |                                         |       |
//               v   v   v                                         v       v
//               +---+---+-----------------------------------------+---+---+
//               |???| F | h :                                 : f | H |???|
//               +---+---+-----------------------------------------+---+---+
//                       ^                                         ^
//                       |                                         |
//               32-byte aligned                           32-byte aligned
//
// - allocation policies: first, next, best fit
// - block splitting: always at 32-byte boundaries
// - immediate coalescing upon free
//


#include <assert.h>
#include <error.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dataseg.h"
#include "memmgr.h"

void mm_check(void);

/// @name global variables
/// @{
static void *ds_heap_start = NULL;                     ///< physical start of data segment
static void *ds_heap_brk   = NULL;                     ///< physical end of data segment
static void *heap_start    = NULL;                     ///< logical start of heap
static void *heap_end      = NULL;                     ///< logical end of heap
static int  PAGESIZE       = 0;                        ///< memory system page size
static void *(*get_free_block)(size_t) = NULL;         ///< get free block for selected allocation policy
static int  mm_initialized = 0;                        ///< initialized flag (yes: 1, otherwise 0)
static int  mm_loglevel    = 0;                        ///< log level (0: off; 1: info; 2: verbose)
static void *nf_curr       = NULL;
/// @}

/// @name Macro definitions
/// @{
#define MAX(a, b)          ((a) > (b) ? (a) : (b))     ///< MAX function

#define TYPE               unsigned long               ///< word type of heap
#define TYPE_SIZE          sizeof(TYPE)                ///< size of word type

#define ALLOC              1                           ///< block allocated flag
#define FREE               0                           ///< block free flag
#define STATUS_MASK        ((TYPE)(0x7))               ///< mask to retrieve flagsfrom header/footer
#define SIZE_MASK          (~STATUS_MASK)              ///< mask to retrieve size from header/footer

#define CHUNKSIZE          (1*(1 << 12))               ///< size by which heap is extended

#define BS                 32                          ///< minimal block size. Must be a power of 2
#define BS_MASK            (~(BS-1))                   ///< alignment mask

#define WORD(p)            ((TYPE)(p))                 ///< convert pointer to TYPE
#define PTR(w)             ((void*)(w))                ///< convert TYPE to void*

#define PREV_PTR(p)        ((p)-TYPE_SIZE)             ///< get pointer to word preceeding p

#define PACK(size,status)  ((size) | (status))         ///< pack size & status into boundary tag
#define SIZE(v)            (v & SIZE_MASK)             ///< extract size from boundary tag
#define STATUS(v)          (v & STATUS_MASK)           ///< extract status from boundary tag

#define GET(p)             (*(TYPE*)(p))               ///< read word at *p
#define GET_SIZE(p)        (SIZE(GET(p)))              ///< extract size from header/footer
#define GET_STATUS(p)      (STATUS(GET(p)))            ///< extract status from header/footer

// TODO add more macros as needed

/// @brief print a log message if level <= mm_loglevel. The variadic argument is a printf format
///        string followed by its parametrs
#ifdef DEBUG
  #define LOG(level, ...) mm_log(level, __VA_ARGS__)

/// @brief print a log message. Do not call directly; use LOG() instead
/// @param level log level of message.
/// @param ... variadic parameters for vprintf function (format string with optional parameters)
static void mm_log(int level, ...)
{
  if (level > mm_loglevel) return;

  va_list va;
  va_start(va, level);
  const char *fmt = va_arg(va, const char*);

  if (fmt != NULL) vfprintf(stdout, fmt, va);

  va_end(va);

  fprintf(stdout, "\n");
}

#else
  #define LOG(level, ...)
#endif

/// @}


/// @name Program termination facilities
/// @{

/// @brief print error message and terminate process. The variadic argument is a printf format
///        string followed by its parameters
#define PANIC(...) mm_panic(__func__, __VA_ARGS__)

/// @brief print error message and terminate process. Do not call directly, Use PANIC() instead.
/// @param func function name
/// @param ... variadic parameters for vprintf function (format string with optional parameters)
static void mm_panic(const char *func, ...)
{
  va_list va;
  va_start(va, func);
  const char *fmt = va_arg(va, const char*);

  fprintf(stderr, "PANIC in %s%s", func, fmt ? ": " : ".");
  if (fmt != NULL) vfprintf(stderr, fmt, va);

  va_end(va);

  fprintf(stderr, "\n");

  exit(EXIT_FAILURE);
}
/// @}


static void* ff_get_free_block(size_t);
static void* nf_get_free_block(size_t);
static void* bf_get_free_block(size_t);

void mm_init(AllocationPolicy ap)
{
  LOG(1, "mm_init()");

  //
  // set allocation policy
  //
  char *apstr;
  switch (ap) {
    case ap_FirstFit: get_free_block = ff_get_free_block; apstr = "first fit"; break;
    case ap_NextFit:  get_free_block = nf_get_free_block; apstr = "next fit";  break;
    case ap_BestFit:  get_free_block = bf_get_free_block; apstr = "best fit";  break;
    default: PANIC("Invalid allocation policy.");
  }
  LOG(2, "  allocation policy       %s\n", apstr);

  //
  // retrieve heap status and perform a few initial sanity checks
  //
  ds_heap_stat(&ds_heap_start, &ds_heap_brk, NULL);
  PAGESIZE = ds_getpagesize();

  LOG(2, "  ds_heap_start:          %p\n"
         "  ds_heap_brk:            %p\n"
         "  PAGESIZE:               %d\n",
         ds_heap_start, ds_heap_brk, PAGESIZE);

  if (ds_heap_start == NULL) PANIC("Data segment not initialized.");
  if (ds_heap_start != ds_heap_brk) PANIC("Heap not clean.");
  if (PAGESIZE == 0) PANIC("Reported pagesize == 0.");

  //
  // initialize heap
  //
  // TODO
  //
  // allocate first chunk
  ds_sbrk(CHUNKSIZE);
  ds_heap_stat(&ds_heap_start, &ds_heap_brk, NULL);
  PAGESIZE = ds_getpagesize();
  heap_start = PTR((WORD(ds_heap_start) / BS + 1) * BS);
  heap_end = PTR(WORD(ds_heap_brk - TYPE_SIZE) / BS * BS); // to ensure 1 block for end sentinel block
  LOG(2, "After allocate heap       \n"
         "  ds_heap_start:          %p\n"
         "  ds_heap_brk:            %p\n"
         "  PAGESIZE:               %d\n",
         "  heap_start:             %p\n",
         "  heap_end:               %p\n",
         ds_heap_start, ds_heap_brk, PAGESIZE, heap_start, heap_end);

  // pre heap block
  GET(PREV_PTR(heap_start)) = PACK(0, ALLOC);
  // first free heap block
  GET(heap_start) = PACK(WORD(heap_end) - WORD(heap_start), FREE);
  GET(PREV_PTR(heap_end)) = PACK(WORD(heap_end) - WORD(heap_start), FREE);
  // post heap block
  GET(heap_end) = PACK(0, ALLOC);


  //
  // heap is initialized
  //
  mm_initialized = 1;
}


void* mm_malloc(size_t size)
{
  LOG(1, "mm_malloc(0x%lx)", size);

  assert(mm_initialized);

  //
  // TODO
  //
  // calculate aligned size that also holds the 2 more words for header & footer
  //
  // set header & footer for the allocated block
  //
  // how to check the block size
  // if the size is bigger than requested size,
  // 1. create the header and footer for requested size
  // 2. create the header and footer for next free block with remaining size
  //
  // else if the addr == NULL, then what happend?
  // if last free block has N byte remaining, then ds)sbrk(alloc_size - N)
  // for N, you have to traverse or store previously
  // after that, you can create the headers and footers as usual

  size = (((size + 2 * TYPE_SIZE - 1) / BS) + 1) * BS; // ceiling the (size + 2 * TYPE_SIZE)
  // LOG(2, "Block size is %lu\n", size); // LOGGING
  void *free_p = get_free_block(size);
  if (free_p == NULL) { // if there is no free block over size
    // LOG(2, "Move sbrk backward\n"); // LOGGING
    unsigned long sbrk_size = size;
    void *last_footer = PREV_PTR(heap_end);
    if (!GET_STATUS(last_footer)) { // last block is free
      sbrk_size -= GET_SIZE(last_footer);
    }
    ds_sbrk(sbrk_size); // sbrk_size is multiple of 32
    // update ds_heap_brk, heap_end
    ds_heap_stat(NULL, &ds_heap_brk, NULL);
    heap_end = PTR((WORD(ds_heap_brk - TYPE_SIZE) / BS) * BS); // to ensure 1 block for end sentinel half block
    GET(heap_end) = PACK(0, ALLOC);
    free_p = heap_end - size;
    GET(free_p) = PACK(size, FREE);
    GET(PREV_PTR(heap_end)) = PACK(size, FREE);
  }
  // allocate
  unsigned long origin_size = GET_SIZE(free_p);
  void *alloc_header = free_p;
  void *alloc_footer = PREV_PTR(free_p + size);
  GET(alloc_header) = PACK(size, ALLOC);
  GET(alloc_footer) = PACK(size, ALLOC);
  if (origin_size > size) { // if free block is bigger than wanted size
    void *free_header = free_p + size;
    void *free_footer = PREV_PTR(free_p + origin_size);
    GET(free_header) = PACK(origin_size - size, FREE);
    GET(free_footer) = PACK(origin_size - size, FREE);
  }

  return free_p + TYPE_SIZE;
}

void* mm_calloc(size_t nmemb, size_t size)
{
  LOG(1, "mm_calloc(0x%lx, 0x%lx)", nmemb, size);

  assert(mm_initialized);

  //
  // calloc is simply malloc() followed by memset()
  //
  void *payload = mm_malloc(nmemb * size);

  if (payload != NULL) memset(payload, 0, nmemb * size);

  return payload;
}

void* mm_realloc(void *ptr, size_t size)
{
  LOG(1, "mm_realloc(%p, 0x%lx)", ptr, size);

  assert(mm_initialized);

  //
  // TODO (optional)
  //

  if (ptr == NULL) {
    return mm_malloc(size);
  }
  else if (size == 0) {
    mm_free(ptr);
    return NULL;
  }
  else {
    void *origin_header = PREV_PTR(ptr);
    void *origin_footer = PREV_PTR(origin_header + GET_SIZE(origin_header));
    unsigned long origin_size = GET_SIZE(origin_header);
    unsigned long alloc_size = (((size + 2 * TYPE_SIZE - 1) / BS) + 1) * BS; // ceiling the (size + 2 * TYPE_SIZE)
    if (origin_size > alloc_size) { // if size smaller than origin size
      GET(origin_header) = PACK(alloc_size, ALLOC);
      GET(PREV_PTR(origin_header + alloc_size)) = PACK(alloc_size, ALLOC);
      // free left block using mm_free
      GET(origin_header + alloc_size) = PACK(origin_size - alloc_size, ALLOC);
      GET(origin_footer) = PACK(origin_size - alloc_size, ALLOC);
      mm_free(origin_header + alloc_size + TYPE_SIZE);
      return ptr;
    }
    else if (origin_size == alloc_size)
      return ptr;
    void *next_header = origin_header + origin_size;
    void *next_footer = PREV_PTR(next_header + GET_SIZE(next_header));
    unsigned long total_size = origin_size + GET_SIZE(next_header);
    if (!GET_STATUS(next_header) && (total_size >= alloc_size)) { // if next block is free and large enough
      // LOG(2, "realloc using extend. extend size: %lu\n", alloc_size - origin_size); // LOGGING
      void *footer = PREV_PTR(origin_header + alloc_size);
      GET(origin_header) = PACK(alloc_size, ALLOC);
      GET(footer) = PACK(alloc_size, ALLOC);

      if (total_size > alloc_size) { // if there is left block, free using mm_free
        void *new_next_header = origin_header + alloc_size;
        GET(new_next_header) = PACK(total_size - alloc_size, ALLOC);
        GET(next_footer) = PACK(total_size - alloc_size, ALLOC);
        mm_free(new_next_header + TYPE_SIZE);
      }

      return ptr;
    }
    else {
      // realloc using mm_malloc
      // LOG(2, "realloc using malloc. size: %lu\n", alloc_size);
      void *new_ptr = mm_malloc(size);
      memcpy(new_ptr, ptr, origin_size - 2 * TYPE_SIZE); // copy origin data
      mm_free(ptr);
      return new_ptr;
    }
  }

  return NULL;
}

void mm_free(void *ptr)
{
  LOG(1, "mm_free(%p)", ptr);

  assert(mm_initialized);

  //
  // TODO
  //
  if (ptr == NULL || (WORD(ptr - TYPE_SIZE) % (4 * TYPE_SIZE)) != 0) // && ptr doesn't point the header of the block
    LOG(0, "%p is Invalid Pointer!\n", ptr);
  else {
    ptr -= TYPE_SIZE;
    // then find the address of the footer for this block by reading the size
    unsigned long size = GET_SIZE(ptr);
    void *header = ptr;
    void *footer = ptr + GET_SIZE(ptr) - TYPE_SIZE;
    // reset status
    GET(header) = PACK(GET_SIZE(header), FREE);
    GET(footer) = PACK(GET_SIZE(footer), FREE);
    // coalescing
    if (!GET_STATUS(header - TYPE_SIZE)) { // if previous block is free
      header -= GET_SIZE(header - TYPE_SIZE);
      size += GET_SIZE(header);
      GET(header) = PACK(size, FREE);
      GET(footer) = PACK(size, FREE);
    }
    if (!GET_STATUS(footer + TYPE_SIZE)) { // if post block is free
      footer += GET_SIZE(footer + TYPE_SIZE);
      size += GET_SIZE(footer);
      GET(header) = PACK(size, FREE);
      GET(footer) = PACK(size, FREE);
    }
    if (footer + TYPE_SIZE == heap_end) { // if this block is at the end
      // if the current block is the last block (end block before end sentinel block)
      // reduce heap by calling ds_sbrk(negative_relative_size);
      // LOG(2, "Move sbrk forward\n"); // LOGGING
      ds_sbrk(-size);
      ds_heap_stat(NULL, &ds_heap_brk, NULL);
      heap_end = PTR((WORD(ds_heap_brk - TYPE_SIZE) / BS) * BS); // to ensure 1 block for end sentinel
      GET(heap_end) = PACK(0, ALLOC);
    }
    if (nf_curr != NULL) { // if next fit policy
      if (nf_curr >= heap_end || (header <= nf_curr && nf_curr <= footer)) // if nf_curr is at free block or over heap_end
        nf_curr = heap_start;
    }
  }
}

/// @name block allocation policites
/// @{

/// @brief find and return a free block of at least @a size bytes (first fit)
/// @param size size of block (including header & footer tags), in bytes
/// @retval void* pointer to header of large enough free block
/// @retval NULL if no free block of the requested size is avilable
static void* ff_get_free_block(size_t size)
{
  LOG(1, "ff_get_free_block(1x%lx (%lu))", size, size);

  assert(mm_initialized);

  //
  // TODO
  //

  void *curr = heap_start;
  while(curr < heap_end) { // until heap end
    if (!GET_STATUS(curr) && GET_SIZE(curr) >= size) {
      return curr;
    }
    curr += GET_SIZE(curr);
  }

  return NULL;
}

/// @brief find and return a free block of at least @a size bytes (next fit)
/// @param size size of block (including header & footer tags), in bytes
/// @retval void* pointer to header of large enough free block
/// @retval NULL if no free block of the requested size is avilable
static void* nf_get_free_block(size_t size)
{
  LOG(1, "nf_get_free_block(0x%x (%lu))", size, size);

  assert(mm_initialized);

  //
  // TODO
  //

  if (nf_curr == NULL || nf_curr >= heap_end) // if nf_curr is not init or over heap_end
    nf_curr = heap_start;
  void *nf_find_start = nf_curr;
  do {
    if (!GET_STATUS(nf_curr) && GET_SIZE(nf_curr) >= size) { // if there is proper free block
      // LOG(2, "nf_curr: %p\n", nf_curr);
      return nf_curr;
    }
    nf_curr += GET_SIZE(nf_curr);
    if (nf_curr >= heap_end) // make cycle (return to start)
      nf_curr = heap_start;
  } while(nf_curr != nf_find_start); // until see all block

  // LOG(2, "nf_curr: %p\n", NULL);
  return NULL;
}

/// @brief find and return a free block of at least @a size bytes (best fit)
/// @param size size of block (including header & footer tags), in bytes
/// @retval void* pointer to header of large enough free block
/// @retval NULL if no free block of the requested size is avilable
static void* bf_get_free_block(size_t size)
{
  LOG(1, "bf_get_free_block(0x%lx (%lu))", size, size);

  assert(mm_initialized);

  //
  // TODO
  //
  unsigned long best_size = ~0; // maxium size value
  void *best_ptr = NULL;
  void *curr = heap_start;
  while(curr < heap_end) { // until heap_end
    if (!GET_STATUS(curr) && GET_SIZE(curr) >= size && best_size >= size) {
      // update best free block
      best_size = size;
      best_ptr = curr;
    }
    curr += GET_SIZE(curr);
  }
  return best_ptr;
}

/// @}

void mm_setloglevel(int level)
{
  mm_loglevel = level;
}


void mm_check(void)
{
  assert(mm_initialized);

  void *p;
  char *apstr;
  if (get_free_block == ff_get_free_block) apstr = "first fit";
  else if (get_free_block == nf_get_free_block) apstr = "next fit";
  else if (get_free_block == bf_get_free_block) apstr = "best fit";
  else apstr = "invalid";

  LOG(2, "  allocation policy    %s\n", apstr);
  printf("\n----------------------------------------- mm_check ----------------------------------------------\n");
  printf("  ds_heap_start:          %p\n", ds_heap_start);
  printf("  ds_heap_brk:            %p\n", ds_heap_brk);
  printf("  heap_start:             %p\n", heap_start);
  printf("  heap_end:               %p\n", heap_end);
  printf("  allocation policy:      %s\n", apstr);
  printf("  next_block:             %p\n", nf_curr);   // this will be needed for the next fit policy

  printf("\n");
  p = PREV_PTR(heap_start);
  printf("  initial sentinel:       %p: size: %6lx (%7ld), status: %s\n",
         p, GET_SIZE(p), GET_SIZE(p), GET_STATUS(p) == ALLOC ? "allocated" : "free");
  p = heap_end;
  printf("  end sentinel:           %p: size: %6lx (%7ld), status: %s\n",
         p, GET_SIZE(p), GET_SIZE(p), GET_STATUS(p) == ALLOC ? "allocated" : "free");
  printf("\n");
  printf("  blocks:\n");

  long errors = 0;
  p = heap_start;
  while (p < heap_end) {
    TYPE hdr = GET(p);
    TYPE size = SIZE(hdr);
    TYPE status = STATUS(hdr);
    printf("    %p: size: %6lx (%7ld), status: %s\n", 
           p, size, size, status == ALLOC ? "allocated" : "free");

    void *fp = p + size - TYPE_SIZE;
    TYPE ftr = GET(fp);
    TYPE fsize = SIZE(ftr);
    TYPE fstatus = STATUS(ftr);

    if ((size != fsize) || (status != fstatus)) {
      errors++;
      printf("    --> ERROR: footer at %p with different properties: size: %lx, status: %lx\n", 
             fp, fsize, fstatus);
    }

    p = p + size;
    if (size == 0) {
      printf("    WARNING: size 0 detected, aborting traversal.\n");
      break;
    }
  }

  printf("\n");
  if ((p == heap_end) && (errors == 0)) printf("  Block structure coherent.\n");
  printf("-------------------------------------------------------------------------------------------------\n");
}
