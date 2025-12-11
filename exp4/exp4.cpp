#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

// 边界框结构体：包含位置、大小、置信度、索引（用于NMS后映射原始数据）
struct BoundingBox {
    float x1;     // 左上角x
    float y1;     // 左上角y
    float x2;     // 右下角x
    float y2;     // 右下角y
    float score;  // 置信度
    int index;    // 原始索引（避免排序后丢失位置）

    BoundingBox(float x1_ = 0, float y1_ = 0, float x2_ = 0, float y2_ = 0, float s_ = 0, int idx_ = 0)
        : x1(x1_), y1(y1_), x2(x2_), y2(y2_), score(s_), index(idx_) {}
};

// 1. 排序算法实现（四种）
// 1.1 快速排序（按置信度降序）
int partition(vector<BoundingBox>& arr, int low, int high) {
    float pivot = arr[high].score;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].score >= pivot) { // 降序排列
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<BoundingBox>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// 1.2 归并排序（按置信度降序）
void merge(vector<BoundingBox>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<BoundingBox> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].score >= R[j].score) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<BoundingBox>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// 1.3 堆排序（按置信度降序）
void heapify(vector<BoundingBox>& arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && arr[l].score > arr[largest].score) largest = l;
    if (r < n && arr[r].score > arr[largest].score) largest = r;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    // 构建大顶堆
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);
    // 堆排序（逐个提取堆顶元素）
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

// 1.4 冒泡排序（按置信度降序）
void bubbleSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].score < arr[j + 1].score) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// 2. 数据生成模块（两种分布）
// 2.1 随机分布：边界框位置、大小、置信度均随机（合理范围）
vector<BoundingBox> generateRandomBoxes(int count) {
    vector<BoundingBox> boxes;
    srand(time(0)); // 随机种子
    for (int i = 0; i < count; i++) {
        float x1 = rand() % 800; // 图像宽度假设为800
        float y1 = rand() % 600; // 图像高度假设为600
        float w = 20 + rand() % 100; // 宽度20~120
        float h = 20 + rand() % 100; // 高度20~120
        float x2 = x1 + w;
        float y2 = y1 + h;
        float score = (rand() % 1000) / 1000.0f; // 置信度0~1
        boxes.emplace_back(x1, y1, x2, y2, score, i);
    }
    return boxes;
}

// 2.2 聚集分布：边界框集中在图像中心区域，少量分散
vector<BoundingBox> generateClusteredBoxes(int count) {
    vector<BoundingBox> boxes;
    srand(time(0));
    float center_x = 400; // 图像中心x
    float center_y = 300; // 图像中心y
    for (int i = 0; i < count; i++) {
        // 80%概率在中心200x200区域，20%概率随机分布
        float prob = (rand() % 100) / 100.0f;
        float x1, y1;
        if (prob < 0.8) {
            x1 = center_x - 100 + rand() % 200; // 300~500
            y1 = center_y - 100 + rand() % 200; // 200~400
        } else {
            x1 = rand() % 800;
            y1 = rand() % 600;
        }
        float w = 20 + rand() % 100;
        float h = 20 + rand() % 100;
        float x2 = x1 + w;
        float y2 = y1 + h;
        float score = (rand() % 1000) / 1000.0f;
        boxes.emplace_back(x1, y1, x2, y2, score, i);
    }
    return boxes;
}

// 3. 基础NMS算法（依赖排序后的边界框）
// 计算两个边界框的交并比（IoU）
float calculateIoU(const BoundingBox& a, const BoundingBox& b) {
    float inter_x1 = max(a.x1, b.x1);
    float inter_y1 = max(a.y1, b.y1);
    float inter_x2 = min(a.x2, b.x2);
    float inter_y2 = min(a.y2, b.y2);

    if (inter_x1 >= inter_x2 || inter_y1 >= inter_y2) return 0.0f;

    float inter_area = (inter_x2 - inter_x1) * (inter_y2 - inter_y1);
    float a_area = (a.x2 - a.x1) * (a.y2 - a.y1);
    float b_area = (b.x2 - b.x1) * (b.y2 - b.y1);
    return inter_area / (a_area + b_area - inter_area);
}

// NMS核心逻辑：输入排序后的边界框，输出去重后的结果
vector<BoundingBox> nms(vector<BoundingBox> sorted_boxes, float iou_threshold = 0.5f) {
    vector<BoundingBox> result;
    while (!sorted_boxes.empty()) {
        // 取置信度最高的框
        BoundingBox top = sorted_boxes[0];
        result.push_back(top);
        // 移除与top框IoU超过阈值的框
        vector<BoundingBox> temp;
        for (size_t i = 1; i < sorted_boxes.size(); i++) {
            if (calculateIoU(top, sorted_boxes[i]) < iou_threshold) {
                temp.push_back(sorted_boxes[i]);
            }
        }
        sorted_boxes = temp;
    }
    return result;
}

// 4. 性能测试模块（计算排序+NMS的总运行时间）
typedef void (*SortFunc)(vector<BoundingBox>&); // 排序函数指针
typedef vector<BoundingBox> (*DataGenFunc)(int); // 数据生成函数指针

// 测试单个排序算法的性能
void testSortPerformance(SortFunc sort_func, const string& sort_name,
                         DataGenFunc data_gen_func, const string& data_dist, int data_size) {
    // 生成测试数据
    vector<BoundingBox> boxes = data_gen_func(data_size);
    vector<BoundingBox> boxes_copy = boxes; // 避免数据修改影响多次测试

    // 计时：排序 + NMS
    clock_t start = clock();
    
    // 排序（核心测试步骤）
    if (sort_name == "quickSort") quickSort(boxes_copy, 0, boxes_copy.size() - 1);
    else if (sort_name == "mergeSort") mergeSort(boxes_copy, 0, boxes_copy.size() - 1);
    else if (sort_name == "heapSort") heapSort(boxes_copy);
    else if (sort_name == "bubbleSort") bubbleSort(boxes_copy);
    
    // NMS（固定步骤，确保测试公平）
    vector<BoundingBox> nms_result = nms(boxes_copy);
    
    clock_t end = clock();
    double time_cost = double(end - start) / CLOCKS_PER_SEC * 1000; // 转换为毫秒

    // 输出结果
    cout << left << setw(12) << sort_name
         << setw(12) << data_dist
         << setw(8) << data_size
         << setw(12) << time_cost << "ms"
         << setw(10) << nms_result.size() << endl;
}

int main() {
    // 测试配置：数据规模（100~10000）、分布类型
    vector<int> data_sizes = {100, 1000, 5000, 10000};
    vector<pair<DataGenFunc, string>> data_gens = {
        {generateRandomBoxes, "Random"},
        {generateClusteredBoxes, "Clustered"}
    };
    vector<pair<SortFunc, string>> sort_funcs = {
        {nullptr, "quickSort"},   // 特殊处理递归函数
        {nullptr, "mergeSort"},
        {heapSort, "heapSort"},
        {bubbleSort, "bubbleSort"}
    };

    // 输出表头
    cout << left << setw(12) << "排序算法"
         << setw(12) << "数据分布"
         << setw(8) << "数据规模"
         << setw(12) << "运行时间"
         << setw(10) << "NMS后数量" << endl;
    cout << string(60, '-') << endl;

    // 执行所有测试用例
    for (auto& data_gen : data_gens) {
        for (int size : data_sizes) {
            for (auto& sort_func : sort_funcs) {
                testSortPerformance(sort_func.first, sort_func.second,
                                   data_gen.first, data_gen.second, size);
            }
            cout << endl; // 不同数据规模之间换行
        }
    }

    return 0;
}