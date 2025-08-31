#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <chrono>
#include <immintrin.h>
#include <cstdlib>
#include <cmath>    // for std::fabs


using namespace std;
using namespace std::chrono;

int embedding_table_size = 100000;
const int embedding_dim = 128;
int input_size = 720;
const int num_bags = 20;
int prefetch_distance = 8;
#ifndef LOCALITY_HINT
#define LOCALITY_HINT 0
#endif

int random_int(int range)
{
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(0, range - 1);
    return dis(gen);
}

// emb_tabl 128 * 10 ** 6. random floats. its a array of floats till range 10 ** 8
// input 720. random ints. its a array of ints till range 10 ** 8
// offsets 20. Array of multiples of 36.
long long run_with_prefetching(const vector<float> &embedding_table, const vector<int> &input, const vector<int> &offsets)
{
    
    auto start = high_resolution_clock::now();
    //----------------------------------------------------- Write your code here ----------------------------------------------------------------
    #ifdef PREFETCH
    vector<vector<float>> output;

    for (size_t i = 0; i < offsets.size(); ++i)
    {
        const int start_idx = offsets[i];
        const int end_idx = (i + 1 < offsets.size()) ? offsets[i + 1] : input.size();
        int j;

        // WARNING: It is assumed that end_idx > prefetch_distance
        const int end_idx_prefetch = end_idx - prefetch_distance;

        vector<float> bag_embedding(embedding_dim, 0.0f);

        for (j = start_idx; j < end_idx_prefetch; j++)
        {
            const int future_input_index = input[j + prefetch_distance];
            __builtin_prefetch(&(embedding_table[future_input_index * embedding_dim]), 0, LOCALITY_HINT);
            // _mm_prefetch(&(embedding_table[future_input_index* embedding_dim]), _MM_HINT_T0);

            const float *data_ptr = &embedding_table[input[j] * embedding_dim];
            for (int d = 0; d < embedding_dim; ++d)
            {
                bag_embedding[d] += data_ptr[d];
            }
        }

        // leftover iterations
        for (int j = end_idx_prefetch; j < end_idx; j++)
        {
            const float *data_ptr = &embedding_table[input[j] * embedding_dim];

            for (int d = 0; d < embedding_dim; ++d)
            {
                bag_embedding[d] += data_ptr[d];
            }
        }

        output.push_back(bag_embedding);
    }
    #endif
    //-------------------------------------------------------------------------------------------------------------------------------------------

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "\nTime WITH software prefetching: " << duration.count() << " microseconds.";

    return duration.count();
    
}

