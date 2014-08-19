Contextual-Search
=================

A tool which indexes multiple text documents and enables searching of nearby words

written in C++ using stl and boost

Usage:
  Mode 1: Read multiple docs (optionally output pcs db to file)
  ./pcs -c <doc1> -c <doc2> ... (-o <path-to-db-ouput>)
  
  !! Under Construction !!
  Mode 2: Read from pcs db
  ./pcs -d <path-to-db>
  

Why is this useful?

This tool provides a way to quickly traverse multiple documents in multiple locations based on word links.
Example:

Input: Knowledge, Cognitive
Given this input and a left and a right bound, the search finds all words that meet this critera
It then returns the word and its position in the file.
For this example,

<----------------------------------[knowledge]--------------------------->
...Cognitive processes use existing knowledge and generate new knowledge...
  [Cognitive]

Cognitive is found 4 words away from knowledge.

This kind of application could be useful in a machine learning or analytics context where one would need to know what words are located in close proximity of a particular word in order to better understand the context the word was used in.
