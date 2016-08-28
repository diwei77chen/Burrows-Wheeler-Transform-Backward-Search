// Date Created: 22 APR 2016
// Date Revised: 29 APR 2016
// Author: Diwei Chen

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "bwtsearch.h"

const unsigned int num_of_char = 127;
const unsigned int num_of_interval = 16000;      // maximum: 160MB / interval_size = 16000
const int interval_size = 10000;
		
// return the number of occurrences of a given character before its position in the bwt encoded file
unsigned int Occ(unsigned int (&occ_matrix)[num_of_interval][num_of_char], int pos, char ch, std::ifstream& input_file, const char* path);

// return a vectore stored with unique offset numbers
std::vector<unsigned int>& EliminateDuplicates(int (&first)[num_of_char], std::ifstream& input_file, unsigned int (&occ_matrix)[num_of_interval][num_of_char], int pos, std::vector<unsigned int>& numbers, std::string& str_number, int& flag_start_count, const char* path);

// return the number of matching patterns of the original text file
int BackwardSearch(int (&first)[num_of_char], std::ifstream& input_file, unsigned int (&occ_matrix)[num_of_interval][num_of_char], std::vector<char>& pattern, int front_occ, int num_char_match, int& num_pattern_match, std::string& arg, unsigned int file_size, const char* path);

int main(int argc, const char * argv[]) {
    std::string argument = argv[1];
    const char* path = argv[2];
    const char* idx = argv[3];
    std::string str_pattern = argv[4];
    std::vector<char> pattern(str_pattern.begin(), str_pattern.end());
    
    std::ifstream input_file(path);
    
    unsigned int file_size;     // the number of characters store in the file
    input_file.seekg(0, input_file.end);
    file_size = static_cast<unsigned int>(input_file.tellg());
    input_file.seekg(0, input_file.beg);        // set the cursor point to the begining of the file
    
    int first[num_of_char] = {};        // initialise each element of the array as 0
    for (unsigned int i = 0; i < num_of_char; ++i) {
        first[i] = -1;
    }
    
    unsigned int occ_matrix[num_of_interval][num_of_char] = {{0}};
    
    std::freopen(path, "r", stdin);
    if (std::ifstream(idx).good()) {
        std::ifstream input_idx(idx);
        unsigned int i = file_size / interval_size;
        
        // initialise occ_matrix[][] by index file
        for (unsigned int k = 0; k < i; ++k) {
            for (unsigned int m = 0; m < num_of_char; ++m) {
                input_idx >> occ_matrix[k][m];
            }
        }
        // initialise first[] by index file
        for (unsigned int p = 0; p < num_of_char; ++p) {
            input_idx >> first[p];
        }
        input_idx.close();
    }
    else {
        std::ofstream output_file(idx);
        unsigned int j = 0;
        unsigned int k = 0;
        int ascii;
        char ch_current;
        for (unsigned int i = 0; i < file_size; ++i) {      // i is the index of each character
            scanf("%c", &ch_current);      // scan character of file to arr_interval
            ascii = static_cast<int>(ch_current);
            ++first[ascii];
            j = k;
            k = i / interval_size;
            if (j != k) {
                for (unsigned int p = 0; p < num_of_char; ++p) {
                    output_file << occ_matrix[j][p] << " ";
                    occ_matrix[k][p] += occ_matrix[j][p];
                }
                output_file << '\n';
            }
            occ_matrix[k][ascii]++;        // construct occurrence matrix
        }
        // construct first[]
        for (int m = num_of_char - 1; m >= 0; --m) {
            if (first[m] != -1) {
                int count = 0;
                for (int n = 0; n < m; ++n) {
                    if (first[n] != -1) {
                        count += first[n] + 1;
                    }
                }
                first[m] = count;
            }
            
        }
        for (unsigned int m = 0; m < num_of_char; ++m) {
            if (first[m] != -1) {
                first[m] = 0;
                break;
            }
        }
        // output first to index file
        for (unsigned int m = 0; m < num_of_char; ++m) {
            output_file << first[m] << " ";
        }
        output_file.close();
    }
    
    
    
    std::fclose(stdin);
    
    int num_pattern_match = -1;
    num_pattern_match = BackwardSearch(first, input_file, occ_matrix, pattern, -1, -1, num_pattern_match, argument, file_size, path);
    if (num_pattern_match != -1) {
        std::cout << num_pattern_match << std::endl;
    }
    
    input_file.close();
    
    return 0;
    
}

