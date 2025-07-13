#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>  // for accumulate
#include <iomanip>  // for formatting
using namespace std;

// Bill Node for Linked List (Optional Monthly History)
struct BillNode {
    int day;
    int units;
    float bill;
    BillNode* next;
    BillNode(int d, int u, float b) : day(d), units(u), bill(b), next(nullptr) {}
};

float calculateDailyBill(int units) {
    if (units <= 100) return units * 5;
    else if (units <= 200) return units * 7;
    else return units * 10;
}

int main() {
    ifstream in("data/usage.txt");
    if (!in.is_open()) {
        cerr << "❌ ERROR: Cannot open usage.txt\n";
        return -1;
    }

    vector<int> usage;
    int u;
    while (in >> u) {
        usage.push_back(u);
        if (in.peek() == ',') in.ignore();
    }
    in.close();

    // Linked list to hold per-day bill data
    BillNode* head = nullptr;
    BillNode* tail = nullptr;

    float total = 0;
    for (int i = 0; i < usage.size(); ++i) {
        float daily = calculateDailyBill(usage[i]);
        total += daily;
        BillNode* newNode = new BillNode(i + 1, usage[i], daily);
        if (!head) head = newNode;
        else tail->next = newNode;
        tail = newNode;
    }

    
    ofstream csv("data/bill_data.csv");
    csv << "Day,Units,Bill\n";

    BillNode* current = head;
    while (current) {
        csv << current->day << "," << current->units << "," << fixed << setprecision(2) << current->bill << "\n";
        current = current->next;
    }
    csv << "Total,," << fixed << setprecision(2) << total << "\n";
    csv.close();

    // Clean up linked list
    current = head;
    while (current) {
        BillNode* temp = current;
        current = current->next;
        delete temp;
    }

    cout << "✅ Bill saved to 'data/bill_data.csv'. Open in Excel or Google Sheets for charts!\n";

    return 0;
}



