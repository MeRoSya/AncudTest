# AncudTest

Test task

## Concept

The program sorts an array of unsigned ints from a binary file and writes it to another file

## Usage

*"ParallelSort"* performs sorting of an array. Default file to sort: *"./Array"*. Default file to output: *"./Sorted_Array"*


Command line options:
* -h [ --help ] Shows help
* -o [ --ofile ] arg A path to the output file
* -i [ --ifile ] arg A path to the input file

*"FileGenerator"* generates a file of the needed size. Default size: 1 KB

Command line options:
* -h [ --help ] Shows help
* -s [ --size ] arg Size of generating file            
* -p [ --prefix ] arg Prefix

Prefixes:<br>
"1" - kiloBytes<br>
"2" - MegaBytes<br>
"3" - GigaBytes<br>
"4" - TeraBytes<br>
"5" - PetaBytes<br>

*"SortCheck"* checks if numbers in the file are sorted. Default file to check: file *"./Sorted_Array"*

Command line options:
* -h [ --help ] Shows help
* -f [ --file ] Arg sets path to file

Using CTest in CMake to test

## Algorithm info
The program uses modified merge sort and *file mapping*.<br> 
So the max size of the file, which is to sort, is limited only by the number of possible addresses (2^32 for x32 systems and 2^64 for x64 systems).<br>
So the biggest possible file to sort on x64 machine is *4 exabytes* and on x32 machine - *2 Gb*. <br>

 Sorting time (Tested on 8 core machine):
    
    1 Gb:
    Sequential (using only std::sort()): 26094 ms
    Threaded (using MappedFile::Rec_Threaded_Sort()): 13325 ms

    2 Gb:
    Sequential (using only std::sort()): 58549 ms
    Threaded (using MappedFile::Rec_Threaded_Sort()): 30112 ms

    4 Gb:
    Sequential (using only std::sort()): 195201 ms
    Threaded (using MappedFile::Rec_Threaded_Sort()): 194984 ms

  The best part size - the size, which is big enough and its time of sequential sorting is lesser than threaded or equal to it:

    1 Mb:
    Sequential (using only std::sort()): 17 ms
    Threaded (using MappedFile::Rec_Threaded_Sort()): 18 ms

    2 Mb:
    Sequential (using only std::sort()): 45 ms
    Threaded (using MappedFile::Rec_Threaded_Sort()): 56 ms

    4 Mb:
    Sequential (using only std::sort()): 71 ms
    Threaded (using MappedFile::Rec_Threaded_Sort()): 44 ms

    As you can see, the optimal minimal size of the part is 2 Mb
    
 ## Updating possibilities
 There is the interface IFile, which can be used to create other implementations of file sorting
