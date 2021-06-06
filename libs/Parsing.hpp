#pragma once

#include <sys/stat.h>
#include <getopt.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

/*Base class*/
class Parsing
{
protected:

    /*cmd args*/
    int argc;
    char **argv;

public:
    
    /*Constructor*/
    Parsing(int &argc, char **&argv);

    /*Method, which parses data from cmd*/
    void Parse();

    /*Destructor*/
    ~Parsing();
};


class ParallelSortPars : public Parsing
{
public:

    /*Constructor*/
    ParallelSortPars(int &argc, char **&argv);

    /*
    Method, which parses data from cmd
    Args:
    input_path, output_path - variables to save parsed data
    */
    void Parse(std::string &input_path, std::string &output_path);

    /*Destructor*/
    ~ParallelSortPars();
};


class FileGeneratorPars : public Parsing
{
public:

    /*Constructor*/
    FileGeneratorPars(int &argc, char **&argv);

    /*
    Method, which parses data from cmd
    Args:
    kilosize, size - variables to save parsed data
    */
    void Parse(unsigned int &kilosize, unsigned long long int &size);

    /*Destructor*/
    ~FileGeneratorPars();
};


class SortCheckPars : public Parsing
{
public:

    /*Constructor*/
    SortCheckPars(int &argc, char **&argv);

    /*
    Method, which parses data from cmd
    Args:
    path - variable to save parsed data
    */
    void Parse(std::string &path);

    /*Destructor*/
    ~SortCheckPars();
};