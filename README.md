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

WHEN TESTING, RUN WITH WIDE WINDOWS

## Binary2ROOT
The `binary2root` program takes binary CoMPASS data and converts it into a `ROOT` file with a tree. Currently this program can
accept either the plain .bin format binary CoMPASS data or a compressed archive of .bin files in a .tar.gz format. In principle,
the types of accepted archive formats can be expanded, as the archives and compression is all handled by the libarchive library,
but as .tar.gz is fairly universal this seems unnecessary. The full path to data is specified by `binary2root_input`.txt input file.

## Analyzer
The `analyzer` is the bulk of the analysis process. Analyzer takes each individual run file from
compass, and performs four stages of analysis on each file: shifting, slow sorting, fast sorting, and analyzing. 

### Analyzer options
The analyzer has several program options designed to allow the user to only run pieces of the analysis pipeline. They are as follows:

`-s` -- runs the full analysis pipeline without the fast sorting

`-a` -- runs only the analyzing part of the analysis pipeline. Assumes the existance of a fast sorted file in the directory of specified
by the input file

`-as` or `-sa` -- runs only the analyzing part of the analysis pipeline, assuming that only slow sorted files exist, int the directory
specified by the input file.

### Shifting
The first stage is shifting all of the timestamps for both the focal plane scintillator 
and SABRE silicons so that they come in coincidence with one of the anodes in the ion chamber. 
The shifted data is then written to an output file. In general, this means that one
will need to run the analyzer with "wide open" coincidence windows, look at the histograms of 
relative timings and then apply shifts to a subsequent execution which make it so the relative
timing peaks are centered on 0. For more details see the section on the `cleaner` for which 
histograms are relevant.
 
### Slow Sorting
The second stage is  slow sorting the shifted data by timestamp and orgainizing detector hits into 
large data events. This is done through the use of TTree::BuildIndex which will sort a specific 
branch value (in this case timestamps). The data is then collected in events; events are 
structures which contain all detector hits that occur within a given coincidence window. This
event data is then written to an output file. The goal of the slow sorting is to be as general
as possible; slow sorting should change very little on a data set to data set basis, as this 
coincidence window is limited mostly be the time difference between an anode hit and the maximum
delay line time if the correct shifts are applied to SABRE and the scintillator.

### Fast Sorting
This is basically a secondary tier of event building, that is more user specific. It breaks down
data within the coincidence window into single focal plane events with asscoiated SABRE data. The
principle is that the scintillator provides very sharp timing resolution by which we can further
refine the built event. Currently, `FastSort` is desinged to take two windows: a coincidence window 
for SABRE and the scintillator, and a coincidence window for the ion chamber and the scintillator. 
For the ion chamber, the front anode was chosen to be the representative (it really doesn't matter
which part of the ion chamber is chosen). SABRE data is additionally filtered to contain only paired
hits (hits that have both a ring and a wedge). Fast sorting is where the user will have to make the
most changes to the actual event building. Any new detector or additional changes will require more
coincidence definitions and sorting depth. As such, this is probably the most computationally expensive
part of the analysis pipeline.

### Analyzing
Finally, the sorted event data is then converted into meaningful physical data, and saved to a 
final analyzed file. This is where the digitizer parameters (charge/energy, time, etc.) are converted
into the actual paramters of interest such as focal plane position, SABRE energy, etc. In this way, 
each raw data file gives four output files from the analysis: a shifted file, a slow sorted file,
a fast sorted file, and an analyzed file. The rationale behind the repetative writting is that
it helps the user isolate at which stage data issues occur at; this is especially useful for the 
shifting and sorting stages, where the values for the shifts and coincidence window have to be 
estimated by the user before running. 

All of the user input is handled through an input file in the program directory named 
`analyzer_input.txt`. This file is preformated; all the user needs to do is update the names and
values. Everything from input and output directories, to shifts and coincidence windows should
be specified in this file. Note that directorires should be explicit fullpaths.

See the `cleaner` section for advice on which histograms are useful for choosing the correct shifts
and window sizes for the data set.

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

### Determining Shifts and Windows
The cleaner already provides most of the histograms one would need to determine the shifts and windows
for a data set. These, in general, come from plots of the relative time of various components of the
detector. The goal of the scintillator and si shifts are to make them occur in coincidence with the
anode (pick one of the focal plane anodes, they occur at essentially the same time). Included automatically
are plots of the back anode relative to the scintillator (anodeB.Time-scintL.Time, gives scint offset), the
si relative to the scint (SABRE fronts and backs... pick higher res one to make offsets and shifts), and
maximum delay times relative to scint for both lines. Note also that boards can also have global offsets!
To account for this some boards will have to be shifted slightly differently in the RealTimer.cpp. THIS
VERSION CONTAINS AN INDIVIDUAL BOARD OFFSET AS AN EXAMPLE, WHEN MAKING A FIRST TEST REMOVE THIS.

The method is the following:

Using the anode relative to the scint, one can determine the scint offset (center the peak on 0). Then,
by looking at the SABRE relative to scint plots one can determine the shift for si and the fast window
size (again center the peak on 0, the width of the peak becomes the fast window). Finally, if everything goes
according to plan, now the maximum size of the slow coincidence window will be the relative time of the maximum
delay line signal. Look at the plot of this and determine where you want to cut off. Run it again and
check the results. You should look for, in general, reduced background and noise along with correct
centering of the timing peaks.

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

