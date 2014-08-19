//
//  main.cpp
//  ParallelContextualSearch
//
//  Created by Anubhav on 8/10/14.
//  Copyright (c) 2014 anubhav. All rights reserved.
//

#include "indexer.h"
#include "search.h"
#include <iostream>
#include <map>
#include <vector>
#include <boost/thread/thread.hpp>


#include <iostream>
#include <boost/program_options.hpp>


using namespace std;

namespace po = boost::program_options;

const char* toolName = "pcs";

namespace {

    /*! @brief Define command line options for this entrypoint */
    struct Options {
        std::vector<std::string> corpusPath;
        std::string dbPath;
        std::string outputDbPath;
        std::string db;
        bool repin = false;

        po::variables_map processCmdLine(int argc, char **argv, po::options_description& desc)
        {
            po::variables_map vm;
            po::options_description inputOpts("Input settings");
            inputOpts.add_options()

            ("corpusPath,c",po::value(&corpusPath), "Path to a corpus file")
            ("dbPath,d", po::value(&dbPath), "Path of database")
            ("outputDbPath,o", po::value(&outputDbPath), "(optional) Path to output db")
            // example to add flags
            // ("repin,p","(optional) flag to repin coastedPath headings")
            ;

            desc.add_options()
            ("help,h", "Print this help")
            ;
            desc.add(inputOpts);

            po::store(po::parse_command_line(argc, argv, desc), vm);

            // handle command-line options
            if (vm.count("help")) {
                std::cout << desc << std::endl;
                exit(0);
            }
            if (vm.count("repin")) {
                repin = true;
            }
            if (!(vm.count("corpusPath") || (vm.count("dbPath")))) {
                throw po::error("Include corpusPath or dbPath");
            }
            po::notify(vm);
            return vm;
        }
    };
}

// TODO: enable regex
// 1. parse
// 2. run queries

int main(int argc, char ** argv) {

    po::options_description desc("Shows the search space");
    Options options;
    po::variables_map vm = options.processCmdLine(argc, argv, desc);

    std::map<std::string, std::vector<std::pair<std::string, size_t> > > wordMap;
    std::vector<std::string> corpi;

    cout << "Welcome to the search console, hit Ctrl C to end" << endl;
    for (std::string c: options.corpusPath) {
        corpi.push_back(c);
    }

    // 1. Index words in corpi
    std::shared_ptr<Indexer> indexer;
    if (options.outputDbPath.length() > 0) {
        indexer = std::make_shared<Indexer>(Indexer(corpi, options.outputDbPath));
    } else {
        indexer = std::make_shared<Indexer>(Indexer(corpi));
    }
    cout << "Indexing complete" << endl;
    // <Load word map>
    // 2. Search
    Search search((*indexer).wordMap);
    // e.g. [w1, w2, max=(10, 10)] -> returns list of [(corpus, word), int] indicating how many words away w2 was found from w1 within an upperbound of <max>. null if not found within range
    do {
        std::string w1, w2;
        cout << "Please enter first word" << endl;
        cin >> w1;
        cout << "Please enter second word" << endl;
        cin >> w2;
        if ((w1.length() == 0) || (w2.length() == 0)) {
            cout << "Words cannot be blank" << endl;
        }
        auto results = search.search(w1, w2);
        cout << results.size() << " results found" << endl;
    } while(!cin.eof());

}
