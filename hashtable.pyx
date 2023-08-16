# distutils: language = c
#cython: language_level=3

from cython.operator cimport dereference as deref

cdef extern from "stdlib.h":
    void free(void *ptr)
    char *strdup(char *v)

cdef extern from "hashtable.h":
    ctypedef struct hash_table_t:        
        char **slots
        int num_slots
        int records
    hash_table_t *init_table(int num_slots)
    void **hash_search(hash_table_t *table, char *key)
    void **hash_insert(hash_table_t *table, char *key)
    void *hash_remove(hash_table_t *table, char *key)
    void hash_destroy(hash_table_t *table)
    void iter_init(hash_table_t *table)
    char *iter_next(hash_table_t *table)

cdef class _hashbase(object):
    cdef hash_table_t* table
    cdef int changed
    cdef void ** ret
    def __init__(self, num_slots = 50000):
        self.table = init_table(num_slots)

    def insert(self, str k):
        cdef bytes k_
        k_ = k.encode()
        hash_insert(self.table, k_)
        self.changed = True

    def search(self, str k):
        cdef bytes k_
        k_ = k.encode()
        ret = hash_search(self.table, k_)
        return ret != NULL

    __contains__ = search

    def __len__(self):
        return self.table.records
    records = __len__

    def __delitem__(self, str k):
        cdef bytes k_
        k_ = k.encode()
        ret = hash_remove(self.table, k_)
        self.changed = True
    remove = __delitem__

    def __dealloc__(self):
        hash_destroy(self.table)

    def __iter__(self):
        iter_init(self.table);
        self.changed = False
        return self

    def __next__(self):
        if self.changed:
            raise RuntimeError("dictionary changed size during iteration")
        cdef void *v = iter_next(self.table);
        if v == NULL:
            raise StopIteration
        ret = str(<char*>v, 'utf-8')
        free(<void*>v)
        return ret


cdef class hashtable(_hashbase):
    def __init__(self, num_slots = 50000):
        super().__init__(num_slots)

    def __setitem__(self, str k, str v):
        cdef bytes k_, v_
        cdef void **ret
        k_ = k.encode()
        v_ = v.encode()
        ret = hash_insert(self.table, k_)
        ret[0] = strdup(v_)
        self.changed = True

    def __getitem__(self, str k):
        cdef bytes k_ = k.encode()
        cdef void **ret
        ret = hash_search(self.table, k_)
        return str(<char*>deref(ret), 'utf-8')
    search = __getitem__

    insert = __setitem__


cdef class hashset(_hashbase):
    def __init__(self, num_slots = 50000):
        super().__init__(num_slots)

    def add(self, str k):
        cdef bytes k_
        k_ = k.encode()
        hash_insert(self.table, k_)
        self.changed = True
