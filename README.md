# Burrows-Wheeler-Transform-Backward-Search

// Date Created: 22 APR 2016
// Date Revised: 29 APR 2016
// Author: Diwei Chen

This program is based on BWT backward search algorithm to implement pattern matches on a given encoded bwt file. The encoded data will be divided into num_of_interval = 16,000 parts as the maximum size of a given file is 160M and the size of each interval is chosen as interval_size = 10,000.At first, it will read through the whole encoded file once to initialise two arrays which are first[num_of_char](num_of_char = 127) and  occ[num_of_interval][num_of_char] and output an index file that contains the data of first[] and occ[][]. The first[] contains each first occurrence position of characters with ascii number bewteen(0 ~ 126) in the bwt file. The occ[][] has the frequency of each characters(0 ~ 126) in a interval.
The index file is built to improve the efficiency for the rest queries.

In order to find a matching text pattern of original file, we search from the last character to the first one of the pattern string. Firstly, we look for all of the matching of last character. As we have already known the first occurrence position of this char by first[] and first[] is an array sorted by ascii number, we can easily get the number of the matching of the last char by subtraction of first occurrence of the char followed by and its first and last position. Then we have built a "gap". As the character before the last character always exists in the gap by BWT, we use this feature to find the number of the character in the gap by counting the frequency of it, this is implemented in Occ() function and it will return the occurrence of a targeted character before a given position. As long as we get the frequency, we can find its position by using first[] again. Then we can keep BackwardSearch() util meet the stop symbol('['). And EliminateDuplicates() function is to return the unique offset number.

Last but not least, with the usage of the occ[][] frequency matrix and the index file, it could helpfully improve the efficiency of searching by advoiding unneccesary and repeated calculations.
