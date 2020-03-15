# SPS-SABRE Data Analysis Package
This is a software package designed to help experimenters analyze data from SPS-SABRE at FSU. 
It contains four programs: `binary2root`, `analyzer`, `merger`, and `cleaner`. Each of these programs represents astep in the analysis pipeline, and should be run in this sequence:
1. `binary2root` (optional, only required if original data is in binary format)
1. `analyzer`
2. `merger` (optional, and usually not recommended)
3. `cleaner`
Programs are built using make, and a make file is included. Simply using the command make will
build all four programs. This program is built using libraries from `ROOT` 6.14. Additionally, the included `binary2root` relies
on the `libarchive` library to read in archived data. If you do not have `libarchive`, you will need to install it.
Alternatively, just dont make and use the binary converter.

## Binary2ROOT
The `binary2root` program takes binary CoMPASS data and converts it into a `ROOT` file with a tree. Currently this program can
accept either the plain .bin format binary CoMPASS data or a compressed archive of .bin files in a .tar.gz format. In principle,
the types of accepted archive formats can be expanded, as the archives and compression is all handled by the libarchive library,
but as .tar.gz is fairly universal this seems unnecessary. The full path to data is specified by `binary2root_input`.txt input file.

## Analyzer
The `analyzer` is the bulk of the analysis process. Analyzer takes each individual run file from
compass, and performs three stages of analysis on each file. The first stage is shifting all 
of the timestamps for both the focal plane scintillator and SABRE silicons so that they come 
later than the focal plane ion-chamber. The shifted data is then written to an output file.
The second stage is sorting the shifted data by timestamp and orgainizing detector hits into 
data events. This is done through the use of TTree::BuildIndex which will sort a specific 
branch value (in this case timestamps). The data is then collected in events; events are 
structures which contain all detector hits that occur within a given coincidence window. This
event data is then written to an output file. Note that at this stage some data is dumped; if
there is an incomplete set of focal plane data, there is no point on carrying that event 
to the next stage of analysis. This helps to reduce file sizes and computation times, however
there are use cases were this may not be desirable. Finally, the sorted event data is then 
converted into meaningful physical data, and saved to a final analyzed file. In this way, each
raw data file gives three output files from the analysis: a shifted file, a sorted file, and 
an analyzed file. The rationale behind the repetative writting is that it helps the user 
isolate at which stage data issues occur at; this is especially useful for the shifting and 
sorting stages, where the values for the shifts and coincidence window have to be estimated by
the user before running. 

All of the user input is handled through an input file in the program directory named 
`analyzer_input.txt`. This file is preformated; all the user needs to do is update the names and
values. Everything from input and output directories, to shifts and coincidence windows should
be specified in this file. Note that directorires should be explicit fullpaths.

## Merger
The `merger` is a small program with the intent of merging multiple analyzed files together into
a single file. It is not recommended to use this program in general, but it can be useful for
storing data if changes are to be made to the `analyzer` process, which generally overwrites
previous files. It also uses an input file, `merger_input.txt`, to define outputs and inputs.

## Cleaner
The `cleaner` is intended to be the final leg of the analysis pipeline. The goal of this program
is to take a collection of analyzed files and produce a file containing relevant histograms,
graphs, and other such data measures. As it is currently built, this program has no ability to
save any data of its own, it merely makes data measures. Data writing could be added, but this
is kind of against the principle of this part of the program. It would most likely be best to
add any further data writing to the `merger` program instead. The advantage of keeping the data
in individual analysis files is that CoMPASS timestamps are not global to the project they
are part of. That is, each individual start and stop cycle contains its own individual time 
block, and these blocks currently cannot be merged together. So any time analysis would be
useless in a bulk file. Additionally, `ROOT` merging methods are generally slow and expensive,
while reading in multiple trees from multiple files is generally easy and fast. Like the other
programs, `cleaner` takes an input file (`cleaner_input.txt`) which specifies analyzed data 
directory, output file, etc. These root files can then be combined into another file by using the
command `hadd`, which is included in the `ROOT` distrobution.

## System Requirements
Only tested with `ROOT` 6.14, so compatibility with any other version is questionable
Uses C++11 standards

## Compliling and Running
To compile use the command `make`
To run the code in a standard pipeline use the process shell script. This executes in this order:
```
./bin/binary2root binary_input.txt
./bin/analyzer analyzer_input.txt
./bin/cleaner cleaner_input.txt
```

