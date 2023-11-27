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

bool compare(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
{
    return a.second > b.second;
}

#define cache_size 64 * 1024 // 64KB
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
        char* buffer = new char[size];
        file.read(&buffer[0], size);

        // Delimeter for tokenizing the chunks
        const char *delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

        // tokenize + count words toward a tally
        // the tally is a hash table of words and their counts
        std::unordered_map<std::string, int> tally;

        // start the timer
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();

        // local cache
        // std::vector<char> cache(cache_size);
        char cache[cache_size];

        int copy_size;
        int buffer_offset = 0; // offset into the buffer holding the file content
        while (size > 0)
        {
            // copy from buffer to cache
            copy_size = size > cache_size ? cache_size : size;
            memcpy(&cache[0], &buffer[buffer_offset], copy_size);

            // process file content by tokenizing it
            char *token = std::strtok(&cache[0], delim);
            std::string word;
            while (token != NULL)
            {
                // get the word
                word = std::string(token);

                // turn it to lowercase
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);

                // count words with 6 or more characters only. skip if less than 6
                if (word.length() < 6)
                {
                    token = std::strtok(NULL, delim);
                    continue;
                }

                // write to tally
                tally[word] += 1;

                // move onto the next word
                token = std::strtok(NULL, delim);
            }

            // update remaining size
            size -= copy_size;
            buffer_offset += copy_size;
        }

        // stop the timer
        std::chrono::time_point<std::chrono::high_resolution_clock> end_time = std::chrono::high_resolution_clock::now();

        // close the file
        file.close();

        // sort the tally by count in desc. order
        std::vector<std::pair<std::string, int>> sorted_tally(tally.begin(), tally.end());
        std::sort(sorted_tally.begin(), sorted_tally.end(), compare);

        // output results
        printf("Chunk size: %d\n", buffer_offset);

        // print the top 10 words
        int i = 0;
        for (const auto &pair : sorted_tally)
        {
            printf("%2d. %s: %d\n", i, pair.first.c_str(), pair.second);

            if (++i == 10)
            {
                break;
            }
        }

        // print the time taken
        auto duration = end_time - start_time;
        auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(duration);
        // std::cout << "Time taken (excl. sorting tally): " << duration_ms.count() << " ms" << std::endl;
        printf("Time taken to count words: %ld microsecs\n", duration_ms.count());

        // clean up
        delete[] buffer;
    }

    return 0;
}
