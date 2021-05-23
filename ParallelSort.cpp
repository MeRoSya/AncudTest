#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cmath>
#include <sys/stat.h>
#include <getopt.h>

const unsigned long long int default_mem_amount = pow(2,28);

using namespace std;

/*USE MERGE SORT AND TEMP FILES TO SORT FILE CONTENT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

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

void Threaded_File_Sort(string path);

void File_Sort(ifstream &input, int begin, int end);

void File_Split(string path);

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

    Threaded_File_Sort(path);

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

void Threaded_File_Sort(string path)
{
    /*Finding out numbers of elements in the file*/
    ifstream input(path, ios::ate);
    unsigned long long int size = input.tellg()/sizeof(unsigned int);
    input.close();

    unsigned int t_num = sysconf(_SC_NPROCESSORS_ONLN);
    thread threads[t_num];
    int nodes [t_num+1];
    nodes[0]=0;
    File_Split(path);

    /*input.open("Sorted_Array", ios::binary);

    for (int i=0; i<t_num; i++)
    {
        nodes[i+1]=(i+1)*size/t_num;
        threads[i] = thread(File_Sort, ref(input), nodes[i], nodes[i+1]);
    }

    for (int i=0; i<t_num; i++)
    {
        if (threads[i].joinable())
            threads[i].join();
    }*/
    
}

void File_Sort(ifstream &input, int begin, int end)
{
    for (int i=begin; i<end; i++)
    {
        input.seekg(i);
        int cur_value;
        input.read((char*)&cur_value, sizeof(cur_value));
        for (int j=i; j>0; j--)
        {
            input.seekg(j);
            int cur_position;
            int next_position;
            input.read((char*)&cur_position, sizeof(cur_position));
            if (cur_value>cur_position)
                break;
            else
            {
                input.seekg(j-1);
                input.read((char*)&next_position, sizeof(next_position));
            }
            
        }
    }
}

/*splits the file correctly, but when it goes for array less than 4 kb, threre are extra zeros*/
void File_Split(string path)
{
    ifstream input(path, ios::binary);
    mkdir("temp",S_IRWXU);
    int f_num = 0;
    vector<unsigned int> cur_values(1024);
    //ifstream check("./temp/"+to_string(i), ios::ate);
    while (input.peek()!=EOF)
    {
        input.read((char*)&cur_values[0], cur_values.size()*sizeof(cur_values[0]));
        sort(cur_values.begin(),cur_values.end());
        ofstream output("./temp/"+to_string(f_num), ios::binary | ios_base::app);
        output.write((char*)&cur_values[0], cur_values.size()*sizeof(cur_values[0]));
        output.close();
        f_num++;
    }
    input.close();
}

