#include <getopt.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <cmath>

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
        {
            break;
        }

        switch (cur_opt)
        {

        /*Help information*/
        case 'h':
            std::cout << "Options list:" << std::endl;
            std::cout << std::endl;
            std::cout << "-h [ --help ]\t\tShows help" << std::endl;
            std::cout << "-s [ --size ] arg\tInput size of generating file" << std::endl;
            std::cout << std::endl;
            std::cout << "if -s flag isn't used, program will try to use default size: 1" << std::endl;
            std::cout << std::endl;
            std::cout << "-p [ --prefix ] arg\tPrefix" << std::endl;
            std::cout << std::endl;
            std::cout << "Prefixes" << std::endl;
            std::cout << "\"1\" - kiloBytes" << std::endl;
            std::cout << "\"2\" - MegaBytes" << std::endl;
            std::cout << "\"3\" - GigaBytes" << std::endl;
            std::cout << "\"4\" - TeraBytes" << std::endl;
            std::cout << "\"5\" - PetaBytes" << std::endl;
            std::cout << "etc..." << std::endl;
            std::cout << std::endl;
            std::cout << "if -p flag isn't used, program will try to use default prefix kilo-" << std::endl;
            return 0;
            break;

        /*Checking --size flag*/
        case 's':
            try
            {
                std::string sizestr(optarg);
                /*Not a number argument for a size makes no sense, so it must be handled as an error*/
                if (std::all_of(sizestr.begin(),sizestr.end(), [] (char i){return isdigit(i);}))
                {
                    unsigned int n = atoi(optarg);

                    /*
                    A size with a negative value makes no sense, so it must 
                    be handled as error. But it is already checked in the Is_Number() function, 
                    so it is unnecessary to check it here. Also it is pointless for a size
                    to be equal 0. So that case must be handled too
                    */
                    if (n == 0)
                    {
                        throw(std::invalid_argument("The size cannot be equal 0"));
                    }
                    else
                    {
                        size = n;
                    }
                }
                else
                {
                    throw std::invalid_argument("Invalid size");
                }
            }
            catch (std::invalid_argument ex)
            {
                std::cout << ex.what() << std::endl;
                std::cout << "Default size will be used" << std::endl;
            }
            break;

        /*Checking --prefix flag*/
        case 'p':
            try
            {
                /*Not a number argument for a prefix makes no sense, so it must be handled as an error*/
                std::string prefix(optarg);
                if (std::all_of(prefix.begin(),prefix.end(), [] (char i){return isdigit(i);}))
                {
                    unsigned int n = atoi(optarg);

                    /*Prefix with value not in [1;5] makes no sense, so it must be handled as error*/
                    if ((n < 1) || (n > 5))
                    {
                        throw std::invalid_argument("Invalid prefix value: is too big, or too small.");
                        n = 1;
                    }
                    kilosize = pow(kilosize, n);
                }
                else
                {
                    throw std::invalid_argument("Invalid prefix type");
                }
            }
            catch (std::invalid_argument ex)
            {
                std::cout << ex.what() << std::endl;
                std::cout << "Default prefix will be used" << std::endl;
            }
            break;

        case '?':
            /*Error is already printed by getopt_long*/
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
    std::ofstream output("Array", std::ios::binary);

    for (int i = 0; i < size; i++)
    {

        /*Generating a random value*/
        unsigned int value = rand();

        output.write((char *)&value, sizeof(value));
    }

    output.close();

    return 0;
}
