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

struct IntervalPair {
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

// Custom hash function for IntervalPair
struct IntervalPairHash {
    size_t operator()(const IntervalPair &ip) const
    {
        // Calculate hash by XORing the hashes of start_time and end_time
        return hash<string>()(ip.start_time) ^ hash<string>()(ip.end_time);
    }
};

// Struct to store item data
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

// Struct to store room priorities
struct RoomPriority {
    string floor_name;
    string room_no;
    int priority;
};

// Struct to store room time intervals
struct RoomIntervals {
    string floor_name;
    string room_no;
    vector<IntervalPair> intervals;
};

/* START: WEIGHTED INTERVAL SCHEDULING (for each floor) */

// Struct to store a schedule for a room
struct Schedule {
    string floor_name;
    string room_no;
    vector<IntervalPair> intervals;

    // This could've been implemented better.

    // Currently, it is used to store the total priority gain for the floor
    // despite the struct existing for each room.
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
    // Iterate over the intervals in reverse order
    for (int i = n - 1; i >= 0; i--) {
        // If the current interval does not conflict
        // with the given interval, return its index
        if (intervals[i].first.end_time <= intervals[n].first.start_time) {
            return i;
        }
    }
    // If no non-conflicting interval is found, return -1
    return -1;
}

vector<Schedule> weighted_interval_scheduling(vector<Schedule> schedules)
{
    // Map to store intervals for each floor
    unordered_map<string, vector<pair<IntervalPair, int>>> floor_intervals;

    // Map to store room number for each interval
    unordered_map<string, unordered_map<IntervalPair, string, IntervalPairHash>> interval_to_room;

    // Fill the maps with the given schedules
    for (const Schedule &schedule : schedules) {
        for (const auto &interval : schedule.intervals) {
            floor_intervals[schedule.floor_name].emplace_back(interval, schedule.total_priority);
            interval_to_room[schedule.floor_name][interval] = schedule.room_no;
        }
    }

    // Vector to store optimal schedules.
    // This will be the return value
    vector<Schedule> optimal_schedules;

    // Iterate over the intervals for each floor
    for (auto &pair : floor_intervals) {
        // Name of the current floor
        const string &floor_name = pair.first;

        // Intervals for the current floor
        auto &intervals_with_priority = pair.second;

        // If there are no intervals for the current floor, skip
        int n = intervals_with_priority.size();
        if (n == 0)
            continue;

        // Sort the intervals based on their end times
        sort(intervals_with_priority.begin(), intervals_with_priority.end(),
            [](const ::pair<IntervalPair, int> &a, const ::pair<IntervalPair, int> &b) {
                return a.first.end_time < b.first.end_time;
            });

        // Vector to store the maximum priority gain for each interval
        vector<int> dp(n);
        dp[0] = intervals_with_priority[0].second;

        // Calculate the maximum priority gain for each interval
        for (int i = 1; i < n; i++) {
            // Priority gain of the current interval
            int incl_prof = intervals_with_priority[i].second;

            // Latest non-conflicting interval
            int l = find_latest_non_conflict(intervals_with_priority, i);

            // If a non-conflicting interval is found,
            if (l != -1) {
                // Add the priority gain of the latest non-conflicting interval
                incl_prof += dp[l];
            }

            // Store the maximum priority gain for the current interval.-
            dp[i] = max(incl_prof, dp[i - 1]);
        }

        // Calculate the total priority gain for the floor
        int total_priority = dp[n - 1];

        // Vector to store the best intervals
        vector<IntervalPair> best_intervals;

        // Vector to store the room numbers for the best intervals
        vector<string> best_room_no;

        // Iterate over the intervals in reverse order
        for (int i = n - 1; i >= 0;) {
            // If the current interval has a different priority gain,
            if (i == 0 || dp[i] != dp[i - 1]) {
                // Add the interval and its room number to the best vectors
                best_intervals.push_back(intervals_with_priority[i].first);
                best_room_no.push_back(interval_to_room[floor_name][intervals_with_priority[i].first]);

                // Change i to the latest non-conflicting interval
                i = find_latest_non_conflict(intervals_with_priority, i);
            } else {
                // If the current interval has the same priority gain as the previous one,
                i--;
            }
        }

        // Reverse the vectors to get the intervals in order
        reverse(best_intervals.begin(), best_intervals.end());
        reverse(best_room_no.begin(), best_room_no.end());

        // Map to store the intervals for each room
        unordered_map<string, vector<IntervalPair>> room_to_intervals;

        // Fill the map with the best intervals
        for (size_t i = 0; i < best_intervals.size(); ++i) {
            room_to_intervals[best_room_no[i]].push_back(best_intervals[i]);
        }

        // Fill up the optimal schedules vector
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
    // Number of items
    int n = items.size();

    // Create a (n+1) by (budget+1) 2D DP table
    // dp[i][w] represents the maximum value achievable with the first i items and a budget w
    vector<vector<double>> dp(n + 1, vector<double>(budget + 1, 0));

    for (int i = 1; i <= n; i++) {
        // Price of the current item
        int price = items[i - 1].price;

        // Value of the current item
        double value = items[i - 1].value;

        // Loop through each possible budget
        for (int w = 0; w <= budget; w++) {
            // If the item can fit in the budget,
            if (price <= w) {
                // dp[i][w] is now the max. of not taking the item and taking the item
                dp[i][w] = max(dp[i - 1][w], dp[i - 1][w - price] + value);
            } else { // If the item can't fit in the current budget w
                // Go on without including the current item
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    // Vector to store the selected items
    vector<Item> selected_items;

    // Backtrack to find the selected items
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
    // Vector to store read items
    vector<Item> items;

    // Initialize items file stream
    ifstream items_file(filepath);

    // Handle file not found
    if (!items_file) {
        cerr << "Items file not found!\n";
        return items;
    }

    string line;

    // Skip header (per given input format)
    getline(items_file, line);

    // Read items from the file
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
    // Initialize vectors to store room priorities and intervals
    vector<RoomPriority> room_priorities;
    vector<RoomIntervals> room_intervals;
    vector<Schedule> schedules;

    // Initialize priorities file stream
    ifstream priorities_file(priorities_filepath);

    // Handle file not found
    if (!priorities_file) {
        cerr << "Priorities file not found!\n";
        return schedules;
    }
    string line;

    // Skip header (per given input format)
    getline(priorities_file, line);

    // Read room priorities from the file
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

    // Initialize intervals file stream
    ifstream intervals_file(room_time_intervals_filepath);

    // Handle file not found
    if (!intervals_file) {
        cerr << "Room time intervals file not found!\n";
        return schedules;
    }

    // Skip header (per given input format)
    getline(intervals_file, line);

    // Read room time intervals from the file
    while (getline(intervals_file, line)) {
        istringstream iss(line);
        string floor_name, room_no, start, end;

        if (iss >> floor_name >> room_no >> start >> end) {
            // See if currently read room has any stored intervals
            auto it = find_if(room_intervals.begin(), room_intervals.end(),
                [&](const RoomIntervals &ri) {
                    return ri.floor_name == floor_name && ri.room_no == room_no;
                });

            // If intervals are found, add the new interval to the existing intervals
            if (it != room_intervals.end()) {
                it->intervals.emplace_back(start, end);
            } else {
                // Otherwise, create a new RoomIntervals object and add it to the room_intervals vector
                room_intervals.push_back({ floor_name, room_no, { { start, end } } });
            }
        } else {
            cerr << "Error reading line: " << line << '\n';
        }
    }

    // Fill schedules vector with the all the intervals for each room
    for (const auto &rp : room_priorities) {
        // Find the intervals for the current room
        auto it = find_if(room_intervals.begin(), room_intervals.end(),
            [&](const RoomIntervals &ri) {
                return ri.floor_name == rp.floor_name && ri.room_no == rp.room_no;
            });

        vector<IntervalPair> intervals;

        // If intervals are found, assign them to the intervals vector
        if (it != room_intervals.end()) {
            intervals = it->intervals;
        }

        // Add the room schedule to the schedules vector
        schedules.emplace_back(rp.floor_name, rp.room_no, intervals, rp.priority);
    }

    return schedules;
}

int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <case_no>\n";
        return 1;
    }

    // To construct input file paths
    string case_no = argv[1];
    string case_name = "case_" + case_no;
    string path = "./inputs/" + case_name;

    // Total budget for the items
    int total_budget = 200000;

    // Read data from files
    vector<Item> items = read_item_data(path + "/items.txt");
    vector<Schedule> schedules = read_room_data(path + "/priority.txt", path + "/room_time_intervals.txt");

    // Find an optimal schedule for each floor
    vector<Schedule> optimal_schedules = weighted_interval_scheduling(schedules);

    // Find the most valuable items that can be purchased with the total budget
    vector<Item> purchased_items = knapsack(items, total_budget);

    /* Following operations are for fitting the data to expected output format */

    // Create a mapping from floors to their full schedules
    unordered_map<string, vector<Schedule>> floor_to_schedules;
    for (const auto &schedule : optimal_schedules) {
        floor_to_schedules[schedule.floor_name].push_back(schedule);
    }

    // Create a set of unique floors based on their names
    unordered_set<string> floors_set;
    for (const auto &schedule : schedules) {
        floors_set.insert(schedule.floor_name);
    }

    // Sort floor list based on floor names
    vector<string> floor_list(floors_set.begin(), floors_set.end());
    sort(floor_list.begin(), floor_list.end());

    cout << "Best Schedule for Each Floor\n";

    // Print the optimal schedule for each floor
    for (const auto &floor_name : floor_list) {
        // Skip floors that do not have any schedules
        if (floor_to_schedules.find(floor_name) == floor_to_schedules.end()) {
            continue;
        }

        // Get the schedules for the current floor
        const vector<Schedule> &schedules = floor_to_schedules[floor_name];

        // Find the total priority gain for the current floor
        auto it = find_if(schedules.begin(), schedules.end(),
            [&floor_name](const Schedule &schedule) {
                return schedule.floor_name == floor_name;
            });

        double total_priority_gain = (it != schedules.end()) ? it->total_priority : 0.0;

        cout << floor_name << " --> Priority Gain: " << total_priority_gain << endl;

        // Sort the intervals based on their start times
        vector<pair<string, IntervalPair>> floor_intervals;
        for (const auto &schedule : schedules) {
            for (const auto &interval : schedule.intervals) {
                floor_intervals.emplace_back(schedule.room_no, interval);
            }
        }

        sort(floor_intervals.begin(), floor_intervals.end(), [](const pair<string, IntervalPair> &a, const pair<string, IntervalPair> &b) {
            return a.second.start_time < b.second.start_time;
        });

        // Print the intervals for the current floor
        for (const auto &entry : floor_intervals) {
            const string &room_no = entry.first;
            const IntervalPair &interval = entry.second;
            cout << floor_name << "\t" << room_no << "\t" << interval.start_time << "\t" << interval.end_time << endl;
        }

        // Print an empty line between floors
        cout << endl;
    }

    // Find the total value of the purchased items
    double total_value = 0;
    for (const auto &item : purchased_items) {
        total_value += item.value;
    }

    // Round the total value to one decimal place
    total_value = floor(total_value * 10 + 0.5f) / 10;

    // Print the best use of the budget
    cout << "Best Use of Budget\n";
    cout << "Total Value --> " << fixed << setprecision(1) << total_value << endl;
    for (const auto &item : purchased_items) {
        cout << item.name << "\n";
    }

    return 0;
}
