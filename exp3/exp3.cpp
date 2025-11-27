#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
using namespace std;

// ========================= 图的基础数据结构 =========================
class Graph {
public:
    int n;                  // 节点数
    vector<char> nodes;     // 节点名称（A~H 或 A~L）
    vector<vector<int>> adjMatrix; // 邻接矩阵（-1表示无边，正数为权值）

    // 构造函数：初始化节点和邻接矩阵
    Graph(int nodeCount, const vector<char>& nodeNames) {
        n = nodeCount;
        nodes = nodeNames;
        adjMatrix.resize(n, vector<int>(n, -1)); // 初始化为无边
        for (int i = 0; i < n; ++i) adjMatrix[i][i] = 0; // 自身到自身权值为0
    }

    // 添加无向边（u, v为节点索引，weight为权值）
    void addEdge(int u, int v, int weight) {
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight;
    }

    // 输出邻接矩阵
    void printAdjMatrix() {
        cout << "邻接矩阵（-1表示无边，0表示自身，正数为权值）：" << endl;
        // 输出表头（节点名称）
        cout << "   ";
        for (char c : nodes) cout << c << "  ";
        cout << endl;
        // 输出每行数据
        for (int i = 0; i < n; ++i) {
            cout << nodes[i] << "  ";
            for (int j = 0; j < n; ++j) {
                if (adjMatrix[i][j] == -1) cout << "-1 ";
                else cout << adjMatrix[i][j] << "  ";
            }
            cout << endl;
        }
    }

    // 辅助函数：根据节点名称找索引
    int findNodeIndex(char c) {
        auto it = find(nodes.begin(), nodes.end(), c);
        return it != nodes.end() ? it - nodes.begin() : -1;
    }
};

// ========================= 任务2：图1的BFS和DFS =========================
// BFS遍历（从startNode出发）
void BFS(Graph& g, char startNode) {
    int start = g.findNodeIndex(startNode);
    if (start == -1) { cout << "起点不存在！" << endl; return; }

    vector<bool> visited(g.n, false);
    queue<int> q;
    visited[start] = true;
    q.push(start);

    cout << "BFS遍历结果（从" << startNode << "出发）：";
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        cout << g.nodes[u] << " ";

        // 遍历所有邻接节点
        for (int v = 0; v < g.n; ++v) {
            if (g.adjMatrix[u][v] > 0 && !visited[v]) { // 有边且未访问
                visited[v] = true;
                q.push(v);
            }
        }
    }
    cout << endl;
}

// DFS遍历（递归版，从startNode出发）
void DFS_recursive(Graph& g, int u, vector<bool>& visited) {
    visited[u] = true;
    cout << g.nodes[u] << " ";

    for (int v = 0; v < g.n; ++v) {
        if (g.adjMatrix[u][v] > 0 && !visited[v]) {
            DFS_recursive(g, v, visited);
        }
    }
}

void DFS(Graph& g, char startNode) {
    int start = g.findNodeIndex(startNode);
    if (start == -1) { cout << "起点不存在！" << endl; return; }

    vector<bool> visited(g.n, false);
    cout << "DFS遍历结果（从" << startNode << "出发）：";
    DFS_recursive(g, start, visited);
    cout << endl;
}

// ========================= 任务3：图1的最短路径（Dijkstra）和最小支撑树（Prim） =========================
// Dijkstra最短路径算法（从startNode出发到所有节点）
void Dijkstra(Graph& g, char startNode) {
    int start = g.findNodeIndex(startNode);
    if (start == -1) { cout << "起点不存在！" << endl; return; }

    int n = g.n;
    vector<int> dist(n, INT_MAX); // 最短距离数组
    vector<bool> visited(n, false); // 是否确定最短路径
    dist[start] = 0;

    // 优先队列（小顶堆）：(当前距离, 节点索引)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;

        // 松弛操作
        for (int v = 0; v < n; ++v) {
            if (g.adjMatrix[u][v] > 0 && !visited[v] && dist[u] != INT_MAX) {
                if (dist[v] > dist[u] + g.adjMatrix[u][v]) {
                    dist[v] = dist[u] + g.adjMatrix[u][v];
                    pq.push({dist[v], v});
                }
            }
        }
    }

    // 输出结果
    cout << "Dijkstra最短路径（从" << startNode << "出发）：" << endl;
    for (int i = 0; i < n; ++i) {
        cout << startNode << "→" << g.nodes[i] << ": ";
        if (dist[i] == INT_MAX) cout << "不可达";
        else cout << dist[i];
        cout << endl;
    }
}

