import os, psutil

import hashtable

def func():
    ht = hashtable.hashtable(500000)
    for i in range(1000000):
        ht.insert(str.encode('string{}'.format(i)))

#    assert(ht.search(b'string1') == True)
#    assert(ht.search(b'string101') == False)
#    assert(ht.remove(b'string1') == True)
#    assert(ht.search(b'string1') == False)
    for i in range(100):
        k = 'string{}'.format(i)
        _ = ht.search(str.encode('string{}'.format(i)))

    k =0
    for v in ht:
        k += 1

    process = psutil.Process()
    print(process.memory_info().rss/1024/1024.)  # in bytes 

if __name__ == '__main__':
    func()
