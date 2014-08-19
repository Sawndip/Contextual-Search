//
//  indexer.cpp
//  ParallelContextualSearch
//
//  Created by Anubhav on 8/10/14.
//  Copyright (c) 2014 anubhav. All rights reserved.
//

#include "indexer.h"
#include <fstream>
#include <pthread.h>
#include "boost/thread.hpp"
#include <map>
#include <algorithm>


// TODO: parallelize this part
// typical mapreduce
#define NUM_THREADS     5
//boost::thread threads[NUM_THREADS];


Indexer::Indexer(std::vector<std::string> corpi)
{
    // index manually
    index(corpi, wordMap);
}

Indexer::Indexer(std::vector<std::string> corpi, std::string outputPath)
{
    // index manually
    index(corpi, wordMap, outputPath);
}


Indexer::Indexer(std::string inputPath)
{
    // load from file
}

Indexer::Indexer(const Indexer& indexer)
{
    this->wordMap = indexer.wordMap;
}

void Indexer::p_index(std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap, std::string corpusPath, boost::shared_mutex& mapLock)
{
    // TODO: tokenize and get name or pass in a <path, name> pair vector
    std::string corpusName = corpusPath;
    fstream fin(corpusPath, fstream::in);
    char ch;
    size_t charpos = 0;
    std::string wordBuffer = "";
    do {
        fin.get(ch);
        // TODO: dont count consecutive new lines
        if ((ch == ' ') || (ch == ':') || (ch == '\n') || (ch == '\t') || (ch =='.') || (ch =='\\') || (ch =='(') || (ch ==')') || (ch =='/') ){
            // end of word
            std::string word(wordBuffer);
            if (word != "") {
                std::transform(word.begin(), word.end(), word.begin(), ::toupper);
                boost::upgrade_lock<boost::shared_mutex> lock(mapLock);
                boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
                std::vector<std::pair<std::string, size_t> >& vec = wordMap[word];
                // cout << word << endl;
                vec.push_back(std::make_pair(corpusName, charpos - wordBuffer.length()));
            }
            wordBuffer.clear();
        } else {
            wordBuffer += ch;
        }
        charpos++;
    } while(!fin.eof());
}

void Indexer::index(std::vector<std::string> corpi, std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap)
{
    index(corpi, wordMap, boost::none);
}

void Indexer::index(std::vector<std::string> corpi, std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap, boost::optional<std::string> outputPath)
{
    boost::thread_group tgroup;
    for (size_t i = 0; i < corpi.size(); i++) {
        cout << "Indexing " << corpi[i] << endl;
        tgroup.create_thread(boost::bind(p_index, boost::ref(wordMap), corpi[i], boost::ref(mapLock)));
    }
    tgroup.join_all();
    cout << wordMap.size() << endl;
    cout << "=====" << endl;
    // Output wordmap
    // TODO: add a flag to specify output
    if (outputPath != boost::none) {
        output(wordMap, *outputPath);
        cout << "outputted data to " << *outputPath << endl;
    }
}

void Indexer::output(const std::map<std::string, std::vector<std::pair<std::string, size_t> > >& wordMap, std::string outputPath)
{

    ofstream fout(outputPath);

    fout << "{" << endl;
    for (auto& kvpair: wordMap) {
        std::string word;
        std::vector<std::pair<std::string, size_t> > positions;
        std::tie(word, positions) = kvpair;
        // output word
        fout << "\"" << word << "\": [" << endl;
        for (std::pair<std::string, size_t> position: positions) {
            // TODO: take care of trailing comma
            fout << "[" << "\"" << position.first << "\"" << ", " << position.second << "]," << endl;
        }
        // TODO: and here
        fout << "]," << endl;
    }
    fout << "}" << endl;
    fout.close();
}
