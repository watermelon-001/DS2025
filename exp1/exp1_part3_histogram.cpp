// exp1_part3_histogram.cpp
#include <bits/stdc++.h>
using namespace std;

int largestRectangleArea(vector<int>& heights) {
    stack<int> s;
    int n = heights.size();
    int maxA = 0;
    for (int i = 0; i <= n; ++i) {
        int h = (i==n ? 0 : heights[i]);
        while (!s.empty() && h < heights[s.top()]) {
            int height = heights[s.top()]; s.pop();
            int left = s.empty() ? -1 : s.top();
            int width = i - left - 1;
            maxA = max(maxA, height * width);
        }
        s.push(i);
    }
    return maxA;
}

int main() {
    vector<vector<int>> tests = {
        {2,1,5,6,2,3},
        {2,4},
        {0},
        {2,2,2,2},
        {6,2,5,4,5,1,6}
    };
    for (auto &t : tests) {
        auto copy = t;
        cout << "heights = [";
        for (int i=0;i<(int)t.size();++i){ if (i) cout << ","; cout << t[i]; }
        cout << "] -> max area = " << largestRectangleArea(copy) << "\n";
    }
    // random 10 group tests
    cout << "\nRandom 10 tests:\n";
    random_device rd; mt19937 gen(rd()); uniform_int_distribution<> dis(0, 100);
    for (int k=0;k<10;++k) {
        int len = 1 + (gen() % 100);
        vector<int> a(len);
        for (int i=0;i<len;++i) a[i] = dis(gen);
        cout << "test " << k+1 << " len=" << len << " -> " << largestRectangleArea(a) << "\n";
    }
    return 0;
}
