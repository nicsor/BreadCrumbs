# Gource image

## Build
The docker image attached is provided to help extract some git statistics.

To create the docker image simply run from within the docker folder:
 docker image build -t nna/gitinspector:latest .

## Sample execution
```
cd /root/of/git/directory
docker run --rm -v ${PWD}:${PWD} -w ${PWD} -ti nna/gitinspector


docker run --rm -v ${PWD}:${PWD} -w ${PWD} -ti nna/gitinspector -m -r -F html > statistics.html
```

## Additional parameters
```
  -f, --file-types=EXTENSIONS    a comma separated list of file extensions to
                                   include when computing statistics. The
                                   default extensions used are:
                                   java,c,cc,cpp,h,hh,hpp,py,glsl,rb,js,sql
                                   Specifying * includes files with no
                                   extension, while ** includes all files
  -F, --format=FORMAT            define in which format output should be
                                   generated; the default format is 'text' and
                                   the available formats are:
                                   html,htmlembedded,json,text,xml
      --grading[=BOOL]           show statistics and information in a way that
                                   is formatted for grading of student
                                   projects; this is the same as supplying the
                                   options -HlmrTw
  -H, --hard[=BOOL]              track rows and look for duplicates harder;
                                   this can be quite slow with big repositories
  -l, --list-file-types[=BOOL]   list all the file extensions available in the
                                   current branch of the repository
  -L, --localize-output[=BOOL]   localize the generated output to the selected
                                   system language if a translation is
                                   available
  -m  --metrics[=BOOL]           include checks for certain metrics during the
                                   analysis of commits
  -r  --responsibilities[=BOOL]  show which files the different authors seem
                                   most responsible for
      --since=DATE               only show statistics for commits more recent
                                   than a specific date
  -T, --timeline[=BOOL]          show commit timeline, including author names
      --until=DATE               only show statistics for commits older than a
                                   specific date
  -w, --weeks[=BOOL]             show all statistical information in weeks
                                   instead of in months
  -x, --exclude=PATTERN          an exclusion pattern describing the file
                                   paths, revisions, revisions with certain
                                   commit messages, author names or author
                                   emails that should be excluded from the
                                   statistics; can be specified multiple times
  -h, --help                     display this help and exit
      --version                  output version information and exit

```
