import hashtable

def func():
    ht = hashtable.hashtable()
    for i in range(100):
        ht.insert(str.encode('string{}'.format(i)))

    assert(ht.search(b'string1') == True)
    assert(ht.search(b'string101') == False)
    assert(ht.remove(b'string1') == True)
    assert(ht.search(b'string1') == False)

if __name__ == '__main__':
    func()
