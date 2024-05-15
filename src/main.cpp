/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 3
 * Meriç Bağlayan
 * 150190056
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

/* YOU CAN INCLUDE ANY OTHER NECESSARY LIBRARIES */

/* YOU CAN DEFINE EXTRA FUNCTIONS AND MODIFY THE CODE TEMPLATE */

/* YOU HAVE TO CONSIDER THE OUTPUT FORMAT TO TEST YOUR CODE WITH CALICO */

using namespace std;

class IntervalPair {
public:
    string start_time;
    string end_time;

    IntervalPair(string s, string e)
        : start_time(s)
        , end_time(e)
    {
    }

    bool operator==(const IntervalPair &other) const
    {
        return start_time == other.start_time && end_time == other.end_time;
    }
};

struct IntervalPairHash {
    size_t operator()(const IntervalPair &ip) const
    {
        return hash<string>()(ip.start_time) ^ hash<string>()(ip.end_time);
    }
};

struct Item {
    string name;
    int price;
    double value;

    Item(string n, int p, double v)
        : name(n)
        , price(p)
        , value(v)
    {
    }
};

struct RoomPriority {
    string floor_name;
    string room_no;
    int priority;
};

struct RoomIntervals {
    string floor_name;
    string room_no;
    vector<IntervalPair> intervals;
};

/* START: WEIGHTED INTERVAL SCHEDULING (for each floor) */

struct Schedule {
    string floor_name;
    string room_no;
    vector<IntervalPair> intervals;
    int total_priority;

    Schedule(const string &fn, const string &rn, const vector<IntervalPair> &iv, int tp)
        : floor_name(fn)
        , room_no(rn)
        , intervals(iv)
        , total_priority(tp)
    {
    }
};

int find_latest_non_conflict(const vector<pair<IntervalPair, int>> &intervals, int n)
{
    for (int i = n - 1; i >= 0; i--) {
        if (intervals[i].first.end_time <= intervals[n].first.start_time) {
            return i;
        }
    }
    return -1;
}

vector<Schedule> weighted_interval_scheduling(vector<Schedule> schedules)
{
    unordered_map<string, vector<pair<IntervalPair, int>>> floor_intervals;
    unordered_map<string, unordered_map<IntervalPair, string, IntervalPairHash>> interval_to_room;

    for (const Schedule &schedule : schedules) {
        for (const auto &interval : schedule.intervals) {
            floor_intervals[schedule.floor_name].emplace_back(interval, schedule.total_priority);
            interval_to_room[schedule.floor_name][interval] = schedule.room_no;
        }
    }

    vector<Schedule> optimal_schedules;

    for (auto &pair : floor_intervals) {
        const string &floor_name = pair.first;
        auto &intervals_with_priority = pair.second;

        int n = intervals_with_priority.size();
        if (n == 0)
            continue;

        sort(intervals_with_priority.begin(), intervals_with_priority.end(),
            [](const ::pair<IntervalPair, int> &a, const ::pair<IntervalPair, int> &b) {
                return a.first.end_time < b.first.end_time;
            });

        vector<int> dp(n);
        dp[0] = intervals_with_priority[0].second;

        for (int i = 1; i < n; i++) {
            int incl_prof = intervals_with_priority[i].second;
            int l = find_latest_non_conflict(intervals_with_priority, i);
            if (l != -1) {
                incl_prof += dp[l];
            }
            dp[i] = max(incl_prof, dp[i - 1]);
        }

        int total_priority = dp[n - 1];
        vector<IntervalPair> best_intervals;
        vector<string> best_room_no;

        for (int i = n - 1; i >= 0;) {
            if (i == 0 || dp[i] != dp[i - 1]) {
                best_intervals.push_back(intervals_with_priority[i].first);
                best_room_no.push_back(interval_to_room[floor_name][intervals_with_priority[i].first]);
                i = find_latest_non_conflict(intervals_with_priority, i);
            } else {
                i--;
            }
        }

        reverse(best_intervals.begin(), best_intervals.end());
        reverse(best_room_no.begin(), best_room_no.end());

        unordered_map<string, vector<IntervalPair>> room_to_intervals;
        for (size_t i = 0; i < best_intervals.size(); ++i) {
            room_to_intervals[best_room_no[i]].push_back(best_intervals[i]);
        }

        for (const auto &entry : room_to_intervals) {
            const string &room_no = entry.first;
            const vector<IntervalPair> &intervals = entry.second;
            optimal_schedules.push_back({ floor_name, room_no, intervals, total_priority });
        }
    }

    return optimal_schedules;
}

// *** END ***

/* START: KNAPSACK (for items) */

