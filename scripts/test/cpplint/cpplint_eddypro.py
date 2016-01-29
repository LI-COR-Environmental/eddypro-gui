#!/usr/bin/env python

import argparse
import glob
import imp
import os
import shutil
import sys

FILTERS = ['-whitespace/braces,'
           '-whitespace/indent,'
           '-whitespace/newline,'
           '-readability/braces,'
           '-build/c++11']

LINELENGTH = 120

def getargs():
    parser = argparse.ArgumentParser(description='EddyPro GUI cpplint script decorator', add_help=True, version='1.1')
    parser.add_argument('-n', '--nofilter', dest='nofilter', action='store_true', default=False, help='prevent filtering cpplint warnings')
    parser.add_argument('-l', '--filterlist', dest='filterlist', action='store_true', default=False, help='output cpplint filters list')
    parser.add_argument(dest='filelist', type=argparse.FileType('rt'), help='file containing the list of files to check')
    parser.add_argument('args', nargs=argparse.REMAINDER)

    try:
        return parser.parse_args()
        # return parser.parse_known_args()
    except IOError, msg:
        parser.error(str(msg))

def main():
    cpplint = imp.load_source('cpplint', 'cpplint/cpplint.py')

    args = getargs()

    # output the filters list and exit
    if args.filterlist is not None:
      if args.filterlist == True:
          cpplint_filters = '--filter='
          cpplint_args = [cpplint_filters]
          cpplint.main(cpplint_args)
          return

    # traverse the file list, linting files one by one
    with args.filelist as file_list:
        cpplint_filters = '--filter={0}'.format(''.join(FILTERS))
        cpplint_counting = '--counting=detailed'
        cpplint_linelength = '--linelength={0}'.format(LINELENGTH)

        for filename in file_list:
            cpplint_file = filename.replace('\n', '')

            if args.nofilter == True:
                cpplint_filters = '--filter='
                cpplint_linelength = '--linelength=80' # default value in cpplint
                cpplint_args = [cpplint_counting, cpplint_linelength, cpplint_file]
            else:
                cpplint_args = [cpplint_filters, cpplint_counting, cpplint_linelength, cpplint_file]

            print 'Linting file <' + cpplint_file + '>,  unsing the following cpplint flags:'
            print cpplint_filters
            print cpplint_counting
            print cpplint_linelength
            print

            # run cpplint
            errors = cpplint.main(cpplint_args)
            print "Errors: {}".format(errors)

            # if test passed, copy the file in the current dir
            if not errors:
                shutil.copy(cpplint_file, os.getcwd())

            print('-------')

if __name__ == '__main__':
    status = main()
    sys.exit(status)
