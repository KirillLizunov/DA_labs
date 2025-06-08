#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>       
#include <cstdint>
#include <fstream>
#include <stdexcept>

class AVLTree {
private:
    struct Node {
        std::string key;
        uint64_t    value;
        int         height;
        Node*       left;
        Node*       right;
        Node(const std::string& k, uint64_t v)
            : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root = nullptr;

    static std::string toLower(const std::string& s) {
        std::string res = s;
        std::transform(res.begin(), res.end(), res.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return res;
    }

    int height(Node* n) const {
        if (n != nullptr) return n->height;
        else              return 0;
    }

    int balanceFactor(Node* n) const {
        return height(n->right) - height(n->left);
    }

    void updateHeight(Node* n) {
        n->height = 1 + std::max(height(n->left), height(n->right));
    }

    Node* rotateRight(Node* y) {
        Node* x  = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left  = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y  = x->right;
        Node* T2 = y->left;
        y->left  = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* balance(Node* n) {
        updateHeight(n);
        int bf = balanceFactor(n);
        if (bf < -1) {
            if (balanceFactor(n->left) > 0)
                n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (bf > 1) {
            if (balanceFactor(n->right) < 0)
                n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    Node* insertNode(Node* n, const std::string& k, uint64_t v) {
        if (!n) return new Node(k, v);
        if (k < n->key)
            n->left  = insertNode(n->left,  k, v);
        else if (k > n->key)
            n->right = insertNode(n->right, k, v);
        else
            throw std::logic_error("Exist");
        return balance(n);
    }

    Node* minValueNode(Node* n) const {
        while (n->left) n = n->left;
        return n;
    }

    Node* removeNode(Node* n, const std::string& k) {
        if (!n) throw std::logic_error("NoSuchWord");
        if (k < n->key)
            n->left = removeNode(n->left, k);
        else if (k > n->key)
            n->right = removeNode(n->right, k);
        else {
            if (!n->left || !n->right) {
                Node* t = n->left ? n->left : n->right;
                delete n;
                return t;
            }
            Node* succ = minValueNode(n->right);
            n->key   = succ->key;
            n->value = succ->value;
            n->right = removeNode(n->right, succ->key);
        }
        return balance(n);
    }

    Node* findNode(Node* n, const std::string& k) const {
        if (!n) return nullptr;
        if (k < n->key)      return findNode(n->left,  k);
        else if (k > n->key) return findNode(n->right, k);
        else                  return n;
    }

    void clear(Node* n) {
        if (!n) return;
        clear(n->left);
        clear(n->right);
        delete n;
    }

    uint64_t countNodes(Node* n) const {
        if (!n) return 0;
        return 1 + countNodes(n->left) + countNodes(n->right);
    }

    // In-order обход
    void inorderSave(Node* n, std::ofstream& out) const {
        if (!n) return;
        inorderSave(n->left, out);
        uint16_t len = static_cast<uint16_t>(n->key.size());
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(n->key.data(), len);
        out.write(reinterpret_cast<const char*>(&n->value), sizeof(n->value));
        inorderSave(n->right, out);
    }

public:
    ~AVLTree() { clear(root); }

    bool insert(const std::string& key, uint64_t value) {
        auto k = toLower(key);
        try {
            root = insertNode(root, k, value);
            return true;
        } catch (std::logic_error&) {
            return false;
        }
    }

    bool remove(const std::string& key) {
        auto k = toLower(key);
        try {
            root = removeNode(root, k);
            return true;
        } catch (std::logic_error&) {
            return false;
        }
    }

    bool find(const std::string& key, uint64_t& out) const {
        auto k = toLower(key);
        Node* p = findNode(root, k);
        if (p) { out = p->value; return true; }
        return false;
    }


    void save(const std::string& path) const {
        std::ofstream out(path, std::ios::binary);
        if (!out) throw std::runtime_error("ERROR: cannot open file for writing");
        uint64_t cnt = countNodes(root);
        out.write(reinterpret_cast<const char*>(&cnt), sizeof(cnt));
        inorderSave(root, out);
        if (out.fail()) throw std::runtime_error("ERROR: write failure");
    }


    void load(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) throw std::runtime_error("ERROR: cannot open file for reading");

        uint64_t cnt;
        in.read(reinterpret_cast<char*>(&cnt), sizeof(cnt));
        if (!in) throw std::runtime_error("ERROR: invalid format");

        AVLTree tmp;
        for (uint64_t i = 0; i < cnt; ++i) {
            uint16_t len;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (!in) throw std::runtime_error("ERROR: invalid format");
            std::string key(len, '\0');
            in.read(&key[0], len);
            if (!in) throw std::runtime_error("ERROR: invalid format");
            uint64_t val;
            in.read(reinterpret_cast<char*>(&val), sizeof(val));
            if (!in) throw std::runtime_error("ERROR: invalid format");
            tmp.root = tmp.insertNode(tmp.root, key, val);
        }

        clear(root);
        root = tmp.root;
        tmp.root = nullptr;
    }
};

int main() {
    AVLTree tree;
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "+") {
            std::string w; uint64_t v;
            std::cin >> w >> v;
            std::cout << (tree.insert(w,v) ? "OK\n" : "Exist\n");
        }
        else if (cmd == "-") {
            std::string w;
            std::cin >> w;
            std::cout << (tree.remove(w) ? "OK\n" : "NoSuchWord\n");
        }
        else if (cmd == "!") {
            std::string op, path;
            std::cin >> op >> path;
            try {
                if (op == "Save")      { tree.save(path); std::cout << "OK\n"; }
                else if (op == "Load") { tree.load(path); std::cout << "OK\n"; }
                else std::cout << "ERROR: unknown operation\n";
            } catch (std::runtime_error& e) {
                std::cout << e.what() << "\n";
            }
        }
        else {
            uint64_t v;
            if (tree.find(cmd, v))
                std::cout << "OK: " << v << "\n";
            else
                std::cout << "NoSuchWord\n";
        }
    }
    return 0;
}
