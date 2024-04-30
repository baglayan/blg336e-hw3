/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 3
 * Meriç Bağlayan
 * 150190056
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

/* YOU CAN INCLUDE ANY OTHER NECESSARY LIBRARIES */


/* YOU CAN DEFINE EXTRA FUNCTIONS AND MODIFY THE CODE TEMPLATE */

/* YOU HAVE TO CONSIDER THE OUTPUT FORMAT TO TEST YOUR CODE WITH CALICO */


using namespace std;


struct TimeIntervals {

    // YOUR CODE HERE

};

struct Priority {

    // YOUR CODE HERE
    
};

struct Item {

    // YOUR CODE HERE
    
};


/* START: WEIGHTED INTERVAL SCHEDULING (for each floor) */

struct Schedule
{

    // YOUR CODE HERE  

};

// function to find the best schedule for each floor
vector<Schedule> weighted_interval_scheduling(vector<Schedule> schedules)
{
    // YOUR CODE HERE

    return optimal_schedules;
}

// *** END ***


/* START: KNAPSACK (for items) */

// Function to select the most valuable items that can be purchased with a certain budget
vector<Item> knapsack(const vector<Item>& Items, int budget) {

    // YOUR CODE HERE

    return selected_items;
}

// *** END ***


int main(int argc, char *argv[])
{
    string case_no = argv[1];
    string case_name = "case_" + case_no;
    string path = "./inputs/" + case_name;

    int total_budget = 200000; 

    // YOUR CODE HERE



    return 0;
}