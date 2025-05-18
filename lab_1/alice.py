class Pair:
    def __init__(self, key='', value=''):
        self.key = key
        self.value = value

class AniVector:
    def __init__(self):
        self.data = []
        self.size = 0
        self.capacity = 0

    def reserve(self, new_capacity):
        if new_capacity <= self.capacity:
            return
        self.data.extend([Pair() for _ in range(new_capacity - self.capacity)])
        self.capacity = new_capacity

    def push_back(self, item):
        if self.size == self.capacity:
            new_cap = 4 if self.capacity == 0 else self.capacity * 2
            self.reserve(new_cap)
        self.data[self.size] = item
        self.size += 1

    def destroy(self):
        self.data = []
        self.size = 0
        self.capacity = 0

def f(c):
    if c.isdigit():
        return int(c)
    return ord(c) - ord('a') + 10

def radix_sort(arr):
    n = len(arr)
    tmp = [Pair() for _ in range(n)]
    
    for p in range(31, -1, -1):
        count = [0] * 16
        
        for i in range(n):
            b = f(arr[i].key[p])
            count[b] += 1
        
        prefix = [0] * 16
        sum_count = 0
        for b in range(16):
            prefix[b] = sum_count
            sum_count += count[b]
        
        for i in range(n):
            b = f(arr[i].key[p])
            pos = prefix[b]
            tmp[pos] = arr[i]
            prefix[b] += 1
        
        arr[:] = tmp[:]

def main():
    arr = AniVector()
    arr.reserve(4)
    while True:
        try:
            line = input().strip()
            if not line:
                break
            key, value = line.split('\t')
            pp = Pair(key, value)
            arr.push_back(pp)
        except EOFError:
            break

    radix_sort(arr.data[:arr.size])

    for i in range(arr.size):
        print(f"{arr.data[i].key}\t{arr.data[i].value}")

    arr.destroy()

if __name__ == "__main__":
    main()