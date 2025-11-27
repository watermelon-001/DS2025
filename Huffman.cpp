#include <bits/stdc++.h>
using namespace std;

class Bitmap {
private:
    unsigned char* M;
    int N, _sz;

    void init(int n) {
        M = new unsigned char[N = (n + 7) / 8];
        memset(M, 0, N);
        _sz = 0;
    }

public:
    Bitmap(int n = 8) { init(n); }
    ~Bitmap() { delete[] M; }

    void expand(int k) {
        if (k < 8 * N) return;
        int oldN = N;
        unsigned char* oldM = M;
        init(2 * k);
        memcpy(M, oldM, oldN);
        delete[] oldM;
    }

    void set(int k) {
        if (k >= 8 * N) expand(k);
        M[k >> 3] |= (0x80 >> (k & 7));
        _sz++;
    }

    bool test(int k) const {
        return M[k >> 3] & (0x80 >> (k & 7));
    }
};

template <typename T>
struct BinNode {
    T data;
    BinNode *lc, *rc, *parent;
    BinNode(T v, BinNode* p=nullptr) : data(v), lc(nullptr), rc(nullptr), parent(p) {}
};

template <typename T>
class BinTree {
public:
    BinNode<T>* _root;
    int _size;
    BinTree(): _root(nullptr), _size(0) {}
    BinNode<T>* insertAsRoot(T const& e) {
        _size = 1;
        return _root = new BinNode<T>(e);
    }
};

struct HuffNode {
    char ch;
    int freq;
    HuffNode *lc, *rc;
    HuffNode(char c, int f) : ch(c), freq(f), lc(nullptr), rc(nullptr) {}
    HuffNode(HuffNode* a, HuffNode* b)
        : ch(0), freq(a->freq + b->freq), lc(a), rc(b) {}
};

struct cmp {
    bool operator()(HuffNode* a, HuffNode* b) {
        return a->freq > b->freq;
    }
};

void generateCodes(HuffNode* root, string path, unordered_map<char,string>& code) {
    if (!root) return;
    if (root->ch != 0) code[root->ch] = path;
    generateCodes(root->lc, path + "0", code);
    generateCodes(root->rc, path + "1", code);
}

int main() {
    unordered_map<char,int> freq = {
        {'a', 177},{'b',20},{'c',38},{'d',86},{'e',280},{'f',52},{'g',42},
        {'h',186},{'i',191},{'j',2},{'k',6},{'l',88},{'m',59},{'n',165},
        {'o',178},{'p',33},{'q',2},{'r',135},{'s',125},{'t',271},{'u',63},
        {'v',29},{'w',87},{'x',2},{'y',33},{'z',1}
    };

    priority_queue<HuffNode*, vector<HuffNode*>, cmp> pq;
    for (auto &p : freq) pq.push(new HuffNode(p.first, p.second));

    while (pq.size() > 1) {
        HuffNode* a = pq.top(); pq.pop();
        HuffNode* b = pq.top(); pq.pop();
        pq.push(new HuffNode(a, b));
    }
    HuffNode* root = pq.top();

    unordered_map<char,string> code;
    generateCodes(root, "", code);

    cout << "===== Huffman Code Table =====\n";
    for (auto &p : code) cout << p.first << " : " << p.second << endl;

    string word = "dream";
    cout << "\nHuffman encoding of \"" << word << "\": ";
    for (char c : word) cout << code[c];
    cout << endl;

    return 0;
}
