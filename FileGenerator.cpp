#include <iostream>
#include <fstream>
#include <getopt.h>
#include <string.h>
#include <cmath>

using namespace std;

/*Functions' prototypes*/

/*
Function, checking c-type string to be number
Args:
value - c-type string to be checked
*/
bool Is_Number(char *value);

int main(int argc, char *argv[])
{
    srand(time(0));
    unsigned int kilosize = 1024;
    unsigned long long int size = 1;

    /*Cmd args handling*/
    while (true)
    {

        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"size", required_argument, 0, 's'}};
        int option_index = 0;

        int cur_opt = getopt_long(argc, argv, "hs:p:", long_options, &option_index);

        if (cur_opt == -1)
            break;

        switch (cur_opt)
        {

        /*Help information*/
        case 'h':
            cout << "Options list:" << endl;
            cout << endl;
            cout << "-h [ --help ]\t\tShows help" << endl;
            cout << "-s [ --size ] arg\tInput size of generating file" << endl;
            cout << endl;
            cout << "if -s flag isn't used, program will try to use default size: 1" << endl;
            cout << endl;
            cout << "-p [ --prefix ] arg\tPrefix" << endl;
            cout << endl;
            cout << "Prefixes" << endl;
            cout << "\"1\" - kiloBytes" << endl;
            cout << "\"2\" - MegaBytes" << endl;
            cout << "\"3\" - GigaBytes" << endl;
            cout << "\"4\" - TeraBytes" << endl;
            cout << "\"5\" - PetaBytes" << endl;
            cout << "etc..." << endl;
            cout << endl;
            cout << "if -p flag isn't used, program will try to use default prefix kilo-" << endl;
            return 0;
            break;

        /*Checking --size flag*/
        case 's':
            try
            {
                /*Not number argument for size makes no sence, so it must be handled as error*/
                if (Is_Number(optarg))
                {
                    unsigned int n = atoi(optarg);

                    /*
                    Size with negative value makes no sence, so it must 
                    be handled as error. But it is already checked in Is_Number function, 
                    so it is unneseserry to check it here. Also it is pointless for size
                    to be equal 0. So that case must be handled too
                    */
                    if (n == 0)
                        throw(invalid_argument("Size cannot be 0"));
                    else
                        size = n;
                }
                else
                    throw invalid_argument("Invalid size");
            }
            catch (invalid_argument ex)
            {
                cout << ex.what() << endl;
                cout << "Default size will be used" << endl;
            }
            break;

        /*Checking --prefix flag*/
        case 'p':
            try
            {
                /*Not number argument for prefix makes no sence, so it must be handled as error*/
                if (Is_Number(optarg))
                {
                    unsigned int n = atoi(optarg);

                    /*Prefix with value not in [1;5] makes no sence, so it must be handled as error*/
                    if ((n < 1) || (n > 5))
                    {
                        throw invalid_argument("Invalid prefix value: is too big, or too small.");
                        n = 1;
                    }
                    kilosize = pow(kilosize, n);
                }
                else
                    throw invalid_argument("Invalid prefix type");
            }
            catch (invalid_argument ex)
            {
                cout << ex.what() << endl;
                cout << "Default prefix will be used" << endl;
            }
            break;

        case '?':
            /*Error already printed by getopt_long*/
            return 1;
            break;

        default:
            break;
        }
    }

    size *= kilosize;

    /*Calculating number of numbers, needed to 
    file size equal to size*/
    size /= sizeof(unsigned int);

    /*Output to binary file*/
    remove("Array");
    ofstream output("Array", ios::binary);

    for (int i = 0; i < size; i++)
    {

        /*Generating random value*/
        unsigned int value = rand() % (INT32_MAX - 1);

        output.write((char *)&value, sizeof(value));
    }

    output.close();

    return 0;
}

/*Implementation of functions*/

bool Is_Number(char *value)
{
    string str(value);
    for (auto item = str.begin(); item < str.end(); item++)
    {
        /*Checking current symbol to be digit*/
        if (!isdigit(*item))
        {
            return false;
        }
    }
    return true;
}