// Function to select the most valuable items that can be purchased with a certain budget
vector<Item> knapsack(const vector<Item> &items, int budget)
{
    int n = items.size();
    vector<vector<double>> dp(n + 1, vector<double>(budget + 1, 0));

    for (int i = 1; i <= n; i++) {
        int price = items[i - 1].price;
        double value = items[i - 1].value;
        for (int w = 0; w <= budget; w++) {
            if (price <= w) {
                dp[i][w] = max(dp[i - 1][w], dp[i - 1][w - price] + value);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    vector<Item> selected_items;
    for (int i = n, w = budget; i > 0 && w > 0; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            selected_items.push_back(items[i - 1]);
            w -= items[i - 1].price;
        }
    }

    return selected_items;
}

// *** END ***

vector<Item> read_item_data(string filepath)
{
    vector<Item> items;

    ifstream items_file(filepath);

    if (!items_file) {
        cerr << "Items file not found!\n";
        return items;
    }

    string line;

    getline(items_file, line);

    while (getline(items_file, line)) {
        istringstream iss(line);
        string name;
        int price;
        double value;

        if (iss >> name >> price >> value) {
            items.emplace_back(name, price, value);
        } else {
            cerr << "Error reading line: " << line << '\n';
        }
    }

    return items;
}

vector<Schedule> read_room_data(string priorities_filepath, string room_time_intervals_filepath)
{
    vector<RoomPriority> room_priorities;
    vector<RoomIntervals> room_intervals;
    vector<Schedule> schedules;

    ifstream priorities_file(priorities_filepath);

    if (!priorities_file) {
        cerr << "Priorities file not found!\n";
        return schedules;
    }

    string line;

    getline(priorities_file, line);

    while (getline(priorities_file, line)) {
        istringstream iss(line);
        string floor_name, room_no;
        int priority;

        if (iss >> floor_name >> room_no >> priority) {
            room_priorities.push_back({ floor_name, room_no, priority });
        } else {
            cerr << "Error reading line: " << line << '\n';
        }
    }

    ifstream intervals_file(room_time_intervals_filepath);

    if (!intervals_file) {
        cerr << "Room time intervals file not found!\n";
        return schedules;
    }

    getline(intervals_file, line);

    while (getline(intervals_file, line)) {
        istringstream iss(line);
        string floor_name, room_no, start, end;

        if (iss >> floor_name >> room_no >> start >> end) {
            auto it = find_if(room_intervals.begin(), room_intervals.end(),
                [&](const RoomIntervals &ri) {
                    return ri.floor_name == floor_name && ri.room_no == room_no;
                });

            if (it != room_intervals.end()) {
                it->intervals.emplace_back(start, end);
            } else {
                room_intervals.push_back({ floor_name, room_no, { { start, end } } });
            }
        } else {
            cerr << "Error reading line: " << line << '\n';
        }
    }

    for (const auto &rp : room_priorities) {
        auto it = find_if(room_intervals.begin(), room_intervals.end(),
            [&](const RoomIntervals &ri) {
                return ri.floor_name == rp.floor_name && ri.room_no == rp.room_no;
            });

        vector<IntervalPair> intervals;
        if (it != room_intervals.end()) {
            intervals = it->intervals;
        }

        schedules.emplace_back(rp.floor_name, rp.room_no, intervals, rp.priority);
    }

    return schedules;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <case_no>\n";
        return 1;
    }

    string case_no = argv[1];
    string case_name = "case_" + case_no;
    string path = "./inputs/" + case_name;

    int total_budget = 200000;

    vector<Item> items = read_item_data(path + "/items.txt");
    vector<Schedule> schedules = read_room_data(path + "/priority.txt", path + "/room_time_intervals.txt");

    vector<Schedule> optimal_schedules = weighted_interval_scheduling(schedules);
    vector<Item> purchased_items = knapsack(items, total_budget);

    unordered_map<string, vector<Schedule>> floor_to_schedules;
    for (const auto &schedule : optimal_schedules) {
        floor_to_schedules[schedule.floor_name].push_back(schedule);
    }

    unordered_set<string> floors_set;
    for (const auto &schedule : schedules) {
        floors_set.insert(schedule.floor_name);
    }

    vector<string> floor_list(floors_set.begin(), floors_set.end());
    sort(floor_list.begin(), floor_list.end());

    cout << "Best Schedule for Each Floor\n";

    for (const auto &floor_name : floor_list) {
        if (floor_to_schedules.find(floor_name) == floor_to_schedules.end()) {
            continue;
        }
        const vector<Schedule> &schedules = floor_to_schedules[floor_name];

        auto it = find_if(schedules.begin(), schedules.end(),
            [&floor_name](const Schedule &schedule) {
                return schedule.floor_name == floor_name;
            });

        double total_priority_gain = (it != schedules.end()) ? it->total_priority : 0.0;

        cout << floor_name << " --> Priority Gain: " << total_priority_gain << endl;

        vector<pair<string, IntervalPair>> floor_intervals;
        for (const auto &schedule : schedules) {
            for (const auto &interval : schedule.intervals) {
                floor_intervals.emplace_back(schedule.room_no, interval);
            }
        }

        sort(floor_intervals.begin(), floor_intervals.end(), [](const pair<string, IntervalPair> &a, const pair<string, IntervalPair> &b) {
            return a.second.start_time < b.second.start_time;
        });

        for (const auto &entry : floor_intervals) {
            const string &room_no = entry.first;
            const IntervalPair &interval = entry.second;
            cout << floor_name << "\t" << room_no << "\t" << interval.start_time << "\t" << interval.end_time << endl;
        }

        cout << endl;
    }

    double total_value = 0;
    for (const auto &item : purchased_items) {
        total_value += item.value;
    }

    total_value = floor(total_value * 10 + 0.5f) / 10;

    cout << "Best Use of Budget\n";
    cout << "Total Value --> " << fixed << setprecision(1) << total_value << endl;
    for (const auto &item : purchased_items) {
        cout << item.name << "\n";
    }

    return 0;
}
