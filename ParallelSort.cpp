#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <getopt.h>

using namespace std;

mutex mut;

/*Functions' prototypes*/

/* 
Function break vector into t_num parts and sorts them
in different threads. Then parts are merged. The algorythm
is fast, but uses extra memory for merging, so it may have terrible
efficiency by memory, while sorting large arrays
Args:
Values - reference to vector of unsigned ints to be sorted
t_num - number of threads to be used
*/
void Threaded_Sort(vector<unsigned int> &Values, int t_num);

/*
Function, running by one thread, while sorting.
Threads in that case have no shared memory, so
there is no need for synchronization.
Args:
Values - reference to vector of unsigned ints to be sorted
begin - first border
end - second border
*/
void Threaded_It(vector<unsigned int> &Values, int begin, int end);

int main(int argc, char *argv[])
{
    /*Creating variable, containing path to file
    and intialize it with default value*/

    string path = "Array";

    /*Cmd arguments handling*/
    while (true)
    {

        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"file", required_argument, 0, 'f'}};
        int option_index = 0;

        int cur_opt = getopt_long(argc, argv, "hf:", long_options, &option_index);

        if (cur_opt == -1)
            break;

        switch (cur_opt)
        {

        /*Help information*/
        case 'h':
            cout << "Options list:" << endl;
            cout << "-h [ --help ]\t\tShows help" << endl;
            cout << "-f [ --file ] arg\tInput path to file with data" << endl;
            cout << "if -f flag isn't used, program will try to use default file ./Array" << endl;
            return 0;
            break;

        /*Setting path to file*/
        case 'f':
            path = optarg;
            break;

        case '?':
            /*Error already printed by getopt_long*/
            return 1;
            break;

        default:
            break;
        }
    }

    vector<unsigned int> Values(0);
    ifstream input(path, ios::binary);

    /*Checking, if file is open*/
    if (input.good())
    {
        /*Read values from file*/
        while (input.peek() != EOF)
        {
            unsigned int value;

            /*Here can be errors, while reading from file, so
            it must be handled*/
            try
            {
                input.read((char *)&value, sizeof(value));
                Values.push_back(value);
            }
            catch (...)
            {
                cout << "Error occured while reading the file" << endl;
                return 1;
            }
        }
        input.close();

        /*Sorting*/
        cout << "Sorting..." << endl;
        Threaded_Sort(ref(Values), sysconf(_SC_NPROCESSORS_ONLN));

        /*Output to file*/
        cout << "Writing to file..." << endl;

        remove("Sorted_Array");
        ofstream output("Sorted_Array", ios::binary);
        for (auto item = Values.begin(); item < Values.end(); item++)
        {
            /*cout << *item << endl;*/
            output.write((char *)&(*item), sizeof(*item));
        }
        cout << "Done" << endl;
    }
    else
        cout << "Error, while trying to open the file" << endl;

    return 0;
}

/*Implementation of functions*/

void Threaded_Sort(vector<unsigned int> &Values, int t_num)
{
    thread threads[t_num];
    int nodes[t_num + 1];
    nodes[0] = 0;

    /*Cheking, if size of vector is lesser, than 2*t_num.
    If it is so, then no need to start threads,
    because parts will be of size 1 and it will not be
    efficient*/
    if (2 * t_num > Values.size())
        sort(Values.begin(), Values.end());
    else
    {
        /*Starting t_num threads to sort one part of vector each*/
        for (int i = 0; i < t_num; i++)
        {
            nodes[i + 1] = (i + 1) * Values.size() / t_num;
            threads[i] = thread(Threaded_It, ref(Values), nodes[i], nodes[i + 1]);
        }

        /*Join those threads*/
        for (int i = 0; i < t_num; i++)
        {
            if (threads[i].joinable())
                threads[i].join();
        }

        /*Merge sorted parts of vector*/
        for (int i = 1; i < t_num; i++)
        {
            inplace_merge(Values.begin(), Values.begin() + nodes[i], Values.begin() + nodes[i + 1]);
        }
    }
}

void Threaded_It(vector<unsigned int> &Values, int begin, int end)
{
    sort(Values.begin() + begin, Values.begin() + end);
}


