#include <fstream>

#include "Parsing.hpp"

int main(int argc, char *argv[])
{
    srand(time(0));
    unsigned int kilosize = 1024;
    unsigned long long int size = 1;
    std::string file = "./Array";

    /*Cmd args handling*/
    FileGeneratorPars ps(argc, argv);
    ps.Parse(kilosize, size);

    size *= kilosize;

    /*Calculating number of numbers, needed to 
    file size equal to size*/
    size /= sizeof(unsigned int);

    /*Output to binary file*/
    struct stat checkup;

    /*Checking, if file exists*/
    if (stat(file.c_str(), &checkup) == 0)
    {
        std::cout << "File \"" << file << "\" already exists. Override it?" << std::endl;
        std::cout << "Y - yes, N - no: " << std::endl;
        char ov;
        std::cin >> ov;
        switch (ov)
        {
        case 'Y':
            remove(file.c_str());
            break;

        case 'N':
            std::cout << "Input another filename to generate: ";
            std::cin >> file;
            break;

        default:
            std::cout << "Invalid input" << std::endl;
            return 1;
            break;
        }
    }

    std::ofstream output(file, std::ios::binary);

    if (output.is_open())
    {
        for (int i = 0; i < size; i++)
        {

            /*Generating a random value*/
            unsigned int value = rand();

            output.write((char *)&value, sizeof(value));
        }
        output.close();
    }
    else
    {
        std::cout << "The error occurred, while trying to write the file" << std::endl;
        return 2;
    }

    return 0;
}
