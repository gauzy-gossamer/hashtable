# distutils: language = c
#cython: language_level=3

cdef extern from "stdlib.h":
    void free(void *ptr)

cdef extern from "hashtable.h":
    ctypedef struct hash_table_t:        
        char **slots
        int num_slots
        int records
    hash_table_t *init_table(int num_slots)
    void *hash_search(hash_table_t *table, char *key)
    void *hash_insert(hash_table_t *table, char *key)
    void *hash_remove(hash_table_t *table, char *key)
    void hash_destroy(hash_table_t *table)
    void iter_init(hash_table_t *table)
    char *iter_next(hash_table_t *table)


cdef class hashtable(object):
    cdef hash_table_t* table
    cdef void ** ret
    def __init__(self, num_slots = 50000):
        self.table = init_table(num_slots)

    def insert(self, char *record):
        hash_insert(self.table, record)

    def search(self, char *record):
        ret = hash_search(self.table, record)
        return ret != NULL

    def __len__(self):
        return self.table.records
    records = __len__

    def remove(self, char *record):
        ret = hash_remove(self.table, record)
        return ret != NULL

    def __dealloc__(self):
        hash_destroy(self.table)

    def __iter__(self):
        iter_init(self.table);
        return self

    def __next__(self):
        cdef void *v = iter_next(self.table);
        if v == NULL:
            raise StopIteration
        ret = str(<char*>v, 'utf-8')
        free(<void*>v)
        return ret
