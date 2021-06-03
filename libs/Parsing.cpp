#include "Parsing.hpp"

/*Base class implementation*/
Parsing::Parsing(int &argc, char **&argv)
{
    this->argc = argc;
    this->argv = argv;
}

void Parsing::Parse()
{
    while (true)
    {

        static struct option long_options[] = {{"help", no_argument, 0, 'h'}};
        int option_index = 0;

        int cur_opt = getopt_long(argc, argv, "h", long_options, &option_index);

        if (cur_opt == -1)
        {
            break;
        }

        switch (cur_opt)
        {

        /*Help information*/
        case 'h':
            std::cout << "Options list:" << std::endl;
            std::cout << "-h [ --help ]\t\tShows help" << std::endl;
            exit(0);
            break;

        default:
            break;
        }
    }
}

Parsing::~Parsing()
{
}

/*ParallelSortPars class implementation*/
ParallelSortPars::ParallelSortPars(int &argc, char **&argv) : Parsing(argc, argv)
{
}

void ParallelSortPars::Parse(std::string &input_path, std::string &output_path)
{
    while (true)
    {

        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"ifile", required_argument, 0, 'i'},
                                               {"ofile", required_argument, 0, 'o'}};
        int option_index = 0;

        int cur_opt = getopt_long(argc, argv, "hi:o:", long_options, &option_index);

        if (cur_opt == -1)
        {
            break;
        }

        switch (cur_opt)
        {

        /*Help information*/
        case 'h':
            std::cout << "Options list:" << std::endl;
            std::cout << "-h [ --help ]\t\tShows help" << std::endl;
            std::cout << "-i [ --ifile ] arg\tA path to the input file" << std::endl;
            std::cout << "if -i flag isn't used, program will try to use default file ./Array" << std::endl;
            std::cout << std::endl;
            std::cout << "-o [ --ofile ] arg\tA path to the output file" << std::endl;
            std::cout << "if -o flag isn't used, program will try to use default file ./Sorted_Array" << std::endl;
            exit(0);
            break;

        /*Setting the path to the input file*/
        case 'i':
            input_path = optarg;

            struct stat checkup;
            if (!(stat(input_path.c_str(), &checkup) == 0))
            {
                std::cout << "The input file doesn't exist" << std::endl;
                exit(1);
            }
            break;

        /*Setting the path to the output file*/
        case 'o':
            output_path = optarg;
            break;

        case '?':
            /*Error is already printed by getopt_long*/
            exit(1);
            break;

        default:
            break;
        }
    }
}

ParallelSortPars::~ParallelSortPars()
{
}

FileGeneratorPars::FileGeneratorPars(int &argc, char **&argv) : Parsing(argc, argv)
{
}

void FileGeneratorPars::Parse(unsigned int &kilosize, unsigned long long int &size)
{
    while (true)
    {

        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"size", required_argument, 0, 's'},
                                               {"prefix", required_argument, 0, 'p'}};
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
            exit(0);
            break;

        /*Checking --size flag*/
        case 's':
            try
            {
                std::string sizestr(optarg);
                /*Not a number argument for a size makes no sense, so it must be handled as an error*/
                if (std::all_of(sizestr.begin(), sizestr.end(), [](char i)
                                { return isdigit(i); }))
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
                if (std::all_of(prefix.begin(), prefix.end(), [](char i)
                                { return isdigit(i); }))
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
            exit(1);
            break;

        default:
            break;
        }
    }
}

FileGeneratorPars::~FileGeneratorPars()
{
}

SortCheckPars::SortCheckPars(int &argc, char **&argv) : Parsing(argc, argv)
{
}

void SortCheckPars::Parse(std::string &path)
{
    while (true)
    {

        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"file", required_argument, 0, 'f'}};
        int option_index = 0;

        int cur_opt = getopt_long(argc, argv, "hf:", long_options, &option_index);

        if (cur_opt == -1)
        {
            break;
        }

        switch (cur_opt)
        {

        /*Help information*/
        case 'h':
            std::cout << "Options list:" << std::endl;
            std::cout << "-h [ --help ]\t\tShows help" << std::endl;
            std::cout << "-f [ --file ] arg\tInput path to file with data" << std::endl;
            std::cout << "if -f flag isn't used, program will try to use default file ./Array" << std::endl;
            exit(0);
            break;

        /*Setting the path to file*/
        case 'f':
            path = optarg;
            break;

        case '?':
            /*Error is already printed by getopt_long*/
            exit(1);
            break;

        default:
            break;
        }
    }
}

SortCheckPars::~SortCheckPars()
{
}