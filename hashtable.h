#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#define _32_BYTES 32
#define _64_BYTES 64

typedef struct {
    char **slots;
    uint32_t num_slots;
    uint32_t records;
    int iter_slot;
    int iter_shift;
} hash_table_t;

void * 
zmalloc (int bytes) {
	void *mem;
	mem = malloc(bytes);
	if (mem == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(1);
	}
	memset(mem, 0, bytes);
	return mem;
}

/* this implementation is 
 *
 *
 */

/*******************************************************************************
 * // Begin statement                                                          *
 *                                                                             *
 * Permission to use this software is granted provided that this statement     *
 * is retained.                                                                *
 *                                                                             *
 * This software is for NON-COMMERCIAL use only!                               *
 *                                                                             *
 * If you would like to use any part of my software in a commercial or public  *
 * environment/product/service, let me know (askitisn@gmail.com) before        *
 * you use it (this statement will also need to be retained along with other   *
 * details), so I can grant you permission.                                    *
 *                                                                             *
 *                                                                             *
 * Please DO NOT distribute my software (in any manner), in part or in whole.  *
 * Thank you.                                                                  *
 *                                                                             *
 * Developer: Dr. Nikolas Askitis                                              *
 *                                                                             *
 * Website:   http://www.naskitis.com                                          *
 * Email:     askitisn@gmail.com                                               *
 * Paper(s):  'Efficient Data Structures for Cache Architectures', PhD 2007    *
 *            'Cache-Conscious Collision Resolution in String Hash Tables',    *
 *            SPIRE 2005:  www.springerlink.com/index/b61721172558qt03.pdf     *
 *                                                                             *
 * Please show your support by visiting:      www.naskitis.com                 *
 *                                                                             *
 * Copyright @ 2010.  All rights reserved.                                     *
 * This program is distributed without any warranty; without even the          *
 * implied warranty of merchantability or fitness for a particular purpose.    *
 *                                                                             *
 *                                                                             *
 * Array hash table in C for 64-bit Linux-based architectures.                 *
 *                                                                             *
 * Below is my original implementation of the array hash. See my PhD Thesis    *
 * for more details, found at naskitis.com.                                    *
 *                                                                             *
 * If you have any questions about the source or the workings of the array     *
 * hash, feel free to contact me.                                              *
 *                                                                             *
 * Compile command: gcc -ansi -pedantic -O3 -fomit-frame-pointer ...           *
 *                                                                             *
 * Compiler version used: gcc (Ubuntu/Linaro 4.4.4-14ubuntu5) 4.4.5            *
 * O/S used: Linux ubuntu 2.6.35-22-generic #33-Ubuntu SMP Sun Sep 19 20:32:27 *
 * UTC 2010 x86_64 GNU/Linux                                                   *
 *                                                                             *
 * Developed and implemented by Dr. Nikolas Askitis, April 2004 for            *
 * Linux-based 64-bit platforms.                                               *
 *                                                                             *
 * // End statement                                                            *
 ******************************************************************************/

#define MEMORY_EXHAUSTED "memory failure"

static void fatal(char *str) { puts(str); exit(1); }

uint32_t NUM_SLOTS = 231071;
uint64_t hash_mem = 0;

/* FNV hash function */
#define FNV_HV 0x811c9dc5

static inline uint32_t
bitwise_hash(char *str, int num_slots) {
    uint32_t hval = FNV_HV;
    while (*str) {
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
        hval ^= *str++;
    }
    return hval % num_slots;
}

/* copy a block of memory, a word at a time */
static void 
node_cpy(uint32_t *dest, uint32_t *src, uint32_t bytes) {
    bytes = bytes >> 2;
    while (bytes != 0) {
        *dest++ = *src++; 
        bytes--;
    }   
}

