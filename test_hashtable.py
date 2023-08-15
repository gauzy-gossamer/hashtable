import os, psutil

import hashtable

def test_hashtable():
    ht = hashtable.hashtable(500000)
    items = 1000000
    for i in range(items):
        ht.insert('string{}'.format(i))

    assert(len(ht) == items)
    assert('string1' in ht)
    assert('strin' not in ht)
    ht.remove('string1')
    assert('string1' not in ht)
    for i in range(100):
        k = 'string{}'.format(i)
        _ = ht.search('string{}'.format(i))

    k = 0
    for v in ht:
        k += 1
    assert(k == len(ht))

    process = psutil.Process()
    print(process.memory_info().rss/1024/1024.)  # in bytes 

if __name__ == '__main__':
    test_hashtable()
