//
//  indexer.h
//  ParallelContextualSearch
//
//  Created by Anubhav on 8/10/14.
//  Copyright (c) 2014 anubhav. All rights reserved.
//

#ifndef ParallelContextualSearch_indexer_h
#define ParallelContextualSearch_indexer_h

#include <iostream>
#include <vector>
#include <map>
#include <boost/optional.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>


using namespace std;

class Indexer {
    public:
    Indexer(std::vector<std::string> corpi);
    Indexer(const Indexer& indexer);
    Indexer(std::vector<std::string> corpi, std::string outputPath);
    Indexer(std::string inputPath);
    std::map<std::string, std::vector<std::pair<std::string, size_t> > > wordMap;


    // Method that indexes list of corpus and outputs file
    void index(std::vector<std::string> corpi, std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap);
    void index(std::vector<std::string> corpi, std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap, boost::optional<std::string> outputPath);
    private:
    static void p_index(std::map<std::string, std::vector<std::pair<std::string, size_t> > >& , std::string, boost::shared_mutex&);
    // Method that outputs to file
    static void output(const std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap, std::string outputPath);
    boost::shared_mutex mapLock;


};
#endif
