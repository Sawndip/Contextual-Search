//
//  search.h
//  ParallelContextualSearch
//
//  Created by Anubhav on 8/10/14.
//  Copyright (c) 2014 anubhav. All rights reserved.
//

#ifndef __ParallelContextualSearch__search__
#define __ParallelContextualSearch__search__

#include <iostream>
#include <map>
#include <vector>
#include "boost/optional.hpp"

// TODO: add caching of files

using namespace std;
class Search {
    public:
    Search(std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap);
    // static method that loads word map or creates it based on list of input corpus strings (outputs errors if corpus provided not found)
    void loadMap(std::string path);
    std::vector<std::tuple<std::string, std::string, size_t> > search(std::string w1, std::string w2);
    // bounds.first is max words to left, bounds.second is max words to right
    std::vector<std::tuple<std::string, std::string, size_t> > search(std::string w1, std::string w2, std::pair<size_t, size_t> bounds);


    private:
    std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap;
    // method that returns word to left or right of current position
    enum Direction {
        Left,
        Right
    };
    
    // TODO: add boundary conditions check
    static std::pair<std::string, std::size_t>  scanWord(std::string corpusPath, size_t pos, Search::Direction direction);
    static std::pair<std::string, std::size_t>  scanWord(std::string corpusPath, size_t pos, Search::Direction direction, int n);
};
#endif /* defined(__ParallelContextualSearch__search__) */
