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

/*Interface to use in all File-classes*/
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

    static void Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end);

protected:
    unsigned int *Array;
    unsigned long long int size;
    string input_path;
    string output_path;

public:
    /*Constructor*/
    MappedFile(string input_path = "./Array", string output_path = "./Sorted_Array");

    /*Implementing interface's methods*/
    virtual void Read();
    void Sort();
    virtual void Write();

    /*Additional method to implement reading of the any file, not only input*/
    void Read(string path);

    /*Destructor*/
    virtual ~MappedFile();
};

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
    MappedFile file(input_path, output_path);
    file.Sort();
    return 0;
}

/*Implementation of class methods*/

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
        cout << "Error occurred: " << ex.what();
        exit(1);
    }
    this->Array = nullptr;
}

void MappedFile::Read(string path)
{
    int fd;
    if ((fd = open(path.c_str(), O_RDWR)))
    {
        cout << "Reading the file " << path << endl;
        Array = (unsigned int *)mmap(0, size * sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        madvise(Array, size, POSIX_MADV_WILLNEED | POSIX_MADV_SEQUENTIAL);
    }
    else
    {
        cout << "Error occurred, while trying to read the file" << endl;
        exit(1);
    }
}

void MappedFile::Read()
{
    Read(input_path);
}

void MappedFile::Write()
{
    if (boost::filesystem::space(boost::filesystem::current_path()).available > size * sizeof(unsigned int))
        try
        {
            cout << "Writing to the file " << output_path << endl;
            boost::filesystem::copy(input_path, output_path, boost::filesystem::copy_options::overwrite_existing);
        }
        catch (boost::filesystem::filesystem_error ex)
        {
            cout << "Error occurred, while trying to write the file: " << ex.what();
            exit(1);
        }
    else
    {
        cout << "Error occurred: Not enough disk space to create the file's copy" << endl;
        exit(1);
    }
}

void MappedFile::Sort()
{

    if (input_path != output_path)
    {
        this->Write();
        this->Read(output_path);
    }
    else
        this->Read();
    cout << "Sorting the file " << output_path << endl;
    Rec_Threaded_Sort(ref(Array), 0, size);
}

void MappedFile::Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end)
{
    static atomic_int t_num(0);
    t_num++;
    if (!(t_num > thread::hardware_concurrency() / 2) && (end > pow(2, 30) - 1))
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

MappedFile::~MappedFile()
{
}
