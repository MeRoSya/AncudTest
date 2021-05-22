#include <iostream>
#include <fstream>
#include <getopt.h>

using namespace std;

int main(int argc, char *argv[])
{

    /*Creating variable, containing path to file
    and intialize it with default value*/

    string path = "Sorted_Array";

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

    /*Variable, containing
    previous value, read from file*/

    unsigned int comp = 0;
    ifstream input(path, ios::binary);

    /*Checking, if file is open*/
    if (input.good())
    {
        /*Read values from file*/
        while (input.peek() != EOF)
        {
            unsigned int value;
            /*Here can be errors, while reading from file (broken file, for example), so
            it must be handled*/
            try
            {
                input.read((char *)&value, sizeof(value));

                /*Checking if current value is lesser than previous value.
                if it is so, array isn't sorted and we can finish programm.*/

                if (value < comp)
                {

                    cout << "Array is not sorted" << endl;
                    return 1;
                }

                comp = value;
            }
            catch (...)
            {
                cout << "Error occured while reading the file" << endl;
                return 1;
            }
        }
        cout << "Array is sorted" << endl;
    }
    else
        cout << "Error, while trying to open the file" << endl;
    return 0;
}