/* resize a slot entry in the hash table */
static void 
resize_array(char **hash_table, uint32_t idx, uint32_t array_offset, uint32_t required_increase) {
  /* if there is no slot, then create one with respect to the growth policy used */
  if (array_offset == 0) {
#ifdef EXACT_FIT
   
    if ((*(hash_table + idx) = malloc(required_increase)) == NULL) 
        fatal(MEMORY_EXHAUSTED);
    
    /* keep track of the amount of memory allocated */
    hash_mem += required_increase + 16;  
   
#else    

    /* otherwise, grow the array with paging */
    /* if the required space is less than 32 bytes, than allocate a 32 byte block */
    if (required_increase <= _32_BYTES) {
      if ((*(hash_table + idx) = (char *) malloc(_32_BYTES)) == NULL) fatal(MEMORY_EXHAUSTED);  
      hash_mem += _32_BYTES + 16;  
    }
    /* otherwise, allocate as many 64-byte blocks as required */
    else
    {
      uint32_t number_of_blocks = ((int)( (required_increase-1) >> 6)+1);   

      if ((*(hash_table + idx) = (char *) malloc(number_of_blocks << 6)) == NULL) fatal(MEMORY_EXHAUSTED);
         
	  /* keep track of the amount of memory allocated */ 
      hash_mem += (number_of_blocks << 6) + 16; 
    }
    
#endif
  } else {
    /* otherwise, a slot entry (an array) is found which must be resized */
#ifdef EXACT_FIT

    char *tmp = malloc(array_offset + required_increase);
    if (tmp == NULL) fatal (MEMORY_EXHAUSTED);
    
    /* copy the existing array into the new one */
    memcpy(tmp, *(hash_table + idx), array_offset + 1);
    
    /* free the old array and assign the slot pointer to the new array */ 
    free( *(hash_table + idx) );
    *(hash_table + idx) = tmp;
 
    /* keep track of the amount of memory allocated */
    hash_mem = hash_mem - 1 + required_increase; 
    
    /* else grow the array in blocks or pages */
#else 
    
    uint32_t old_array_size = array_offset + 1;
    uint32_t new_array_size = (array_offset+required_increase);
    
    /* if the new array size can fit within the previously allocated 32-byte block, 
     * then no memory needs to be allocated.
     */
    if ( old_array_size <= _32_BYTES  &&  new_array_size <= _32_BYTES ) {
      return;

    /* if the new array size can fit within a 64-byte block, then allocate only a
     * single 64-byte block. */
    } else if ( old_array_size <= _32_BYTES  &&  new_array_size <= _64_BYTES) {
      char *tmp = malloc(_64_BYTES);
      if(tmp == NULL) fatal (MEMORY_EXHAUSTED);
      
      /* copy the old array into the new */
      memcpy( tmp,  *(hash_table + idx), old_array_size);
      
      /* delete the old array */ 
      free( *(hash_table + idx) );

      /* assign the slot pointer to the new array */
      *(hash_table + idx) = tmp;

      /* accumulate the amount of memory allocate */
      hash_mem += _32_BYTES;
   
      return;
    /* if the new array size can fit within a 64-byte block, then return */
    } else if (old_array_size <= _64_BYTES && new_array_size <= _64_BYTES ) { 
      return;
    /* resize the current array by as many 64-byte blocks as required */
    } else {
      uint32_t number_of_blocks = ((int)( (old_array_size - 1) >> 6) + 1);
      uint32_t number_of_new_blocks = ((int)( (new_array_size - 1) >> 6) + 1);

      if (number_of_new_blocks > number_of_blocks) {
        /* allocate as many blocks as required */
        char *tmp = malloc(number_of_new_blocks << 6);
        if (tmp == NULL) fatal(MEMORY_EXHAUSTED);
        
        /* copy the old array, a word at a time, into a new array */
        node_cpy( (uint32_t *) tmp, (uint32_t *) *(hash_table + idx), number_of_blocks<<6); 
        
        /* free the old array */
        free( *(hash_table + idx) );
        
        /* assign the slot pointer to the new array */
        *(hash_table + idx) = tmp;

        /* keep track of the number of bytes allocated */
        hash_mem += ((number_of_new_blocks - number_of_blocks) << 6); 
      } 
    } 
#endif 
  }
}

/* 
 * checks whether a string exists in the hash table. 1 is returned 
 * if the string is found, 0 otherwise.
 * it return pointer to pointer, to which you can save any data;
 */
void *
hash_search(hash_table_t *table, char *query_start) {
    char **hash_table = table->slots;
    uint32_t register len = 0;
    char *array, *query = query_start;
    char *word_start;

    /* hash the query term using the bitwise hash function to acquire a slot */   
    if ((array = *(hash_table + bitwise_hash(query_start, table->num_slots)) ) == NULL) {
        return NULL;
    }

    /* the main search loop */
loop:

    query = query_start; 

    /* 
    * strings are length-encoded.  The first byte of each string is its length.  If however,
    * the most significant bit is set, this means that the length of the string is greater than 
    * 127 characters, and so, the next byte also represents the string length. In which case, the 
    * first byte is moved into an integer, followed by the second byte, thus forming the string length. 
    */   
    if ((len = (unsigned int) *array) >= 128) {
        len = (unsigned int) ( ( *array & 0x7f ) << 8 ) | (unsigned int) ( *(++array) & 0xff );
    }

    /* 
    * once the length has been acquired, move to the next byte which represents the
    * first character of the string or the end-of-bucket flag (a null character)
    */
    array++;

    word_start = array;

    /* 
    * compare the query to the word in the array, a character a time until a mismatch
    * occurs or a null character is encountered 
    */   
    for (; *query != '\0' && *query == *array; query++, array++);

    /* 
    * if every character of the query string was compared and the length of
    * the query matches the length of the string compared, then its a match
    */
    if (*query == '\0' && (array - word_start) == len)  {
        return array;
    }

    /* a mismatch occurred during the string comparison phase. skip to the next word */
    array = word_start + len + sizeof(void*);

    /* if the next character is the end-of-bucket flag, then the search failed */
    if (*array == '\0') {
        return NULL;
    }

    /* otherwise, jump back up to the main search loop */
    goto loop; 
}


