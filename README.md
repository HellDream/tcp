# tcp
tcp or tcpm to read and write file in unix

NAME tcp —trivially copy a file

SYNOPSIS tcp source target

DESCRIPTION
The tcp utility copies the contents of the source to target.
If target is a directory, tcp will copy source into this directory.
EXAMPLES
The following examples show common usage:
tcp file1 file2
tcp file1 dir
tcp file1 dir/file2
tcp file1 dir/subdir/subsubdir/file2