unsigned int Occ(unsigned int (&occ_matrix)[num_of_interval][num_of_char], int pos, char ch, std::ifstream& input_file, const char* path) {
    unsigned int count = 0;
    int ascii = static_cast<int>(ch);
    int occ_first_dimension = pos / interval_size - 1;     // using occ matrix
    char ch_tmp;
    
    if (occ_first_dimension >= 0) {     // making use of the occ_matrix
        count = occ_matrix[occ_first_dimension][ascii];
        int i = (occ_first_dimension + 1) * interval_size;
        input_file.seekg(i, input_file.beg);
        for (; i <= pos; ++i) {
            input_file.get(ch_tmp);
            if (ch_tmp == ch) {
                ++count;
            }
        }
    }
    else {
        std::freopen(path, "r", stdin);
        for (int i = 0; i <= pos; ++i) {    // counting from original bwt file
            scanf("%c", &ch_tmp);
            if (ch_tmp == ch) {
                ++count;
            }
        }
        std::fclose(stdin);
    }
    return count;
};

std::vector<unsigned int>& EliminateDuplicates(int (&first)[num_of_char], std::ifstream& input_file, unsigned int (&occ_matrix)[num_of_interval][num_of_char], int pos, std::vector<unsigned int>& numbers, std::string& str_number, int& flag_start_count, const char* path) {
    char ch_pos;
    input_file.seekg(pos, input_file.beg);
    input_file.get(ch_pos);
    int ascii = static_cast<int>(ch_pos);
    
    while (ch_pos != '[') {
        if (ch_pos == ']' ) {       // if meet ']', ready to get number
            pos = Occ(occ_matrix, pos, ch_pos, input_file, path) - 1 + first[ascii];
            input_file.seekg(pos, input_file.beg);
            input_file.get(ch_pos);
            ascii = static_cast<int>(ch_pos);
            flag_start_count = 1;
        }
        else if (flag_start_count == 0) {
            pos = Occ(occ_matrix, pos, ch_pos, input_file, path) - 1 + first[ascii];
            input_file.seekg(pos, input_file.beg);
            input_file.get(ch_pos);
            ascii = static_cast<int>(ch_pos);
            continue;
        }
        
        if (flag_start_count == 1) {        // start to get number
            str_number = ch_pos + str_number;
            pos = Occ(occ_matrix, pos, ch_pos, input_file, path) - 1 + first[ascii];
            input_file.seekg(pos, input_file.beg);
            input_file.get(ch_pos);
            ascii = static_cast<int>(ch_pos);
        }
    }
    
    unsigned int num = std::stoi(str_number);
    // numbers contain num
    if (std::find(numbers.begin(), numbers.end(), num) != numbers.end()) {
        // do nothing
    }
    else {  // numbers does not contain num
        numbers.push_back(num);
    }
    
    return numbers;
};

