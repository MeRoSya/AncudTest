# UncudTest

Test tasks

## Concept

Program sorting an array of unsigned int from binary file and write it to another file

## Usage

*"ParallelSort"* performs sorting of array. Default file to sort: file *"Array"* from the same directory.

Command line options:
* -h [ --help ] shows help
* -f [ --file ] arg sets path to file

*"FileGenerator"* generates file of needed size. Default size: 1 KB

Command line options:
* -h [ --help ] shows help
* -s [ --size ] arg Input size of generating file            
* -p [ --prefix ] arg Prefix

Prefixes:<br>
"1" - kiloBytes<br>
"2" - MegaBytes<br>
"3" - GigaBytes<br>
"4" - TeraBytes<br>
"5" - PetaBytes<br>

*"SortCheck"* checks if numbers in file are sorted. Default file to check: file *"Sorted_Array"* from the same directory

Command line options:
* -h [ --help ] shows help
* -f [ --file ] arg sets path to file

Using CTest in CMake to test
