#include <bits/stdc++.h>

using namespace std;
using ll = long long;

const ll SIZE = 100000;
const int BUCKETS = 16;

struct PairKV {
    char key[33];
    char value[65];
};

struct Anivector {
    PairKV* data;
    int size;
    int capacity;
};

void AnivectorInit(Anivector* v) {
    v->data = nullptr;
    v->size = 0;
    v->capacity = 0;
}

void Reserve(Anivector* v, int new_capacity) {
    if (new_capacity <= v->capacity) { 
        return; 
    }
    PairKV* new_data = new PairKV[new_capacity];
    if (v->data) {
        memcpy(new_data, v->data, v->size * sizeof(PairKV));
        delete[] v->data;
    }
    v->data = new_data;
    v->capacity = new_capacity;
}

void PushBack(Anivector* v, const PairKV& item) {
    if (v->size == v->capacity) {
        int new_cap = (v->capacity == 0) ? 16 : v->capacity * 2;
        Reserve(v, new_cap);
    }
    v->data[v->size++] = item;
}

void Destroy(Anivector* v) {
    delete[] v->data;
    v->data = nullptr;
    v->size = 0;
    v->capacity = 0;
}

int HexToDecimal(char c) {
    switch(c) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': return 10;
        case 'b': return 11;
        case 'c': return 12;
        case 'd': return 13;
        case 'e': return 14;
        case 'f': return 15;
    }
}

void RadixSort(PairKV a[], int n) {
    PairKV* tmp = new PairKV[n];
    for (int p = 31; p >= 0; --p) {
        int count[BUCKETS] = {0};
        for (int i = 0; i < n; ++i) {
            int b = HexToDecimal(a[i].key[p]);
            count[b]++;
        }
        int prefix[BUCKETS] = {0};
        prefix[0] = count[0];
        for (int i = 1; i < BUCKETS; ++i) {
            prefix[i] = prefix[i - 1] + count[i];
        }
        for (int i = n - 1; i >= 0; --i) {
            int idx = HexToDecimal(a[i].key[p]);
            tmp[prefix[idx] - 1] = a[i];
            prefix[idx]--;
        }
        memcpy(a, tmp, n * sizeof(PairKV));
    }
    delete[] tmp;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    Anivector arr;
    AnivectorInit(&arr);
    char key[33] = {0};
    char value[65] = {0};
    PairKV pp;
    while(scanf("%s\t%s", pp.key, pp.value) != EOF) {
        PushBack(&arr, pp);
    }

    RadixSort(arr.data, arr.size);

    for(int i = 0; i < arr.size; ++i) {
        printf("%s\t%s\n", arr.data[i].key, arr.data[i].value);
    }
    Destroy(&arr);
    return 0;
}