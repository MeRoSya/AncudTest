#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <boost/filesystem.hpp>
#include <sys/stat.h>
#include <getopt.h>

using namespace std;

/*Mutex to synchronize threads, where is nessesery*/
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

/*
Function break in halfs and then fork one thread to call recursion
for the first half and call recursion for another half in this thread.
After they finished, sorted parts are merged.
Recursion continues, until either number of active threads not greater, than
system core num. When recursion ends, current range of vector sorts by std::sort
and then go on upper level of recursion, where sorted parts are merged.
It is about 1.5 times faster, than Threaded_Sort 

Comparasing for 1 GB file:
Rec_Threaded_Sort: 36648462 mcs
Threaded_Sort: 56514361 mcs

Args:
Values - reference to vector of unsigned ints to be sorted
begin - first border
end - second border
*/
void Rec_Threaded_Sort(vector<unsigned int> &Values, int begin, int end);

/*
Making merge sort, using files. It is not really fast (Sorting of file with
size of 100 MB is 24665 ms), but it uses fixed amount of RAM 
(aproximatelly 4 kb * 16 (size of all buffers), but might use a little more 
for other variables), so it can be used to sort really big files
Args:
path - path to file to sort
*/
void File_Sort(string path);

/*
Function splits initial file into many files with size of 4 kb (standart claster size),
in which values are already sorted. It is slow, but I don't know, how it could be
made faster.
Args:
path - path to file to be splitted
*/
void File_Split(string path);

/*
Function merges two files
Args:
file1 - path to the first file
file2 - path to the second file
*/
void File_Merge(string file1, string file2);

/*
Merges files in directory (making merge sort).
For merging it uses 2xthr threads, there thr - number of cores
(that number was experimentally confirmed as optimal).
Args:
delta - differece in the name of 
neighbour files
*/
void Threaded_Files_Merge_Sort(int delta);

/*
Function, performed by each thread
Args:
input - reference to input stream
f_num - number of current file
*/
void Threaded_File_Input(ifstream &input, int f_num);

