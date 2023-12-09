/* OPTIONAL: Pools to replace heap allocation
 * Optional: Pools can be used instead of the heap for mem_malloc. If
 * so, these should be defined here, in increasing order according to 
 * the pool element size.
 *
 * LWIP_MALLOC_MEMPOOL(number_elements, element_size)
 */
LWIP_MALLOC_MEMPOOL_START
LWIP_MALLOC_MEMPOOL(50, 128)
LWIP_MALLOC_MEMPOOL(50, 256)
LWIP_MALLOC_MEMPOOL(24, 512)
LWIP_MALLOC_MEMPOOL(10, 1024)
LWIP_MALLOC_MEMPOOL(320, 1536)
LWIP_MALLOC_MEMPOOL_END

/* Optional: Your custom pools can go here if you would like to use
 * lwIP's memory pools for anything else. For example,
 * LWIP_MEMPOOL(SYS_MBOX, 22, 100, "SYS_MBOX")
 */

