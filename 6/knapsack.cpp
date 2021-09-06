/*
 *      Knapsack solver's functions
 */

#include <vector>
#include <algorithm>
#include <cassert>
#include <thread>
#include <unistd.h>

#include "knapsack.h"


#ifndef MAX_RECURSION_DEPTH
#define MAX_RECURSION_DEPTH 10000
#endif

#define ERROR_ALLOC_FAILED -1

#define DIM2LIN(x, y, N) ((x)*(N) + (y))


using namespace std;


void sample_timecomplex_task()
{
    // usleep(1000);
}


value_t knapsack_exhaustive(vector<Item> &items, weight_t max_w)
{
    return knapsack_brute(items, max_w, 0, 0, false);
}


value_t knapsack_branch_bound(vector<Item> &items, weight_t max_w)
{
    return knapsack_brute(items, max_w, 0, 0, true);
}


value_t knapsack_brute(vector<Item> &items, weight_t max_w, value_t curr_value, int offset, bool branch_bound)
{
    if (offset == items.size())
        return curr_value;

    sample_timecomplex_task();
    assert(offset < MAX_RECURSION_DEPTH);
    
    value_t if_incl;
    value_t if_not_incl;
    
    if_not_incl = knapsack_brute(items, max_w, curr_value, offset + 1, branch_bound);
    
    if (!branch_bound || items[offset].weight <= max_w)
    {
        if_incl = knapsack_brute(items, max_w - items[offset].weight,
                                 curr_value + items[offset].value, offset + 1, branch_bound);
        if (!branch_bound && items[offset].weight > max_w)
            return if_not_incl;
        return max(if_incl, if_not_incl);
    }
    return if_not_incl;
}


void knapsack_step_str(vector<Item> &items, value_t* str1, value_t* str2, int i, int j)
{
    sample_timecomplex_task();
    if (items[i].weight > j)
        str2[j] = str1[j];
    else
        str2[j] = max(str1[j], str1[j-items[i].weight] + items[i].value);
}


void knapsack_step_arr(vector<Item> &items, value_t* m, int i, int init_index, int N, int block_size)
{
    size_t n = min(init_index + block_size, N);
    for (int j = init_index; j < n; j++)
    {
        sample_timecomplex_task();
        if (items[i-1].weight > j)
            m[DIM2LIN(i,j,N)] = m[DIM2LIN(i-1,j,N)];
        else
            m[DIM2LIN(i,j,N)] = max(m[DIM2LIN(i-1,j,N)], m[DIM2LIN(i-1, j-items[i-1].weight, N)] + items[i-1].value);
    }
}


value_t knapsack_dynamic_single(vector<Item> &items, weight_t max_w)
{
    int n = items.size();

    auto* str1 = (value_t*) calloc(max_w+1, sizeof(value_t));
    auto* str2 = (value_t*) calloc(max_w+1, sizeof(value_t));
    if (!str1 || !str2)
        return -1;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= max_w; j++)
            knapsack_step_str(items, str1, str2, i, j);
        swap(str1, str2);
    }

    value_t ans = max(str2[max_w], str1[max_w]);
    free(str1);
    free(str2);
    return ans;
}


value_t knapsack_dynamic_multi(vector<Item> &items, weight_t max_w, size_t threads_num)
{
    int n = items.size();
    auto* m = (value_t*) calloc((n+1)*(max_w+1), sizeof(value_t));
    if (!m)
        return ERROR_ALLOC_FAILED;

    size_t block_size = max_w / threads_num + 2;
    vector<thread> threads;
    for (int wave = 1; wave <= n; wave++)
    {
        for (int block = 1; block <= max_w; block += block_size)
            threads.emplace_back(thread(knapsack_step_arr, ref(items), m, wave, block, max_w+1, block_size));
        for (auto &t: threads)
            t.join();
        threads.clear();
    }

    value_t ans = m[(n+1)*(max_w+1)-1];
    free(m);
    return ans;
}


value_t knapsack_step_rec(vector<Item> &items, value_t* m, int i, int j, int N, int offset)
{
    assert(offset < MAX_RECURSION_DEPTH);
    sample_timecomplex_task();

    if (j <= 0 || i == 0)
        return 0;

    if (m[DIM2LIN(i, j, N)] != -1)
        return m[DIM2LIN(i, j, N)];

    if (m[DIM2LIN(i-1, j, N)] == -1)
        m[DIM2LIN(i-1, j, N)] = knapsack_step_rec(items, m, i-1, j, N, offset+1);

    weight_t w = items[i-1].weight;
    if (w > j)
        m[DIM2LIN(i, j, N)] = m[DIM2LIN(i-1, j, N)];
    else {
        if (m[DIM2LIN(i-1, j-w, N)] == -1) {
            m[DIM2LIN(i-1, j-w, N)] = knapsack_step_rec(items, m, i-1, j-w, N, offset+1);
        }
        m[DIM2LIN(i, j, N)] = max(m[DIM2LIN(i-1, j, N)], m[DIM2LIN(i-1, j-w, N)] + items[i-1].value);
    }
    return m[DIM2LIN(i, j, N)];
}


value_t knapsack_dynamic_recursive(vector<Item> &items, weight_t max_w)
{
    int n = items.size();

    auto* m = (value_t*) calloc((n+1)*(max_w+1), sizeof(value_t));
    if (!m)
        return -1;

    for (int i = 0; i < (n+1)*(max_w+1); i++)
        m[i] = -1;

    value_t ans = knapsack_step_rec(items, m, n, max_w, max_w+1, 0);

    free(m);
    return ans;
}
