#include <fstream>

#include "Parsing.hpp"

int main(int argc, char *argv[])
{

    /*Creating a variable, containing the path to the file
    and initializing it with a default value*/

    std::string path = "Sorted_Array";

    /*Cmd arguments handling*/
    SortCheckPars ps(argc, argv);
    ps.Parse(path);

    /*A variable, which contains the
    previous value, read from file*/

    unsigned int comp = 0;
    std::ifstream input(path, std::ios::binary);

    /*Checking, if the file is open*/
    if (input.good())
    {
        /*Read values from the file*/
        while (input.peek() != EOF)
        {
            unsigned int value;

            /*Here can be errors while reading from the file (broken file, for example), so
            they must be handled*/
            try
            {
                input.read((char *)&value, sizeof(value));

                /*Checking if the current value is lesser than the previous value.
                if it is so, the array isn't sorted and we can finish the program.*/
                if (value < comp)
                {

                    std::cout << "The array is not sorted" << std::endl;
                    return 1;
                }

                comp = value;
            }
            catch (...)
            {
                std::cout << "An error occurred while reading the file" << std::endl;
                return 1;
            }
        }
        std::cout << "The array is sorted" << std::endl;
    }
    else
    {
        std::cout << "An error occurred, while trying to open the file" << std::endl;
    }
    return 0;
}