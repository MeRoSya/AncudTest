#include <fcntl.h>
#include <boost/filesystem.hpp>
#include <sys/mman.h>

#include <fstream>
#include <thread>
#include <algorithm>

#include "Parsing.hpp"

#define MIN_PART_SIZE pow(2, 21) / sizeof(unsigned int)

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
    part_size - the size of the part to handle in a one thread
    */
    static void Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end, unsigned long long int part_size);

protected:
    /*The pointer to the begin of the information in the file*/
    unsigned int *Array;

    /*The size of the file (the number of elements)*/
    unsigned long long int size;

    /*The file descriptor*/
    int fd;

    /*The path to the input file*/
    std::string input_path;

    /*The path to the output file*/
    std::string output_path;

public:
    /*Constructor*/
    MappedFile(std::string input_path = "./Array", std::string output_path = "./Sorted_Array");

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
    void Read(std::string path);

    /*Destructor*/
    virtual ~MappedFile();
};

int main(int argc, char *argv[])
{
    /*
    Creating variables, which contain paths to the files
    and initializing them with default values
    */

    std::string input_path = "./Array";
    std::string output_path = "./Sorted_Array";

    /*Cmd arguments handling*/
    ParallelSortPars ps(argc, argv);
    ps.Parse(input_path, output_path);

    /*Sorting*/
    std::cout << "Starting processes..." << std::endl;
    MappedFile file(input_path, output_path);
    file.Sort();
    return 0;
}

/*Implementation of the methods of the class*/

MappedFile::MappedFile(std::string input_path, std::string output_path)
{
    this->input_path = input_path;
    this->output_path = output_path;
    try
    {
        this->size = boost::filesystem::file_size(this->input_path) / sizeof(unsigned int);
    }
    catch (boost::filesystem::filesystem_error ex)
    {
        std::cout << "An error occurred: " << ex.what() << std::endl;
        exit(1);
    }
    this->Array = nullptr;
}

void MappedFile::Read(std::string path)
{

    if ((fd = open(path.c_str(), O_RDWR)))
    {
        std::cout << "Reading the file " << path << std::endl;
        Array = (unsigned int *)mmap(0, size * sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        madvise(Array, size, POSIX_MADV_WILLNEED | POSIX_MADV_SEQUENTIAL);
    }
    else
    {
        std::cout << "An error occurred, while trying to read the file" << std::endl;
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
    {
        try
        {
            /*Copy the input file to the output file*/
            std::cout << "Reading the file " << input_path << std::endl;
            std::cout << "Writing to the file " << output_path << std::endl;
            boost::filesystem::copy(input_path, output_path, boost::filesystem::copy_options::overwrite_existing);
        }
        catch (boost::filesystem::filesystem_error ex)
        {
            std::cout << "The error occurred, while trying to write the file: " << ex.what();
            exit(1);
        }
    }
    else
    {
        std::cout << "The error occurred: Not enough disk space to create the file's copy" << std::endl;
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
    {
        this->Read();
    }

    /*Sorting the file*/
    std::cout << "Sorting the file " << output_path << std::endl;
    Rec_Threaded_Sort(std::ref(Array), 0, size, this->size / std::thread::hardware_concurrency());
    std::cout << "Done" << std::endl;
}

void MappedFile::Rec_Threaded_Sort(unsigned int *&ptr, int begin, int end, unsigned long long int part_size)
{

    /*
    end-begin - part size
    Optimal MIN_PART_SIZE is 2 Mb (read README.md for more information)
    */
    if ((end - begin > MIN_PART_SIZE) && !(end - begin < 2 * part_size))
    {

        std::thread t1(Rec_Threaded_Sort, std::ref(ptr), begin, (begin + end) / 2, part_size);
        Rec_Threaded_Sort(std::ref(ptr), (begin + end) / 2, end, part_size);

        t1.join();

        /*
        Following the description of std::inplace_merge() it uses additional RAM ONLY,
        if it is possible. So there can't be the situation, when the program is using more memory,
        than is available
        */
        std::inplace_merge(ptr + begin, ptr + (begin + end) / 2, ptr + end);
    }
    else
    {
        std::sort(ptr + begin, ptr + end);
    }
}

MappedFile::~MappedFile()
{
    int fc = close(fd);
}
