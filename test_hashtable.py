import hashtable

if __name__ == '__main__':
    ht = hashtable.hashtable()
    for i in range(100):
        ht.insert(str.encode('string{}'.format(i)))

    assert(ht.search(b'string1') == True)
    assert(ht.search(b'string101') == False)