int main(int argc, char *argv[])
{
    /*
    Creating variable, containing path to file
    and intialize it with default value
    */

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

    cout << "Starting processes..." << endl;
    File_Sort(path);

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

void Rec_Threaded_Sort(vector<unsigned int> &Values, int begin, int end)
{
    static atomic_int t_num(0);
    t_num++;
    if (!(t_num > sysconf(_SC_NPROCESSORS_ONLN) / 2) && (end > 128))
    {
        thread t(Rec_Threaded_Sort, ref(Values), 0, end / 2);
        t.join();
        Rec_Threaded_Sort(ref(Values), end / 2, end);
        inplace_merge(Values.begin() + begin, Values.begin() + end / 2, Values.begin() + end);
    }
    else
    {
        sort(Values.begin() + begin, Values.begin() + end);
    }
}

void File_Sort(string path)
{
    cout << "Reading from file..." << endl;

    /*
    Splitting initial file into many smaller ones, 
    elements in which are sorted
    */
    File_Split(path);

    /*Merging all sorted files*/
    Threaded_Files_Merge_Sort(1);

    /*Copying sorted file from temporary dir to main dir*/
    boost::filesystem::copy_file("0", "../Sorted_Array", boost::filesystem::copy_option::overwrite_if_exists);

    /*Going back to main dir*/
    boost::filesystem::current_path("..");

    /*Removing temporary dir*/
    cout << "Removing temporary files..." << endl;
    boost::filesystem::remove_all("temp");
}

void Threaded_Files_Merge_Sort(int delta)
{
    /*Setting first file in sequence*/
    int file1 = 0;

    /*Setting array of threads*/
    thread threads[2 * sysconf(_SC_NPROCESSORS_ONLN)];

    /*Merging all files in directory*/
    while (boost::filesystem::exists(to_string(file1)) && boost::filesystem::exists(to_string(file1 + delta)))
    {
        /*
        Starting 2*cores processes to merge files.
        Or less, if there are not so much files
        */
        for (int i = 0; i < 2 * sysconf(_SC_NPROCESSORS_ONLN); i++)
        {
            if (boost::filesystem::exists(to_string(file1)) && boost::filesystem::exists(to_string(file1 + delta)))
                threads[i] = thread(File_Merge, to_string(file1), to_string(file1 + delta));
            else
                break;
            file1 += 2 * delta;
        }

        /*Joining all threads*/
        for (int i = 0; i < 2 * sysconf(_SC_NPROCESSORS_ONLN); i++)
        {
            if (threads[i].joinable())
                threads[i].join();
        }
    }

    /*If there is more than 1 file in directory, start recursion with greater delta*/
    if (boost::filesystem::exists(to_string(0 + 2 * delta)))
        Threaded_Files_Merge_Sort(2 * delta);
    else
    {
        /*
        In case, if original size is less than 4 kb, final file name can be not 0, but
        any other (depends on which thread wrote it). So I rename that last element here
        */
        boost::filesystem::path p = boost::filesystem::current_path();
        for (auto it = boost::filesystem::directory_iterator(p); it != boost::filesystem::directory_iterator(); it++)
        {
            boost::filesystem::rename(it->path().filename().string(), "0");
        }
    }
}

void File_Merge(string file1, string file2)
{
    /*Open input streams*/
    ifstream input1(file1, ios::binary);
    ifstream input2(file2, ios::binary);

    /*Open output stream*/
    ofstream output("mrg_" + file1, ios::binary);
    unsigned int cur_value1;
    unsigned int cur_value2;

    /*Read first values*/
    input1.read((char *)&cur_value1, sizeof(cur_value1));
    input2.read((char *)&cur_value2, sizeof(cur_value2));

    /*Making merging for files*/
    while (input1.peek() != EOF && input2.peek() != EOF)
    {
        if (cur_value1 <= cur_value2)
        {
            output.write((char *)&cur_value1, sizeof(cur_value1));
            input1.read((char *)&cur_value1, sizeof(cur_value1));
        }
        else
        {
            output.write((char *)&cur_value2, sizeof(cur_value2));
            input2.read((char *)&cur_value2, sizeof(cur_value2));
        }
    }
    input1.close();
    input2.close();
    output.close();

    /*Removing initial files*/
    boost::filesystem::remove(file1);
    boost::filesystem::remove(file2);

    /*Rename merged file into name of the first file*/
    boost::filesystem::rename("mrg_" + file1, file1);
}

void File_Split(string path)
{
    ifstream input(path, ios::binary);

    /*Creating temporary directory*/
    mkdir("temp", S_IRWXU);
    int f_num = 0;

    thread threads[2 * sysconf(_SC_NPROCESSORS_ONLN)];

    if (input.good())
    {
        while (input.peek() != EOF)
        {
            /*
            Creating 2*cores threads to read from file.
            Or less, if there are not so much files
            */
            for (int i = 0; i < 2 * sysconf(_SC_NPROCESSORS_ONLN); i++)
            {
                threads[i] = thread(Threaded_File_Input, ref(input), f_num);
                f_num++;
            }

            /*Joining all threads*/
            for (int i = 0; i < 2 * sysconf(_SC_NPROCESSORS_ONLN); i++)
            {
                if (threads[i].joinable())
                    threads[i].join();
            }
        }
        input.close();
        boost::filesystem::current_path("./temp");
    }
    else
    {
        cout << "Error, while trying to open the file" << endl;
        exit(1);
    }
}

void Threaded_File_Input(ifstream &input, int f_num)
{

    vector<unsigned int> cur_values(1024, INT32_MAX);

    /*Checking if we finished reading the file*/
    mut.lock();

    bool is_end = input.peek() != EOF;

    mut.unlock();

    if (is_end)
    {
        /*Reading values of size 4 kb, or less into buffer*/
        mut.lock();

        input.read((char *)&cur_values[0], cur_values.size() * sizeof(cur_values[0]));

        mut.unlock();

        /*Sorting values in buffer*/
        sort(cur_values.begin(), cur_values.end());

        /*Write sorted values in the file with number f_num*/
        ofstream output("./temp/" + to_string(f_num), ios::binary);

        for (auto value = cur_values.begin(); value < cur_values.end(); value++)
        {
            if (!(*value < INT32_MAX))
                break;
            output.write((char *)&(*value), sizeof(*value));
        }

        output.close();
    }
}
