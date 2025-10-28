// exp1_part1_complex.cpp
#include <bits/stdc++.h>
using namespace std;

class Complex {
public:
    double real, imag;
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    double modulus() const { return sqrt(real*real + imag*imag); }
    bool operator==(const Complex& other) const {
        return real == other.real && imag == other.imag;
    }
};

bool cmpByModulus(const Complex& a, const Complex& b) {
    double ma = a.modulus(), mb = b.modulus();
    if (fabs(ma - mb) < 1e-9) return a.real < b.real;
    return ma < mb;
}

void bubbleSort(vector<Complex>& v, bool (*cmp)(const Complex&, const Complex&)) {
    size_t n = v.size();
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j + 1 < n - i; ++j)
            if (!cmp(v[j], v[j+1])) swap(v[j], v[j+1]);
}

void mergeSortImpl(vector<Complex>& v, vector<Complex>& tmp, int l, int r, bool (*cmp)(const Complex&, const Complex&)) {
    if (r - l <= 1) return;
    int m = (l + r) / 2;
    mergeSortImpl(v, tmp, l, m, cmp);
    mergeSortImpl(v, tmp, m, r, cmp);
    int i = l, j = m, k = l;
    while (i < m || j < r) {
        if (j >= r || (i < m && cmp(v[i], v[j]))) tmp[k++] = v[i++];
        else tmp[k++] = v[j++];
    }
    for (int t = l; t < r; ++t) v[t] = tmp[t];
}

void mergeSort(vector<Complex>& v, bool (*cmp)(const Complex&, const Complex&)) {
    vector<Complex> tmp(v.size());
    mergeSortImpl(v, tmp, 0, (int)v.size(), cmp);
}

vector<Complex> generateRandomComplexVector(int n, int vmax=10) {
    vector<Complex> v;
    random_device rd; mt19937 gen(rd());
    uniform_real_distribution<> dis(0, vmax);
    for (int i = 0; i < n; ++i) {
        double r = round(dis(gen) * 100.0) / 100.0;
        double im = round(dis(gen) * 100.0) / 100.0;
        v.emplace_back(r, im);
    }
    return v;
}

vector<Complex> rangeQueryByModulus(const vector<Complex>& sortedVec, double m1, double m2) {
    vector<Complex> res;
    for (const auto& c : sortedVec) {
        double m = c.modulus();
        if (m >= m1 && m < m2) res.push_back(c);
        if (m >= m2) break;
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto vec = generateRandomComplexVector(20, 20);
    cout << "Original vector (20 complex):\n";
    for (auto &c : vec) cout << "("<<c.real<<","<<c.imag<<") ";
    cout << "\n\n";

    auto v_shuf = vec;
    shuffle(v_shuf.begin(), v_shuf.end(), mt19937(chrono::steady_clock::now().time_since_epoch().count()));
    cout << "Shuffled:\n";
    for (auto &c : v_shuf) cout << "("<<c.real<<","<<c.imag<<") ";
    cout << "\n\n";

    Complex target = v_shuf[3];
    auto it = find_if(v_shuf.begin(), v_shuf.end(), [&](const Complex& x){ return x == target; });
    cout << "Searching for ("<<target.real<<","<<target.imag<<") => " << (it!=v_shuf.end() ? "Found\n" : "Not Found\n");

    v_shuf.push_back(Complex(5.5, 5.5));
    cout << "After inserting (5.5,5.5), size = " << v_shuf.size() << "\n";
    v_shuf.erase(remove_if(v_shuf.begin(), v_shuf.end(), [](const Complex& x){ return fabs(x.real-5.5)<1e-9 && fabs(x.imag-5.5)<1e-9; }), v_shuf.end());
    cout << "After removing (5.5,5.5), size = " << v_shuf.size() << "\n";

    sort(v_shuf.begin(), v_shuf.end(), [](const Complex& a, const Complex& b){
        if (a.real != b.real) return a.real < b.real;
        return a.imag < b.imag;
    });
    v_shuf.erase(unique(v_shuf.begin(), v_shuf.end(), [](const Complex& a, const Complex& b){ return a==b; }), v_shuf.end());
    cout << "After unique, size = " << v_shuf.size() << "\n\n";

    vector<Complex> seq = v_shuf;
    sort(seq.begin(), seq.end(), cmpByModulus); 
    
    vector<Complex> asc = seq;
    vector<Complex> desc = seq; reverse(desc.begin(), desc.end());
    vector<Complex> rnd = seq; shuffle(rnd.begin(), rnd.end(), mt19937(12345));

    auto time_bubble = [&](vector<Complex> a){
        auto t0 = chrono::high_resolution_clock::now();
        bubbleSort(a, cmpByModulus);
        auto t1 = chrono::high_resolution_clock::now();
        return chrono::duration<double, milli>(t1 - t0).count();
    };
    auto time_merge = [&](vector<Complex> a){
        auto t0 = chrono::high_resolution_clock::now();
        mergeSort(a, cmpByModulus);
        auto t1 = chrono::high_resolution_clock::now();
        return chrono::duration<double, milli>(t1 - t0).count();
    };

    cout << "Timing (ms):\n";
    cout << "Bubble sort on asc: " << time_bubble(asc) << " ms\n";
    cout << "Bubble sort on rnd: " << time_bubble(rnd) << " ms\n";
    cout << "Bubble sort on desc: " << time_bubble(desc) << " ms\n";
    cout << "Merge sort on asc: " << time_merge(asc) << " ms\n";
    cout << "Merge sort on rnd: " << time_merge(rnd) << " ms\n";
    cout << "Merge sort on desc: " << time_merge(desc) << " ms\n\n";

    sort(seq.begin(), seq.end(), cmpByModulus);
    double m1 = 5.0, m2 = 12.0;
    auto res = rangeQueryByModulus(seq, m1, m2);
    cout << "Range query by modulus in ["<<m1<<","<<m2<<") found " << res.size() << " items:\n";
    for (auto &c: res) cout << "("<<c.real<<","<<c.imag<<") mod="<<c.modulus()<<" ";
    cout << "\n\n";

    return 0;
}
