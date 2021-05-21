#include <iostream>
#include <getopt.h>

using namespace std;

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
            cout << "if -f flag isn't used, program will try to use default file ./array" << endl;
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
    return 0;
}