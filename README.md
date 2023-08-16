Fast hashtable with minimal memory imprint.

## Usage
```python3
ht = hashtable.hashset()

# fill hashset
for i in range(10):
    ht.add('string{}'.format(i))

assert(len(ht) == 10)
assert('string1' in ht)

# print values in set
for v in ht:
    print(v)
```