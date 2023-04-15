# distutils: language = c

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

    def records(self):
        return self.table.records

    def remove(self, char *record):
        ret = hash_remove(self.table, record)
        return ret != NULL

    def __dealloc__(self):
        hash_destroy(self.table)
