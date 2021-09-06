/*
 *          Lab 6  -  Algorithms
 *
 *      Algorithms for the knapsack problem:
 *          exhaustive         -  O(2^n) time,  O(n) memory
 *          branch & bound     -  O(2^n) time,  O(n) memory
 *          dynamic:
 *              recursive      -  O(nW) time,   O(nW) memory
 *              single-thread  -  O(nW) time,   O(W) memory
 *              multi-thread   -  O(nW/T) time, O(nW) memory
 *
 *     All of them are time-measured while runtime.
 *     There's an option to generate random input:  ./lab6 -r
 */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>

#include "knapsack.h"


#ifndef MAX_ITEMS_FOR_EXHAUSTIVE
#define MAX_ITEMS_FOR_EXHAUSTIVE 12
#endif

#ifndef THREADS_NUM
#define THREADS_NUM thread::hardware_concurrency() * 2 - 1
#endif

using namespace std;


template <typename Function>
void measure_time(Function func)
{
    auto start = chrono::high_resolution_clock::now();
    value_t ans = func();
    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << ans << "\t(" << duration.count() << " us)" << endl;
}


int main(int argc, char* argv[])
{
    srand(time(nullptr));

    vector<Item> items;
    Item item;
    value_t value;
    weight_t weight;
    weight_t max_weight;

    char do_exhaustive = 'y';
    bool mode_random = (argc >= 2 && !strcmp("-r", argv[1]));

    cout << "Knapsack solver CLI\tLab 6 - Algorithms   " << ends;
    if (mode_random) cout << "[random input]";
    cout << endl;

    do {
        cout << "Max capacity:  " << ends;
        cin >> max_weight;
    } while (max_weight <= 0);

    if (!mode_random) {
        string line;
        cout << endl << "Items list (Double-newline to end)" << endl;
        cin.ignore();

        while (true) {
            cout << "value & weight:  " << ends;

            if (!getline(cin, line)) return EXIT_SUCCESS;
            if (line.empty()) break;

            istringstream ss(line);
            if (!(ss >> value >> weight) || weight < 0) continue;

            item.value = value;
            item.weight = weight;
            items.push_back(item);
        }
    } else {
        size_t items_count;
        cout << "Items count:  " << ends;
        cin >> items_count;

        for (int i = 0; i < items_count; i++)
        {
            item.weight = (unsigned) random() % (1 + ((unsigned) max_weight >> 3u)) + 1;
            item.value = (unsigned) random() % (1u << 16u) + 1;

            items.push_back(item);
        }
    }

    cout << endl;

    if (items.size() > MAX_ITEMS_FOR_EXHAUSTIVE) {
        cout << "Do exhaustive search for " << items.size() << " items? (y/n) " << ends;
        cin >> do_exhaustive;
    }

    cout << "Computing..." << endl;

    if (do_exhaustive == 'y')
    {
        cout << "exhaustive:           " << ends;
        measure_time([&](){ return knapsack_exhaustive(items, max_weight); });

        cout << "branch & bound:       " << ends;
        measure_time([&](){ return knapsack_branch_bound(items, max_weight); });
    }

    cout << "dynamic (single):     " << ends;
    measure_time([&](){ return knapsack_dynamic_single(items, max_weight); });

    cout << "dynamic (recursive):  " << ends;
    measure_time([&](){ return knapsack_dynamic_recursive(items, max_weight); });

    cout << "dynamic (multi):      " << ends;
    measure_time([&](){ return knapsack_dynamic_multi(items, max_weight, THREADS_NUM); });

    return EXIT_SUCCESS;
}
