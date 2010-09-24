#!/usr/bin/env python
# This script replaces the path of the dependencies in all the dylib files in the specified 
# directory. The paths must match a given user string and this string will be replaced by the
# given replacement string. For example:
# replacepath.py --old /opt/local/lib --new @executable_path/Contents/lib --dir ./libs -Rs
# Add the -R switch to enable recursive exploration of input directory.
# By Andres Colubri

import sys
import os
import subprocess

def print_help():
    print 'This script replaces the path of the dependencies in all the dylib files in the '
    print 'specified directory. The paths must match a given user string and this string   '
    print 'will be replaced by the given replacement string. For example:                  '   
    print 'replacepath.py --old /opt/local/lib/ --new @executable_path/ --dir ./libs -R    '
    print 'Add the -R switch to enable recursive exploration of input directory.           ' 

def print_error(error):
    print error + '. Get some help by typing:'
    print 'replacepath.py --help'

def process_lib(old, new, name, fn):
    pipe = subprocess.Popen('otool -L ' + fn, shell=True, stdout=subprocess.PIPE).stdout
    output = pipe.readlines()

    for line in output[1:len(output)]:
        line = line.strip()
        dep_old = line.split()[0]
        if -1 < dep_old.find(old):
            dep_new = dep_old.replace(old, new)
            if -1 < dep_old.find(name):
                # Changing name of dylib.
                proc = subprocess.Popen('install_name_tool -id ' + dep_new + ' ' + fn, shell=True)
                sts = os.waitpid(proc.pid, 0)[1]
            else:
                # Changing name of dependencies.
                proc = subprocess.Popen('install_name_tool -change ' + dep_old + ' ' + dep_new + ' ' + fn, shell=True)
                sts = os.waitpid(proc.pid, 0)[1]

    # Checking replacement succcess:
    pipe = subprocess.Popen('otool -L ' + fn, shell=True, stdout=subprocess.PIPE).stdout
    output = pipe.readlines()
    for line in output[1:len(output)]:
        line = line.strip()
        dep_old = line.split()[0]
        if -1 < dep_old.find(old):        
           print "Replacement failed:", fn, dep_old

def process_dir(args, dirname, filelist):
    for filename in filelist:
        ext = os.path.splitext(filename)[1]
        if ext == '.dylib' or ext == '.so':
            fn = os.path.join(dirname, filename)
            process_lib(args[0], args[1], filename, fn)

def main():
    old_string = ''
    new_string = ''
    input_dir = ''
    input_file = ''
    recursive = 0
    
    next_is_value = 0
    l = len(sys.argv)
    if 1 < l:
        for i in range(1, l):
            if next_is_value:
                next_is_value = 0 
                continue
            arg = sys.argv[i]
            if arg == '--old':
                if i + 1 < l:
                    next_is_value = 1
                    old_string = sys.argv[i+1]
                else:
                    print_error('Missing argument value')
                    return
            elif arg == '--new':
                if i + 1 < l:
                    next_is_value = 1
                    new_string = sys.argv[i+1]
                else:
                    print_error('Missing argument value')
                    return
            elif arg == '--dir':
                if i + 1 < l:
                    next_is_value = 1
                    input_dir = sys.argv[i+1]
                else:
                    print_error('Missing argument value')
                    return
            elif arg == '--file':
                if i + 1 < l:
                    next_is_value = 1
                    input_file = sys.argv[i+1]
                else:
                    print_error('Missing argument value')
                    return
            elif arg == '-R':
                recursive = 1
            elif arg == '--help':
                print_help()
                return
            else:
                print_error('Unknown argument')
                return
    else:
        print_help()
        return

    if old_string == '':
        print_error('Old string parameter is empty')
        return

    if new_string == '':
        print_error('New string parameter is empty')
        return

    if input_file != '':
        print 'Working (single file mode)...'
        process_lib(old_string, new_string, os.path.basename(input_file), input_file)
        print 'Done.'
    elif input_dir != '':
        print 'Working...'
        if recursive:
            os.path.walk(input_dir, process_dir, [old_string, new_string])
        else:
            filelist = os.listdir(input_dir)
            process_dir([old_string, new_string], input_dir, filelist)
        print 'Done.'
    else:
        print_error('Input directory parameter is empty')
        return

main()