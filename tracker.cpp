#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <ctime>
using namespace std;

// ---------- Linked List for Appliances ----------
struct Appliance {
    string name;
    float unitsPerHour;
    float hoursPerDay;
    Appliance* next;
};

Appliance* createApplianceList() {
    Appliance* head = nullptr;

    auto add = [&](const string& name, float uph, float hrs) {
        Appliance* node = new Appliance{name, uph, hrs, head};
        head = node;
    };

    add("Iron", 1.0, 0.5);
    add("Washing Machine", 0.5, 1);
    add("AC", 1.5, 5);
    add("TV", 0.1, 4);
    add("Refrigerator", 0.15, 24);
    add("Light", 0.01, 6);
    add("Fan", 0.05, 8);

    return head;
}

float calculateApplianceUsage(Appliance* head, ofstream& out) {
    float total = 0;
    out << "Appliance,Units/Day,Cost/Day\n";
    while (head) {
        float units = head->unitsPerHour * head->hoursPerDay;
        float cost = units <= 1 ? units * 5 : (units <= 3 ? units * 7 : units * 10);
        out << head->name << "," << units << ",Rs" << cost << "\n";
        total += units;
        head = head->next;
    }
    return total;
}

// ---------- Sliding Window Max 7-day Usage ----------
int slidingWindowMax(const vector<int>& arr, int k) {
    int maxSum = 0, sum = 0;
    for (int i = 0; i < k; ++i) sum += arr[i];
    maxSum = sum;
    for (int i = k; i < arr.size(); ++i) {
        sum += arr[i] - arr[i - k];
        maxSum = max(maxSum, sum);
    }
    return maxSum;
}

// ---------- Monthly Summary (Sorted + Median) ----------
void summarizeAndSortUsage(vector<int> usage, ofstream& out) {
    // Selection Sort
    for (int i = 0; i < usage.size(); ++i) {
        int minIdx = i;
        for (int j = i + 1; j < usage.size(); ++j) {
            if (usage[j] < usage[minIdx]) minIdx = j;
        }
        swap(usage[i], usage[minIdx]);
    }

    int highest = usage.back();
    int lowest = usage.front();
    float median;
    int n = usage.size();
    if (n % 2 == 0)
        median = (usage[n/2 - 1] + usage[n/2]) / 2.0;
    else
        median = usage[n/2];

    out << "\n===== Usage Summary (Sorted) =====\n";
    out << "Lowest Usage: " << lowest << " units\n";
    out << "Highest Usage: " << highest << " units\n";
    out << "Median Usage: " << median << " units\n";
}

// ---------- Compare Two Users ----------
void compareUsers(const vector<int>& u1, const vector<int>& u2, ofstream& out) {
    int u1Better = 0, u2Better = 0;
    int u1Total = 0, u2Total = 0;

    out << "\n===== User Comparison =====\n";
    out << "Day,User1,User2,Better\n";
    for (int i = 0; i < u1.size() && i < u2.size(); ++i) {
        string better;
        if (u1[i] < u2[i]) {
            better = "User 1";
            u1Better++;
        } else if (u2[i] < u1[i]) {
            better = "User 2";
            u2Better++;
        } else {
            better = "Same";
        }
        u1Total += u1[i];
        u2Total += u2[i];
        out << (i + 1) << "," << u1[i] << "," << u2[i] << "," << better << "\n";
    }

    out << "User 1 used less on " << u1Better << " days\n";
    out << "User 2 used less on " << u2Better << " days\n";
    out << "Total Units - User 1: " << u1Total << ", User 2: " << u2Total << "\n";
}

// ---------- Bill History Linked List ----------
struct BillNode {
    string date;
    int units;
    float total;
    BillNode* next;
};

void insertBill(BillNode*& head, const string& date, int units, float total) {
    BillNode* node = new BillNode{date, units, total, nullptr};
    if (!head) head = node;
    else {
        BillNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = node;
    }
}

void displayBillHistory(BillNode* head, ofstream& out) {
    out << "\n===== Bill History =====\n";
    out << "Date,Units,Bill\n";
    while (head) {
        out << head->date << "," << head->units << ",Rs" << head->total << "\n";
        head = head->next;
    }
}

string getTodayDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d-%b-%Y", ltm);
    return string(buffer);
}

int main() {
    ifstream in("data/usage.txt");
    ofstream out("data/bill_data.csv");

    if (!in || !out) {
        cerr << "❌ Cannot open file. Make sure usage.txt and data folder exist.\n";
        return 1;
    }

    // Read usage array
    string line;
    getline(in, line);
    stringstream ss(line);
    vector<int> usage;
    string temp;
    while (getline(ss, temp, ',')) {
        usage.push_back(stoi(temp));
    }

    // Total Bill
    float totalBill = 0;
    out << "===== Daily Usage and Bill =====\n";
    out << "Day,Units,Bill\n";
    for (size_t i = 0; i < usage.size(); ++i) {
        int u = usage[i];
        float cost = u <= 100 ? u * 5 : (u <= 200 ? u * 7 : u * 10);
        totalBill += cost;
        out << (i + 1) << "," << u << ",Rs" << cost << "\n";
    }

    out << "\nTotal Units: " << accumulate(usage.begin(), usage.end(), 0) << "\n";
    out << "Total Bill: Rs" << totalBill << "\n";

    if (usage.size() >= 7) {
        int maxWeek = slidingWindowMax(usage, 7);
        out << "Max 7-day Usage: " << maxWeek << " units\n";
    }

    // Appliance-based breakdown
    out << "\n===== Appliance-wise Usage =====\n";
    Appliance* appliances = createApplianceList();
    float totalApplianceUnits = calculateApplianceUsage(appliances, out);
    out << "Total Estimated Appliance Usage/Day: " << totalApplianceUnits << " units\n";

    // Monthly Summary
    summarizeAndSortUsage(usage, out);

    // Compare Two Users
    vector<int> user2 = usage;
    transform(user2.begin(), user2.end(), user2.begin(), [](int u) { return u + rand() % 11 - 5; });
    compareUsers(usage, user2, out);

    // Bill History
    BillNode* history = nullptr;
    insertBill(history, getTodayDate(), accumulate(usage.begin(), usage.end(), 0), totalBill);
    displayBillHistory(history, out);

    in.close();
    out.close();

    cout << "✅ Bill saved to 'data/bill_data.csv'. Open in Excel or browser for details.\n";
    return 0;
}