/* 
 * This function checks whether the string exists in the hash table.  If it does not exist,
 * then it can be inserted.  The function returns 0 on insertion failure, that is,
 * when the string is found, and 1 on successful insertion.
 */
void *
hash_insert(hash_table_t *table, char *query_start) {
    char **hash_table = table->slots;
    uint32_t register len, idx;
    uint32_t array_offset;
    char *array, *array_start, *query = query_start;
    char *word_start;

    /* hash the query term to get the required slot */         
    idx = bitwise_hash(query_start, table->num_slots);

    /* access the slot, if the slot is empty then proceed directly
    * to insert the string 
    */
    if ((array = *(hash_table + idx)) == NULL) {
        array_start = array;
        goto insert;
    }

    array_start = array;

    /* main search loop */
loop:

    query = query_start; 

   /* 
    * strings are length-encoded.  the first byte of each string is its length.  if however,
    * the most significant bit is set, this means that the length of the string is greater than 
    * 127 characters, and so, the next byte also represents the string length. in which case, the 
    * first byte is moved into an integer, followed by the second byte, thus forming the string length. 
    */   
    if ((len = (unsigned int) *array ) >= 128) {
        len = (unsigned int) ( ( *array & 0x7f ) << 8 ) | (unsigned int) ( *(++array) & 0xff );
    }
   
   /* 
    * once the length has been acquired, move to the next byte which represents the
    * first character of the string or the end-of-bucket flag (a null character)
    */
    array++;

    word_start = array;

   /* 
    * compare the query to the word in the array, a character a time until a mismatch
    * occurs or a null character is encountered 
    */ 
    for (; *query != '\0' && *query == *array; query++, array++);

    /* 
    * if every character of the query string was compared and the length of
    * the query matches the length of the string compared, then its a match
    */
    if ( *query == '\0' && (array - word_start) == len ) {
        return array;
    }

    /* a mismatch occurred during the string comparison phase. skip to the next word */
    array = word_start + len + sizeof(void*);

    /* if the next character is the end-of-bucket flag, then the search failed */
    if (*array == '\0')  {
        goto insert;
    }
   
    goto loop; 

insert:

    /* get the length of the string to insert */
    for (; *query != '\0'; query++);   
    len = query - query_start;

    /* get the size of the array */
    array_offset = array - array_start;

    /* resize the array to fit the new string */
    resize_array(hash_table, idx, array_offset, ( len < 128 ) ? len + 2 + sizeof(void*) : len + 3 + sizeof(void*));

    /* reinitialize the array pointers, the point to the end of the array */
    array = *(hash_table + idx);
    array_start = array;
    array += array_offset;
     
   /* if the length of the string is less than 128 characters, then only a single byte is
    * needed to store its length
    */
    if (len < 128) {
        *array = (char) len;
    /* if the length of the string is greater than 128 characters, then two bytes are required to
    * store the string 
    */
    } else { 
        *array     = (char) ( len >> 8) | 0x80;
        *(++array) = (char) ( len ) & 0xff; 
    }
    array++;

    /* copy the string into the array */
    while (*query_start != '\0') {
        *array++ = *query_start++;
    }
    char *ret = array;
    memset(array, 0, sizeof(void*));
    array += sizeof(void*); 

    /* make sure the array is null terminated */
    *array = '\0';
    table->records++;

    return ret;
}

