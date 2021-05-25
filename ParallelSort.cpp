#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <sys/mman.h>
#include <fcntl.h>
#include <getopt.h>
#include <cmath>
#include <chrono>

using namespace std;

/*Functions' prototypes*/

void File_Sort(string input_path, string output_path);

void Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end);

int main(int argc, char *argv[])
{
    /*
    Creating variables, which contains paths to files
    and initialize them with default value
    */

    string input_path = "./Array";
    string output_path = "./Sorted_Array";

    /*Cmd arguments handling*/
    while (true)
    {

        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"ifile", required_argument, 0, 'i'},
                                               {"ofile", required_argument, 0, 'o'}};
        int option_index = 0;

        int cur_opt = getopt_long(argc, argv, "hi:o:", long_options, &option_index);

        if (cur_opt == -1)
            break;

        switch (cur_opt)
        {

        /*Help information*/
        case 'h':
            cout << "Options list:" << endl;
            cout << "-h [ --help ]\t\tShows help" << endl;
            cout << "-if [ --ifile ] arg\tA path to the input file" << endl;
            cout << "if -if flag isn't used, program will try to use default file ./Array" << endl;
            cout << endl;
            cout << "-of [ --ofile ] arg\tA path to the output file" << endl;
            cout << "if -of flag isn't used, program will try to use default file ./Sorted_Array" << endl;
            return 0;
            break;

        /*Setting path to input file*/
        case 'i':
            input_path = optarg;
            if (!boost::filesystem::exists(input_path))
            {
                cout << "Input file doesn't exist" << endl;
                exit(1);
            }
            break;

        /*Setting path to input file*/
        case 'o':
            output_path = optarg;
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
    File_Sort(input_path, output_path);
    return 0;
}

/*Implementation of functions*/

void File_Sort(string input_path, string output_path)
{
    try
    {
        boost::filesystem::copy(input_path, output_path, boost::filesystem::copy_options::overwrite_existing);
    }
    catch(boost::filesystem::filesystem_error ex) 
    {
        cout << "Error occurred: " << ex.what();
        exit(1);
    }
    long double number_of_bytes = boost::filesystem::file_size(output_path);
    unsigned long long int sizes = number_of_bytes / sizeof(unsigned int);
    long double max_read_size = pow(2, sizeof(void *) * 8);
    int number_of_parts;
    if (number_of_bytes > max_read_size)
        number_of_parts = max_read_size / number_of_bytes;
    else
        number_of_parts = 1;
    int fd;
    if ((fd = open(output_path.c_str(), O_RDWR)))
    {
        for (int i = 0; i < number_of_parts; i++)
        {
            cout << "Reading file..." << endl;
            unsigned int *ptr = (unsigned int *)mmap(0, number_of_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            cout << "Sorting..." << endl;
            Rec_Threaded_Sort(ref(ptr), 0, sizes);
        }
    }
    else
    {
        cout << "Error occurred, while trying to sort the file" << endl;
        exit(1);
    }
}

void Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end)
{
    static atomic_int t_num(0);
    t_num++;
    if (!(t_num > thread::hardware_concurrency() / 2) && (end > pow(2,30)-1))
    {
        thread t(Rec_Threaded_Sort, ref(ptr), 0, end / 2);
        t.join();
        Rec_Threaded_Sort(ref(ptr), end / 2, end);
        inplace_merge(ptr + begin, ptr + end / 2, ptr + end);
    }
    else
    {
        sort(ptr + begin, ptr + end);
    }
}