// Prim最小支撑树算法（从startNode出发，无向带权图）
void Prim(Graph& g, char startNode) {
    int start = g.findNodeIndex(startNode);
    if (start == -1) { cout << "起点不存在！" << endl; return; }

    int n = g.n;
    vector<int> key(n, INT_MAX); // 记录每个节点到生成树的最小权值
    vector<int> parent(n, -1);   // 记录生成树中节点的父节点
    vector<bool> inMST(n, false); // 是否已加入MST

    key[start] = 0;
    parent[start] = -1;

    // 构建MST（需要n-1条边）
    for (int i = 0; i < n - 1; ++i) {
        // 找key最小且未加入MST的节点
        int u = -1;
        for (int v = 0; v < n; ++v) {
            if (!inMST[v] && (u == -1 || key[v] < key[u])) {
                u = v;
            }
        }

        inMST[u] = true;

        // 更新邻接节点的key值
        for (int v = 0; v < n; ++v) {
            if (g.adjMatrix[u][v] > 0 && !inMST[v] && g.adjMatrix[u][v] < key[v]) {
                key[v] = g.adjMatrix[u][v];
                parent[v] = u;
            }
        }
    }

    // 输出MST
    cout << "Prim最小支撑树（从" << startNode << "出发）：" << endl;
    int totalWeight = 0;
    for (int i = 0; i < n; ++i) {
        if (parent[i] != -1) {
            cout << g.nodes[parent[i]] << "-" << g.nodes[i] << "（权值：" << g.adjMatrix[parent[i]][i] << "）" << endl;
            totalWeight += g.adjMatrix[parent[i]][i];
        }
    }
    cout << "MST总权值：" << totalWeight << endl;
}

// ========================= 任务4：图2的双连通分量和关节点（修复后Tarjan算法） =========================
class BiconnectedComponent {
public:
    vector<vector<int>>& adj; // 邻接表（图2的邻接表）
    vector<char>& nodes;      // 节点名称
    int n;                    // 节点数
    vector<int> disc;         // 发现时间
    vector<int> low;          // 能到达的最早发现节点
    vector<int> parent;       // 父节点
    vector<bool> isArticulation; // 是否为关节点
    stack<pair<int, int>> edgeStack; // 存储边
    int time;                 // 时间戳

    BiconnectedComponent(vector<vector<int>>& adjList, vector<char>& nodeNames) 
        : adj(adjList), nodes(nodeNames) {
        n = adj.size();
        disc.resize(n, -1);
        low.resize(n, -1);
        parent.resize(n, -1);
        isArticulation.resize(n, false);
        time = 0;
        // 清空栈（防止残留）
        while (!edgeStack.empty()) edgeStack.pop();
    }

    // Tarjan算法找双连通分量和关节点
    void tarjan(int u) {
        int children = 0;
        disc[u] = low[u] = ++time;

        for (int v : adj[u]) {
            if (disc[v] == -1) { // 未访问过
                children++;
                parent[v] = u;
                edgeStack.push({u, v});
                tarjan(v);

                // 更新low[u]
                low[u] = min(low[u], low[v]);

                // 情况1：根节点且子节点数>=2（关节点）
                if (parent[u] == -1 && children > 1) {
                    isArticulation[u] = true;
                    printBCC(); // 输出当前双连通分量
                }

                // 情况2：非根节点，low[v] >= disc[u]（关节点）
                if (parent[u] != -1 && low[v] >= disc[u]) {
                    isArticulation[u] = true;
                    printBCC(); // 输出当前双连通分量
                }
            }
            // 已访问过且不是父节点（回边，更新low[u]）
            else if (v != parent[u] && disc[v] < disc[u]) {
                low[u] = min(low[u], disc[v]);
                edgeStack.push({u, v});
            }
        }
    }

    // 修复后的双连通分量输出（避免空栈访问）
    void printBCC() {
        static int bccCount = 0;
        cout << "双连通分量" << ++bccCount << "：";
        vector<pair<int, int>> currentBCC;

        // 弹出当前BCC的所有边（栈非空才操作）
        while (!edgeStack.empty()) {
            auto edge = edgeStack.top();
            edgeStack.pop();
            currentBCC.push_back(edge);

            // 终止条件：当前边是触发BCC的关键边（避免多弹边）
            if ((parent[edge.second] == edge.first && isArticulation[edge.first]) ||
                (parent[edge.first] == edge.second && isArticulation[edge.second])) {
                break;
            }
        }

        // 输出当前BCC的边（去重，避免重复输出同一无向边）
        for (auto& e : currentBCC) {
            cout << nodes[e.first] << "-" << nodes[e.second] << " ";
        }
        cout << endl;
    }

