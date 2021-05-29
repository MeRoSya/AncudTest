#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <sys/mman.h>
#include <fcntl.h>
#include <getopt.h>
#include <cmath>

using namespace std;

/*The interface to use in all File-sort classes*/
class IFile
{
public:
    virtual void Read() = 0;
    virtual void Sort() = 0;
    virtual void Write() = 0;
};

/*Class which is used to work with files using file-mapping*/
class MappedFile : public IFile
{

    /*
    Function, which sorts array of values using modified merge-sort.
    Args:
    ptr - Array
    begin - Start point of sorting
    end - End point of sorting
    */
    static void Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end);

protected:

    /*The pointer to the begin of the information in the file*/
    unsigned int *Array;

    /*The size of the file (the number of elements)*/
    unsigned long long int size;

    /*The file descriptor*/
    int fd;

    /*The path to the input file*/
    string input_path;

    /*The path to the output file*/
    string output_path;

public:
    /*Constructor*/
    MappedFile(string input_path = "./Array", string output_path = "./Sorted_Array");

    /*Implementing interface's methods*/

    /*Reads the input file*/
    virtual void Read();

    /*Sorts data in the output file*/
    void Sort();

    /*
    Writes data to the output file
    (copies the input file)
    */
    virtual void Write();
    
    /*
    Additional method to implement reading of any file, not only the input one.
    Sets the pointer Array to point on the begin of the info in the file path.
    */
    void Read(string path);

    /*Destructor*/
    virtual ~MappedFile();
};

int main(int argc, char *argv[])
{
    /*
    Creating variables, which contain paths to the files
    and initializing them with default values
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
            cout << "-i [ --ifile ] arg\tA path to the input file" << endl;
            cout << "if -i flag isn't used, program will try to use default file ./Array" << endl;
            cout << endl;
            cout << "-o [ --ofile ] arg\tA path to the output file" << endl;
            cout << "if -o flag isn't used, program will try to use default file ./Sorted_Array" << endl;
            return 0;
            break;

        /*Setting the path to the input file*/
        case 'i':
            input_path = optarg;
            if (!boost::filesystem::exists(input_path))
            {
                cout << "The input file doesn't exist" << endl;
                exit(1);
            }
            break;

        /*Setting the path to the output file*/
        case 'o':
            output_path = optarg;
            break;

        case '?':
            /*Error is already printed by getopt_long*/
            return 1;
            break;

        default:
            break;
        }
    }

    cout << "Starting processes..." << endl;
    MappedFile file(input_path, output_path);
    file.Sort();
    return 0;
}

/*Implementation of the methods of the class*/

MappedFile::MappedFile(string input_path, string output_path)
{
    this->input_path = input_path;
    this->output_path = output_path;
    try
    {
        this->size = boost::filesystem::file_size(this->input_path) / sizeof(unsigned int);
    }
    catch (boost::filesystem::filesystem_error ex)
    {
        cout << "An error occurred: " << ex.what() << endl;
        exit(1);
    }
    this->Array = nullptr;
}

void MappedFile::Read(string path)
{
    
    if ((fd = open(path.c_str(), O_RDWR)))
    {
        cout << "Reading the file " << path << endl;
        Array = (unsigned int *)mmap(0, size * sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        madvise(Array, size, POSIX_MADV_WILLNEED | POSIX_MADV_SEQUENTIAL);
    }
    else
    {
        cout << "An error occurred, while trying to read the file" << endl;
        exit(1);
    }
}

void MappedFile::Read()
{
    Read(input_path);
}

void MappedFile::Write()
{
    /*Checking, if there is enough disk space available to create a copy of the initial file*/
    if (boost::filesystem::space(boost::filesystem::current_path()).available > size * sizeof(unsigned int))
        try
        {
            /*Copy the input file to the output file*/
            cout << "Reading the file " << input_path << endl;
            cout << "Writing to the file " << output_path << endl;
            boost::filesystem::copy(input_path, output_path, boost::filesystem::copy_options::overwrite_existing);
        }
        catch (boost::filesystem::filesystem_error ex)
        {
            cout << "The error occurred, while trying to write the file: " << ex.what();
            exit(1);
        }
    else
    {
        cout << "The error occurred: Not enough disk space to create the file's copy" << endl;
        exit(1);
    }
}

void MappedFile::Sort()
{

    /*
    Sets the output file and copy data from the input
    file to it. If the input and  the output are the same files
    only reads the input
    */
    if (input_path != output_path)
    {
        this->Write();
        this->Read(output_path);
    }
    else
        this->Read();
    
    /*Sorting the file*/
    cout << "Sorting the file " << output_path << endl;
    Rec_Threaded_Sort(ref(Array), 0, size);
    cout << "Done" << endl;
}

void MappedFile::Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end)
{
    /*Threads counter*/
    static atomic_int t_num(0);
    t_num++;

    /*
    end-begin - part size
    Optimal minimal part size is 2 Mb (read README.md for more information)
    */
    if (!(t_num > thread::hardware_concurrency()) && !(end-begin < pow(2, 21)/sizeof(unsigned int)))
    {
        thread t(Rec_Threaded_Sort, ref(ptr), begin, end / 2); 
        Rec_Threaded_Sort(ref(ptr), end / 2, end);
        t.join();

        /*
        Following the description of std::inplace_merge() it uses additional RAM ONLY,
        if it is possible. So there can't be the situation, when it is used more memory,
        than is available
        */
        inplace_merge(ptr + begin, ptr + end / 2, ptr + end);
    }
    else
    {
        sort(ptr + begin, ptr + end);
    }
}

MappedFile::~MappedFile()
{
    int fc = close(fd);
}
