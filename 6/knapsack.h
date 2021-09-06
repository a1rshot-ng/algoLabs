/*
 *      Knapsack solver's header
 */

#pragma once

#include <iostream>
#include <cstdlib>
#include <vector>
#include <thread>


using namespace std;

typedef int weight_t;
typedef int value_t;

typedef struct Item
{
    weight_t weight;
    value_t value;
} Item;


//    O(2^n) time,  O(n) memory
value_t knapsack_exhaustive(vector<Item> &items, weight_t max_w);

//    O(2^n) time,  O(n) memory
value_t knapsack_branch_bound(vector<Item> &items, weight_t max_w);

//    O(nW) time,  O(W) memory
value_t knapsack_dynamic_single(vector<Item> &items, weight_t max_w);

//    O(nW) time,  O(nW) memory
value_t knapsack_dynamic_multi(vector<Item> &items, weight_t max_w, size_t threads_num);

//    O(nW) time,  O(nW) memory
value_t knapsack_dynamic_recursive(vector<Item> &items, weight_t max_w);

//    generalization for exhaustive and branch & bound
value_t knapsack_brute(vector<Item> &items, weight_t max_w, int curr_value, int offset, bool branch_bound);

//    internal 'step' simulators
value_t knapsack_step_rec(vector<Item> &items, value_t* m, int i, int j, int N, int offset);
void knapsack_step_str(vector<Item> &items, value_t* str1, value_t* str2, int i, int j);
void knapsack_step_arr(vector<Item> &items, value_t* m, int i, int j, int N, int block_size);
