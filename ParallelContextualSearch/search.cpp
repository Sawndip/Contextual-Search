//
//  search.cpp
//  ParallelContextualSearch
//
//  Created by Anubhav on 8/10/14.
//  Copyright (c) 2014 anubhav. All rights reserved.
//

#include "search.h"
#include <vector>
#include <cassert>
#include <fstream>
#include <algorithm>

Search::Search(std::map<std::string, std::vector<std::pair<std::string, size_t> > >& _wordMap)
    : wordMap(_wordMap)
{
}


std::vector<std::tuple<std::string, std::string, size_t> > Search::search(std::string w1, std::string w2)
{
    return Search::search(w1, w2, std::make_pair(10, 10));
}

// TODO: make pair take in nulls
// TODO: parallelize
std::vector<std::tuple<std::string, std::string, size_t> >  Search::search(std::string w1, std::string w2, std::pair<size_t, size_t> bounds)
{
    std::transform(w1.begin(), w1.end(), w1.begin(), ::toupper);
    std::transform(w2.begin(), w2.end(), w2.begin(), ::toupper);
    assert(w1 != w2);
    assert(bounds.first > 0);
    assert(bounds.second > 0);
    std::vector<std::tuple<std::string, std::string, size_t> > results;
    const std::vector<std::pair<std::string, size_t> >& positions = wordMap[w1];
    size_t left, right;
    std::tie(left, right) = bounds;

    if (positions.empty()) {
        cout << w1 << " does not exist in word map" << endl;
        // return null here
    }
    for (std::pair<std::string, size_t> position: positions) {
        // Load file here (read-only to prevent fs concurrency issues when parallelizing)
        size_t pos = position.second;
        // find matches in words to left
        size_t nPos = pos;
        boost::optional<std::tuple<std::string, std::string, size_t> > leftPair;

        // search <left> words to the left of the current word
        for (size_t i = 0; i < left; i++) {
            std::pair<std::string, std::size_t> wordPair = scanWord(position.first, nPos, Direction::Left);
            cout << wordPair.first << endl;
            std::transform(wordPair.first.begin(), wordPair.first.end(), wordPair.first.begin(), ::toupper);
            if (wordPair.first == w2) {
                leftPair = std::make_tuple(position.first, wordPair.first, -i);
                break;
            }
            nPos = wordPair.second;
        }
        // find matches in words to right
        boost::optional<std::tuple<std::string, std::string, size_t> > rightPair;
        nPos = pos + w1.size();
        for (size_t i = 0; i < right; i++) {
            std::pair<std::string, std::size_t> wordPair = scanWord(position.first, nPos, Direction::Right);
            std::transform(wordPair.first.begin(), wordPair.first.end(), wordPair.first.begin(), ::toupper);
            if (wordPair.first == w2) {
                rightPair = std::make_tuple(position.first, wordPair.first, i);
                break;
            }
            nPos = wordPair.second;
        }

        // add matches to vector
        // check if leftPair exists
        if ((!leftPair.is_initialized()) && (!rightPair.is_initialized())) {
            // add null pair;
        }
        else if (!rightPair.is_initialized()) {
            // add leftPair
            results.push_back(*leftPair);
        } else if (!leftPair.is_initialized()) {
            // add rightPair
            results.push_back(*rightPair);
        } else {
            // both
            if (std::get<2>(*leftPair) > std::get<2>(*rightPair)) {
                // add right pair
                results.push_back(*rightPair);
            } else {
                // add left pair
                results.push_back(*leftPair);
            }
        }
    }
    return results;
}


std::pair<std::string, std::size_t> Search::scanWord(std::string corpusPath, size_t nPos, Direction direction) {
    // LOAD cache or file
    ifstream fin(corpusPath);
    fin.seekg(0, fin.end);
    size_t len = fin.tellg();
    fin.seekg(0, fin.beg);
    assert(nPos < len);
    char c;
    size_t pos = nPos;
    std::string word;
    fin.seekg(nPos, fin.beg);
    size_t retpos;
    switch (direction) {

        case Left:
            // remove first whitespace to left of word
            pos -= 2;
            fin.seekg(pos, fin.beg);

            do {
                c = fin.get();
                // move 2 back
                fin.seekg(pos, fin.beg);
                pos -= 1;
            } while (c != ' ');
            // (at this point, ptr will be at last char of left to left word) so seek back to left (current) word
            fin.seekg(pos+2, fin.beg);
            fin >> word;
            retpos = (size_t)fin.tellg() - (word.size() + 1);
            break;
        case Right:
            /*
            do {
                c = fin.get();
                // move 1 forward
                // fin.seekg(pos + 1, fin.beg);
                pos++;
            } while (c != ' ');
             */
            fin >> word;
            // remove all punctuation and make it upper case
            // if more than one word, get first one and tellg to beginning of next one
            retpos = fin.tellg();
            break;
    }
    return std::make_pair(word, retpos);
}