int BackwardSearch(int (&first)[num_of_char], std::ifstream& input_file, unsigned int (&occ_matrix)[num_of_interval][num_of_char], std::vector<char>& pattern, int front_occ, int num_char_match, int& num_pattern_match, std::string& arg, unsigned int file_size, const char* path) {
    char ch = pattern.back();
    pattern.pop_back();
    
    if (num_char_match == -1) {
        if (pattern.size() != 0) {
            int ascii = static_cast<int>(ch);
            int pos1 = first[ascii];        // position of first occurrence of char
            ++ascii;
            bool flag_last = 0;     // flag: if the last character
            while (first[ascii] == -1) {
                ++ascii;
                if (ascii == num_of_char ) {        // meet the boundary
                    flag_last = 1;
                    break;
                }
            }       // ascii now points to next char in sorted order
            int pos2;
            if (flag_last == 0) {
                pos2 = first[ascii];
            }
            else{
                pos2 = file_size - 1;       // points to the index of last character
            }
            front_occ = Occ(occ_matrix, pos1 - 1, pattern.back(), input_file, path);
            int back_occ = Occ(occ_matrix, pos2, pattern.back(), input_file, path);
            num_char_match = back_occ - front_occ;
        }
        else if (pattern.size() == 0) {
            int ascii = static_cast<int>(ch);
            int pos1 = first[ascii];
            ++ascii;
            bool flag_last = 0;
            while (first[ascii] == -1) {
                ++ascii;
                if (ascii == num_of_char) {
                    flag_last = 1;
                    break;
                }
            }
            int pos2;
            if (flag_last == 0) {
                pos2 = first[ascii];
            }
            else {
                pos2 = file_size;
            }
            num_pattern_match = pos2 - pos1;
            pos2 = pos2 - 1;    // pos2 is the last occurrence of the current character now
            if (arg == "-n") {
                return num_pattern_match;
            }
            else if (arg == "-r") {
                //  query will not result in no matches
                std::vector<unsigned int> numbers;
                std::string str_number;
                int flag_start_count;
                for (int i = pos1; i <= pos2; ++i) {
                    flag_start_count = 0;
                    str_number.clear();
                    numbers = EliminateDuplicates(first, input_file, occ_matrix, i, numbers, str_number, flag_start_count, path);
                }
                return static_cast<int>(numbers.size());
            }
            else if (arg == "-a") {
                //  query will not result in no matches
                std::vector<unsigned int> numbers;
                std::string str_number;
                int flag_start_count;
                for (int i = pos1; i <= pos2; ++i) {
                    flag_start_count = 0;
                    str_number.clear();
                    numbers = EliminateDuplicates(first, input_file, occ_matrix, i, numbers, str_number, flag_start_count, path);
                }
                std::sort(numbers.begin(), numbers.end());
                for (auto itr = numbers.begin(); itr != numbers.end(); ++itr) {
                    std::cout << "[" << *itr << "]" << std::endl;
                }
                return -1;
            }
        }
    }
    else if (num_char_match != -1) {
        if (pattern.size() != 0) {
            int ascii = static_cast<int>(ch);
            int pos1 = first[ascii] + front_occ;
            int pos2 = pos1 + (num_char_match - 1);
            front_occ = Occ(occ_matrix, pos1 -1 , pattern.back(), input_file, path);
            int back_occ = Occ(occ_matrix, pos2, pattern.back(), input_file, path);
            num_char_match = back_occ - front_occ;
        }
        else if (pattern.size() == 0) {
            num_pattern_match = num_char_match;
            int pos1 = -1;
            int pos2 = -1;
            int ascii = static_cast<int>(ch);
            pos1 = first[ascii] + front_occ;
            pos2 = pos1 + (num_char_match - 1);
            if (arg == "-n") {
                return num_pattern_match;
            }
            else if (arg == "-r") {
                std::vector<unsigned int> numbers;
                std::string str_number;
                int flag_start_count;
                for (int i = pos1; i <= pos2; ++i) {
                    flag_start_count = 0;
                    str_number.clear();
                    numbers = EliminateDuplicates(first, input_file, occ_matrix, i, numbers, str_number, flag_start_count, path);
                }
                return static_cast<int>(numbers.size());
            }
            else if (arg == "-a") {
                //  query will not result in no matches
                std::vector<unsigned int> numbers;
                std::string str_number;
                int flag_start_count;
                for (int i = pos1; i <= pos2; ++i) {
                    flag_start_count = 0;
                    str_number.clear();
                    numbers = EliminateDuplicates(first, input_file, occ_matrix, i, numbers, str_number, flag_start_count, path);
                }
                std::sort(numbers.begin(), numbers.end());
                for (auto itr = numbers.begin(); itr != numbers.end(); ++itr) {
                    std::cout << "[" << *itr << "]" << std::endl;
                }
                return -1;
            }
        }
    }
    return BackwardSearch(first, input_file, occ_matrix, pattern, front_occ, num_char_match, num_pattern_match, arg, file_size, path);
};