/* removes the record 
* if the record is found, a pointer to data is returned
* ! Note that this function returns a pointer to the static local variable,
* so after another record is removed, the pointer to the previous record 
* is no longer accessible.
*/
void *
hash_remove(hash_table_t *table, char *query_start) {
    char **hash_table = table->slots;
    uint32_t register len, idx;
//    uint32_t array_offset;
    char *array, *array_start, *query = query_start;
    char *word_start;

    /* hash the query term to get the required slot */         
    idx = bitwise_hash(query_start, table->num_slots);

    /* no record found */
    if ((array = *(hash_table + idx)) == NULL) {
        array_start = array;
        return NULL;
    }

    array_start = array;
    char *found = NULL, *record_start = NULL;
    uint32_t record_len = 0;

    /* main search loop */
loop:

    record_start = array;
    query = query_start; 

    if ((len = (unsigned int) *array ) >= 128) {
        len = (unsigned int) ( ( *array & 0x7f ) << 8 ) | (unsigned int) ( *(++array) & 0xff );
    }

   
   /* 
    * once the length has been acquired, move to the next byte which represents the
    * first character of the string or the end-of-bucket flag (a null character)
    */
    array++;

    word_start = array;

    if (found) {
        array = word_start + len + sizeof(void*);
    
        if (*array == '\0') {
            goto swap;
        }
        goto loop;
    }

   /* 
    * compare the query to the word in the array, a character a time until a mismatch
    * occurs or a null character is encountered 
    */ 
    for (; *query != '\0' && *query == *array; query++, array++);

    /* 
    * if every character of the query string was compared and the length of
    * the query matches the length of the string compared, then its a match
    */
    if (*query == '\0' && (array - word_start) == len) {
        found = record_start;
        record_len = array - record_start;
    }

    /* a mismatch occurred during the string comparison phase. skip to the next word */
    array = word_start + len + sizeof(void*);

    /* if the next character is the end-of-bucket flag, then goto swap */
    if (*array == '\0')  {
        goto swap;
    }
   
    goto loop; 

swap:

    if (!found) {
        return NULL;
    }

    void **ptr = (found + record_len);
    if (*ptr != NULL) {
        free(*ptr);
    }

    table->records--;

    char *record_end = found + record_len + sizeof(void*);
    
    if (found == array_start && record_end == array) {
        free(array_start);
        *(hash_table + idx) = NULL;
        return NULL;
    }

    memcpy(found, record_end , array - record_end + 1);

    return NULL;
}

void
iter_init(hash_table_t *table) {
    table->iter_slot = 0;
    table->iter_shift = 0;
}

char *
iter_next(hash_table_t *table) {
    int i = table->iter_slot;
    char **hash_table = table->slots;
    for (; i < table->num_slots; i++)  {
        table->iter_slot = i;
        if (*(hash_table + i) == NULL) {
            continue;
        }
        int len;
        char *array = *(hash_table + i);
        array += table->iter_shift;
        for (;;) {
            if (*array == '\0') {
                table->iter_shift = 0;
                break;
            }
            if ((len = (unsigned int) *array) >= 128) {
                 len = (unsigned int) (( *array & 0x7f ) << 8) | (unsigned int) (*(++array) & 0xff);
            }
            table->iter_shift++;
            array++;

            char *key = zmalloc(len);
            strncpy(key, array, len+1);
            key[len] = '\0';

            array += len;
            table->iter_shift += len;

//            void **ptr;
//            ptr = array;
//            data_t *data = *ptr;
//            proc_data(key, data);
            array += sizeof(void*);
            table->iter_shift += sizeof(void*);
            return key;
        }
    }
    return NULL;
}

hash_table_t *
init_table(int num_slots) {
    if (num_slots == 0) {
        num_slots = NUM_SLOTS;
    }
    hash_mem += (sizeof(char *) * num_slots) + 8;
    hash_table_t *hash_table = (hash_table_t*)zmalloc(sizeof(hash_table_t));
  
    hash_table->num_slots = num_slots;
    hash_table->records = 0;
    hash_table->slots = (char **)calloc(num_slots, sizeof(char *));
    if (hash_table->slots == NULL) 
        fatal(MEMORY_EXHAUSTED);
    
    return hash_table;
}

/* frees up all the memory allocated by the slots of the array hash */
void
hash_destroy(hash_table_t *table) {
    char **hash_table = table->slots;
    uint32_t i = 0;
    int len;

    for (i = 0; i < table->num_slots; i++)  {
        if (*(hash_table + i) == NULL) {
            continue;
        }

        char *array = *(hash_table + i);
        for (;;) {
            if (*array == '\0') {
                break;
            }
            if ((len = (unsigned int) *array) >= 128) {
                 len = (unsigned int) (( *array & 0x7f ) << 8) | (unsigned int) (*(++array) & 0xff);
            }
            array += len + 1;

            void **ptr = (void*)array;
            if (*ptr != NULL) {
                free(*ptr);
            }
            array += sizeof(void*);
        }
        free( *(hash_table + i) );
    }
    free(table->slots);
    free(table);
}

#endif