long long run_with_simd(const vector<float> &embedding_table, const vector<int> &input, const vector<int> &offsets)
{
    auto start = high_resolution_clock::now();

    //----------------------------------------------------- Write your code here ----------------------------------------------------------------
    vector<vector<float>> output;
    int d;

    #ifdef SIMD_512
    
    __m512 A_r, B_r;

    for (size_t i = 0; i < offsets.size(); ++i)
    {
        int start_idx = offsets[i];
        int end_idx = (i + 1 < offsets.size()) ? offsets[i + 1] : input.size();

        vector<float> bag_embedding(embedding_dim, 0.0f);

        for (int j = start_idx; j < end_idx; ++j)
        {
            const float *data_ptr = &embedding_table[input[j] * embedding_dim];

            for (d = 0; d < embedding_dim; d += 16)
            {
                A_r = _mm512_loadu_ps(&bag_embedding[d]);
                B_r = _mm512_loadu_ps(&data_ptr[d]);

                _mm512_storeu_ps(&bag_embedding[d], _mm512_add_ps(A_r, B_r));
            }

            // leftover iterations
            for (; d < embedding_dim; d++)
            {
                bag_embedding[d] += data_ptr[d];
            }
        }

        output.push_back(bag_embedding);
    }

    #endif

    #ifdef SIMD_256

    __m256 A_r, B_r;

    for (size_t i = 0; i < offsets.size(); ++i)
    {
        int start_idx = offsets[i];
        int end_idx = (i + 1 < offsets.size()) ? offsets[i + 1] : input.size();

        vector<float> bag_embedding(embedding_dim, 0.0f);

        for (int j = start_idx; j < end_idx; ++j)
        {
            const float *data_ptr = &embedding_table[input[j] * embedding_dim];

            for (d = 0; d < embedding_dim; d += 8)
            {
                A_r = _mm256_loadu_ps(&bag_embedding[d]);
                B_r = _mm256_loadu_ps(&data_ptr[d]);
                _mm256_storeu_ps(&bag_embedding[d], _mm256_add_ps(A_r, B_r));
            }

            // leftover iterations
            for (; d < embedding_dim; d++)
            {
                bag_embedding[d] += data_ptr[d];
            }
        }

        output.push_back(bag_embedding);
    }

    #endif

    #ifdef SIMD_128
    __m128 A_r, B_r;

    for (size_t i = 0; i < offsets.size(); ++i)
    {
        int start_idx = offsets[i];
        int end_idx = (i + 1 < offsets.size()) ? offsets[i + 1] : input.size();

        vector<float> bag_embedding(embedding_dim, 0.0f);

        for (int j = start_idx; j < end_idx; ++j)
        {
            const float *data_ptr = &embedding_table[input[j] * embedding_dim];

            for (d = 0; d < embedding_dim; d += 4)
            {
                A_r = _mm_loadu_ps(&bag_embedding[d]);
                B_r = _mm_loadu_ps(&data_ptr[d]);
                _mm_storeu_ps(&bag_embedding[d], _mm_add_ps(A_r, B_r));
            }

            // leftover iterations
            for (; d < embedding_dim; d++)
            {
                bag_embedding[d] += data_ptr[d];
            }
        }

        output.push_back(bag_embedding);
    }

    #endif

    //-------------------------------------------------------------------------------------------------------------------------------------------

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "\nTime WITH SIMD: " << duration.count() << " microseconds.";

    return duration.count();
}

long long run_with_prefetching_simd(const vector<float> &embedding_table, const vector<int> &input, const vector<int> &offsets)
{

    auto start = high_resolution_clock::now();

    //----------------------------------------------------- Write your code here ----------------------------------------------------------------
    #ifdef prefetch_simd_512
    vector<vector<float>> output;
    int d;
    __m512 A_r, B_r;

    for (size_t i = 0; i < offsets.size(); ++i)
    {
        const int start_idx = offsets[i];
        const int end_idx = (i + 1 < offsets.size()) ? offsets[i + 1] : input.size();
        int j;

        // WARNING: It is assumed that end_idx > prefetch_distance
        const int end_idx_prefetch = end_idx - prefetch_distance;

        vector<float> bag_embedding(embedding_dim, 0.0f);

        for (j = start_idx; j < end_idx_prefetch; j++)
        {
            const int future_input_index = input[j + prefetch_distance];
            __builtin_prefetch(&(embedding_table[future_input_index * embedding_dim]), 0, LOCALITY_HINT);
            // _mm_prefetch(&(embedding_table[future_input_index* embedding_dim]), _MM_HINT_T0);

            const float *data_ptr = &embedding_table[input[j] * embedding_dim];
         
            for (d = 0; d < embedding_dim; d += 16)
            {
                A_r = _mm512_loadu_ps(&bag_embedding[d]);
                B_r = _mm512_loadu_ps(&data_ptr[d]);

                _mm512_storeu_ps(&bag_embedding[d], _mm512_add_ps(A_r, B_r));
            }

            // leftover iterations
            for (; d < embedding_dim; d++)
            {
                bag_embedding[d] += data_ptr[d];
            }

        }

        // leftover iterations
        for (int j = end_idx_prefetch; j < end_idx; j++)
        {
            const float *data_ptr = &embedding_table[input[j] * embedding_dim];

             for (d = 0; d < embedding_dim; d += 16)
            {
                A_r = _mm512_loadu_ps(&bag_embedding[d]);
                B_r = _mm512_loadu_ps(&data_ptr[d]);

                _mm512_storeu_ps(&bag_embedding[d], _mm512_add_ps(A_r, B_r));
            }

            // leftover iterations
            for (; d < embedding_dim; d++)
            {
                bag_embedding[d] += data_ptr[d];
            }
        }

        output.push_back(bag_embedding);
    }
    #endif

    //-------------------------------------------------------------------------------------------------------------------------------------------

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "\nTime WITH software prefetching and SIMD: " << duration.count() << " microseconds.";

    return duration.count();
}

