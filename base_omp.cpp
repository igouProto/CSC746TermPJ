// base algorithm that takes in a text file, reads it, and count all the unique words in the file
// accepts a file name (we're not doing threading yet)

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <set>
#include <algorithm>
#include <cctype>
#include <vector>

#include <chrono> // for the timer

#include <omp.h>

bool compare(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
{
    return a.second > b.second;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    // read the file in
    std::ifstream file(argv[1]);

    if (!file.is_open())
    {
        std::cout << "Could not open file " << argv[1] << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Opened file " << argv[1] << std::endl;

        // get the file's size
        file.seekg(0, std::ios::end);
        int size = file.tellg();
        file.seekg(0, std::ios::beg); // remember to reset the file pointer

        // read the entire file into the buffer the size of the file
        // std::vector<char> buffer(size);
        char *buffer = new char[size];
        file.read(&buffer[0], size);

        // Delimeter for tokenizing the chunks
        const char *delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

        // tokenize + count words toward a tally
        // the tally is a hash table of words and their counts
        std::unordered_map<std::string, int> tally;

        // const int buffer_size = buffer.size();
        const int buffer_size = size;
        std::string word;
        // int bytes_read = 0;

        // start the timer
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();

        // parallel region
        #pragma omp parallel private(word) //, bytes_read)
        {
            int num_threads = omp_get_num_threads();
            int thread_id = omp_get_thread_num();

            // this thread would be reading: buffer_size / num_threads
            // with the offset of: thread_id * (buffer_size / num_threads)
            int chunk_size = buffer_size / num_threads;
            int offset = thread_id * chunk_size;

            // process its share of the buffer by tokenizing it
            char *hold;
            char *token = strtok_r(&buffer[offset], delim, &hold); // thread-safe

            // printf("Thread %d start, with chunk size %d\n", thread_id, chunk_size);

            while (token != NULL){
                // turn it to lowercase
                word = std::string(token);
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                // skip if char count is less than 6
                if (word.length() < 6){
                   token = strtok_r(NULL, delim, &hold);
                   continue;
                }
                // count to tally, move on to the next word
                #pragma omp critical
                {
                    tally[word] += 1;
                }
                token = strtok_r(NULL, delim, &hold);
                // break if we read enough so each thread reads the same amount
                if (token >= &buffer[offset + chunk_size]){
                    break;
                }
            }

            // printf("Thread %d end\n", thread_id);
        }

        // stop the timer
        std::chrono::time_point<std::chrono::high_resolution_clock> end_time = std::chrono::high_resolution_clock::now();

        file.close();

        // sort the tally by count in desc. order
        std::vector<std::pair<std::string, int>> sorted_tally(tally.begin(), tally.end());
        std::sort(sorted_tally.begin(), sorted_tally.end(), compare);

        // output results
        printf("Chunk size: %d\n", size);
        int i = 0;
        for (const auto &pair : sorted_tally)
        {
            printf("%2d. %s: %d\n", i, pair.first.c_str(), pair.second);

            if (++i == 10)
            {
                break;
            }
        }

        auto duration = end_time - start_time;
        auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(duration);
        printf("Time taken to count words: %ld microsecs\n", duration_ms.count());

        // clean up
        delete[] buffer;
    }

    return 0;
}