    // 执行算法并输出结果（处理非连通图）
    void findBCCAndArticulation() {
        // 重置状态（避免多次调用时残留数据）
        fill(disc.begin(), disc.end(), -1);
        fill(low.begin(), low.end(), -1);
        fill(parent.begin(), parent.end(), -1);
        fill(isArticulation.begin(), isArticulation.end(), false);
        time = 0;
        while (!edgeStack.empty()) edgeStack.pop();

        // 处理所有连通分量
        for (int i = 0; i < n; ++i) {
            if (disc[i] == -1) {
                tarjan(i);
                // 处理当前连通分量的剩余边（最后一个BCC）
                if (!edgeStack.empty()) {
                    printBCC();
                }
            }
        }

        // 输出关节点
        cout << "关节点：";
        bool hasArticulation = false;
        for (int i = 0; i < n; ++i) {
            if (isArticulation[i]) {
                cout << nodes[i] << " ";
                hasArticulation = true;
            }
        }
        if (!hasArticulation) cout << "无";
        cout << endl;
    }
};

// 构建图2的邻接表（节点A~L，索引0~11）
vector<vector<int>> buildGraph2AdjList() {
    int n = 12;
    vector<vector<int>> adj(n);
    // 边：A-B, E-F, E-I, F-C, F-G, F-K, C-D, C-H, G-K, J-K, K-L
    adj[0].push_back(1); adj[1].push_back(0); // A-B
    adj[4].push_back(5); adj[5].push_back(4); // E-F
    adj[4].push_back(8); adj[8].push_back(4); // E-I
    adj[5].push_back(2); adj[2].push_back(5); // F-C
    adj[5].push_back(6); adj[6].push_back(5); // F-G
    adj[5].push_back(10); adj[10].push_back(5); // F-K
    adj[2].push_back(3); adj[3].push_back(2); // C-D
    adj[2].push_back(7); adj[7].push_back(2); // C-H
    adj[6].push_back(10); adj[10].push_back(6); // G-K
    adj[9].push_back(10); adj[10].push_back(9); // J-K
    adj[10].push_back(11); adj[11].push_back(10); // K-L
    return adj;
}

// ========================= 主函数（测试所有任务） =========================
int main() {
    // 解决中文输出乱码（Windows控制台）
    system("chcp 65001");

    // -------------------------- 任务1+2+3：处理图1 --------------------------
    cout << "===================== 图1 相关操作 =====================" << endl;
    // 图1节点：A,B,C,D,E,F,G,H（索引0~7）
    vector<char> graph1Nodes = {'A','B','C','D','E','F','G','H'};
    Graph graph1(8, graph1Nodes);
    // 添加图1的边（权值与题目一致）
    graph1.addEdge(0,1,4);  // A-B
    graph1.addEdge(0,3,6);  // A-D
    graph1.addEdge(0,6,7);  // A-G
    graph1.addEdge(1,2,12); // B-C
    graph1.addEdge(1,3,9);  // B-D
    graph1.addEdge(1,4,1);  // B-E
    graph1.addEdge(2,5,2);  // C-F
    graph1.addEdge(2,7,10); // C-H
    graph1.addEdge(3,4,13); // D-E
    graph1.addEdge(3,6,2);  // D-G
    graph1.addEdge(4,5,5);  // E-F
    graph1.addEdge(4,6,11); // E-G
    graph1.addEdge(4,7,8);  // E-H
    graph1.addEdge(5,7,3);  // F-H
    graph1.addEdge(6,7,14); // G-H

    // 任务1：输出图1邻接矩阵
    graph1.printAdjMatrix();
    cout << endl;

    // 任务2：BFS和DFS（从A出发）
    BFS(graph1, 'A');
    DFS(graph1, 'A');
    cout << endl;

    // 任务3：最短路径（Dijkstra）和最小支撑树（Prim）
    Dijkstra(graph1, 'A');
    cout << endl;
    Prim(graph1, 'A');
    cout << endl;

    // -------------------------- 任务4：处理图2 --------------------------
    cout << "===================== 图2 双连通分量和关节点 =====================" << endl;
    // 图2节点：A,B,C,D,E,F,G,H,I,J,K,L（索引0~11）
    vector<char> graph2Nodes = {'A','B','C','D','E','F','G','H','I','J','K','L'};
    vector<vector<int>> graph2Adj = buildGraph2AdjList();

    // 测试不同起点（验证关节点结果一致）
    vector<char> testStarts = {'A', 'E', 'K', 'J'};
    for (char start : testStarts) {
        cout << "=== 以" << start << "为起点 ===" << endl;
        BiconnectedComponent bcc(graph2Adj, graph2Nodes);
        bcc.findBCCAndArticulation();
        cout << endl;
    }

    return 0;
}