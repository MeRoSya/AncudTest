#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <getopt.h>

using namespace std;

mutex mut;

/*Functions' prototypes*/
void Threaded_Sort(vector<unsigned int> &, int);       /*Function sorts data using std::sort and threads*/
void Threaded_It(vector<unsigned int> &, int, int);    /*One-thread-func while sorting*/
void Threaded_Merge(vector<unsigned int> &, int, int); /*One-thread-func while merging*/

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

        int c = getopt_long(argc, argv, "hf:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
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
        ofstream output("Sorted_Array", ios::binary);
        for (auto item = Values.begin(); item < Values.end(); item++)
        {
            output.write((char *)&(*item), sizeof(*item));
        }
        cout << "Done" << endl;
    }
    else
        cout << "Error, while trying to open the file" << endl;

    return 0;
}

/*Implementation of functions*/

/* 
Function break vector into t_num parts and sotrs them
in different threads. Then parts are merged. The algorythm
is fast, but uses extra memory for merging, so it may have terrible
efficiency by memory, while sorting large arrays
*/
void Threaded_Sort(vector<unsigned int> &Values, int t_num)
{
    thread threads[t_num];
    int begin[t_num];
    int end[t_num];
    for (int i = 0; i < t_num; i++)
    {
        if (i < t_num - 1)
        {
            begin[i] = i * Values.size() / t_num;
            end[i] = (i + 1) * Values.size() / t_num;
        }
        else
        {
            begin[i] = i * Values.size() / t_num;
            end[i] = Values.size();
        }
        threads[i] = thread(Threaded_It, ref(Values), begin[i], end[i]);
    }
    for (int i = 0; i < t_num; i++)
    {
        if (threads[i].joinable())
            threads[i].join();
        threads[i] = thread(Threaded_Merge, ref(Values), begin[i], end[i]);
    }
    for (int i = 0; i < t_num; i++)
    {
        if (threads[i].joinable())
            threads[i].join();
    }
}

/*Function, running by one thread, while sorting.
Threads in that case have no shared memory, so
there is no need for synchronization.*/
void Threaded_It(vector<unsigned int> &Values, int begin, int end)
{
    sort(Values.begin() + begin, Values.begin() + end);
}

/*Function, running by one thread, while merging.
Threads have access to shared memory, so they have 
to be synchronized. I use mutex for that.*/
void Threaded_Merge(vector<unsigned int> &Values, int begin, int end)
{
    mut.lock();
    inplace_merge(Values.begin(), Values.begin() + begin, Values.begin() + end);
    mut.unlock();
}