#include <getopt.h>

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{

    /*Creating a variable, containing the path to the file
    and initializing it with a default value*/

    std::string path = "Sorted_Array";

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
            std::cout << "Options list:" << std::endl;
            std::cout << "-h [ --help ]\t\tShows help" << std::endl;
            std::cout << "-f [ --file ] arg\tInput path to file with data" << std::endl;
            std::cout << "if -f flag isn't used, program will try to use default file ./Array" << std::endl;
            return 0;
            break;

        /*Setting the path to file*/
        case 'f':
            path = optarg;
            break;

        case '?':
            /*Error is already printed by getopt_long*/
            return 1;
            break;

        default:
            break;
        }
    }

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
        std::cout << "An error occurred, while trying to open the file" << std::endl;
    return 0;
}