#include <iostream>
#include <vector>
#include <unordered_set>
#include <set>
#include <map>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;

// ---------------- UTILITAIRE ----------------
long long nowMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

template<typename F>
long long measure(F f) {
    long long start = nowMs();
    f();
    return nowMs() - start;
}

// ---------------- AVL TREE ----------------
struct AVL {
    struct Node {
        int key;
        Node* left;
        Node* right;
        int height;
        Node(int k): key(k), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root = nullptr;

    int h(Node* n) { return n ? n->height : 0; }

    int getBalance(Node* n) { return n ? h(n->left) - h(n->right) : 0; }

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;

        y->height = max(h(y->left), h(y->right)) + 1;
        x->height = max(h(x->left), h(x->right)) + 1;

        return x;
    }

    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;

        x->height = max(h(x->left), h(x->right)) + 1;
        y->height = max(h(y->left), h(y->right)) + 1;

        return y;
    }

    Node* insertNode(Node* node, int key) {
        if (!node) return new Node(key);

        if (key < node->key) node->left = insertNode(node->left, key);
        else if (key > node->key) node->right = insertNode(node->right, key);
        else return node;

        node->height = 1 + max(h(node->left), h(node->right));

        int balance = getBalance(node);

        if (balance > 1 && key < node->left->key)
            return rightRotate(node);

        if (balance < -1 && key > node->right->key)
            return leftRotate(node);

        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left != nullptr) current = current->left;
        return current;
    }

    Node* deleteNode(Node* root, int key) {
        if (!root) return root;

        if (key < root->key)
            root->left = deleteNode(root->left, key);

        else if (key > root->key)
            root->right = deleteNode(root->right, key);

        else {
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;

                if (!temp) {
                    temp = root;
                    root = nullptr;
                }
                else *root = *temp;

                delete temp;
            }
            else {
                Node* temp = minValueNode(root->right);
                root->key = temp->key;
                root->right = deleteNode(root->right, temp->key);
            }
        }

        if (!root) return root;

        root->height = 1 + max(h(root->left), h(root->right));

        int balance = getBalance(root);

        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);

        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);

        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    bool search(Node* node, int key) {
        if (!node) return false;
        if (key == node->key) return true;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }

    void insert(int key) { root = insertNode(root, key); }
    void erase(int key) { root = deleteNode(root, key); }
    bool contains(int key) { return search(root, key); }
};

// ---------------- RED-BLACK TREE SIMPLE ----------------
enum Color {RED, BLACK};
struct RBNode {
    int key;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    RBNode(int k): key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};
struct RBTree {
    RBNode* root = nullptr;

    RBNode* insertNode(RBNode* root, RBNode* node) {
        if (!root) return node;

        if (node->key < root->key) {
            root->left = insertNode(root->left, node);
            root->left->parent = root;
        }
        else if (node->key > root->key) {
            root->right = insertNode(root->right, node);
            root->right->parent = root;
        }
        return root;
    }

    void insert(int key) {
        RBNode* node = new RBNode(key);
        root = insertNode(root, node);
        node->color = BLACK;
    }

    bool search(RBNode* node, int key) {
        if (!node) return false;
        if (node->key == key) return true;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }

    bool contains(int key) { return search(root, key); }

    RBNode* minValueNode(RBNode* node) {
        while (node->left) node = node->left;
        return node;
    }

    RBNode* deleteNode(RBNode* root, int key) {
        if (!root) return nullptr;

        if (key < root->key)
            root->left = deleteNode(root->left, key);
        else if (key > root->key)
            root->right = deleteNode(root->right, key);
        else {
            if (!root->left) {
                RBNode* r = root->right;
                delete root;
                return r;
            }
            else if (!root->right) {
                RBNode* l = root->left;
                delete root;
                return l;
            }
            RBNode* m = minValueNode(root->right);
            root->key = m->key;
            root->right = deleteNode(root->right, m->key);
        }
        return root;
    }

    void erase(int key) { root = deleteNode(root, key); }
};

struct BenchmarkResult {
    string name;
    string complexity;
    double insertion;
    double search;
    double deletion;
};

// ---------------- MAIN ----------------
int main() {
    const int N = 4000000;
    vector<int> data(N);
    iota(data.begin(), data.end(), 0);
    shuffle(data.begin(), data.end(), mt19937(0));

    vector<BenchmarkResult> results;

    cout << "\n=========== BENCHMARK 4,000,000 ELEMENTS ===========\n";

    // unordered_set
    {
        unordered_set<int> us;
        double ins = measure([&](){ for (int v : data) us.insert(v); }) / 1000.0;
        double sea = measure([&](){ for (int v : data) us.find(v); }) / 1000.0;
        double del = measure([&](){ for (int v : data) us.erase(v); }) / 1000.0;

        cout << "\n[unordered_set]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        results.push_back({"unordered_set","O(1) amorti", ins, sea, del});
    }

    // set
    {
        set<int> s;
        double ins = measure([&](){ for (int v : data) s.insert(v); }) / 1000.0;
        double sea = measure([&](){ for (int v : data) s.find(v); }) / 1000.0;
        double del = measure([&](){ for (int v : data) s.erase(v); }) / 1000.0;

        cout << "\n[set STL]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        results.push_back({"set","O(log n)", ins, sea, del});
    }

    // map
    {
        map<int,int> m;
        double ins = measure([&](){ for (int v:data) m[v]=v; })/1000.0;
        double sea = measure([&](){ for (int v:data) m.find(v); })/1000.0;
        double del = measure([&](){ for (int v:data) m.erase(v); })/1000.0;

        cout << "\n[map STL]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        results.push_back({"map","O(log n)", ins, sea, del});
    }

    // AVL TREE
    {
        AVL avl;
        double ins = measure([&](){ for (int v:data) avl.insert(v); })/1000.0;
        double sea = measure([&](){ for (int v:data) avl.contains(v); })/1000.0;
        double del = measure([&](){ for (int v:data) avl.erase(v); })/1000.0;

        cout << "\n[AVL Tree]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        results.push_back({"AVL","O(log n)", ins, sea, del});
    }

    // Red-Black Tree
    {
        RBTree rbt;
        double ins = measure([&](){ for (int v:data) rbt.insert(v); })/1000.0;
        double sea = measure([&](){ for (int v:data) rbt.contains(v); })/1000.0;
        double del = measure([&](){ for (int v:data) rbt.erase(v); })/1000.0;

        cout << "\n[Red-Black Tree simple]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        results.push_back({"Red-Black Tree","O(log n) approximatif", ins, sea, del});
    }

    // fichier texte
    ofstream fout("benchmark_results.txt");
    fout << "========== BENCHMARK 4,000,000 ELEMENTS ==========\n\n";
    for(auto &r:results){
        fout << r.name << " (" << r.complexity << ")\n";
        fout << "Insertion: " << r.insertion << " s\n";
        fout << "Recherche: " << r.search << " s\n";
        fout << "Suppression: " << r.deletion << " s\n";
        fout << "----------------------------------------\n";
    }
    fout.close();

    cout << "\nBenchmark terminé. Résultats enregistrés dans 'benchmark_results.txt'.\n";

    return 0;
}