// emb_tabl 128 * 10 ** 6. random floats. its a array of floats till range 10 ** 8
// input 720. random ints. its a array of ints till range 10 ** 8
// offsets 20. Array of multiples of 36.
long long naive_emb(vector<float> &embedding_table, const vector<int> &input, const vector<int> &offsets)
{
    
    auto start = high_resolution_clock::now();
    #ifdef NAIVE
    vector<vector<float>> output;
    
    for (size_t i = 0; i < offsets.size(); ++i)
    {
        int start_idx = offsets[i];
        int end_idx = (i + 1 < offsets.size()) ? offsets[i + 1] : input.size();

        vector<float> bag_embedding(embedding_dim, 0.0f);

        for (int j = start_idx; j < end_idx; ++j)
        {
            float *data_ptr = &embedding_table[input[j] * embedding_dim];
            for (int d = 0; d < embedding_dim; ++d)
            {
                bag_embedding[d] += data_ptr[d];
            }
        }

        output.push_back(bag_embedding);
    }
    #endif
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "\nTime WITHOUT software prefetching: " << duration.count() << " microseconds.";

    return duration.count();
    
}

int main()
{
    /*modified part*/
    cin>>prefetch_distance>>embedding_table_size>>input_size;
    /*modified part*/
    // Prepare embedding table
    vector<float> embedding_table(embedding_table_size * embedding_dim);
    for (auto &val : embedding_table)
    {
        val = static_cast<float>(random_int(embedding_table_size));
    }

    // Input indices
    vector<int> input(input_size);
    for (auto &idx : input)
    {
        idx = random_int(embedding_table_size);
    }

    // Offsets
    vector<int> offsets;
    for (int i = 0; i < num_bags; ++i)
    {
        offsets.push_back((input_size * i) / num_bags);
    }
    #ifdef NAIVE
        // ---------- Flush Cache Before Running Prefetching ----------
    for (size_t i = 0; i < embedding_table.size(); i += 16)
    {
        _mm_clflush(&embedding_table[i]);
    }
    _mm_mfence();
    
    #endif
    // Run naive code
    long long time_without_prefetch = naive_emb(embedding_table, input, offsets);
    

    #ifdef PREFETCH
    // ---------- Flush Cache Before Running Prefetching ----------
    for (size_t i = 0; i < embedding_table.size(); i += 16)
    {
        _mm_clflush(&embedding_table[i]);
    }
    _mm_mfence();
    #endif
    // Run emb with software prefetching
    long long time_with_prefetch = run_with_prefetching(embedding_table, input, offsets);
    
     //Run emb with simd
    long long time_with_simd = run_with_simd(embedding_table, input, offsets);
    // Run emb with software prefetching and simd
    long long time_with_prefetch_simd = run_with_prefetching_simd(embedding_table, input, offsets);

    // Compute speedup/
    double speedup1 = static_cast<double>(time_without_prefetch) / time_with_prefetch;
    double speedup2 = static_cast<double>(time_without_prefetch) / time_with_simd;
    double speedup3 = static_cast<double>(time_without_prefetch) / time_with_prefetch_simd;
    cout << fixed << setprecision(3);
    cout << "\n\nSpeedup (with software prefetching) = " << speedup1 << "x\n";
    cout << "Speedup (with simd) = " << speedup2 << "x\n";
    cout << "Speedup (with software prefetching and simd) = " << speedup3 << "x\n";

    return 0;
}
