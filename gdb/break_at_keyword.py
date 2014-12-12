#! /usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2013 NAKAMURA Minoru <nminoru@nminoru.jp>
#
# $Date:: 2013-10-09 11:46:17 +0900#$
# $Rev: 203 $
#
import gdb
import os
import re
from functools import partial


if (os.environ.has_key('SOURCE_ROOT') == True):
    rootdir = os.environ['SOURCE_ROOT']
else:
    rootdir = "."


def break_at_keyword(keyword, filename, offset=0, comment_remove=True):
    as_c_identifier = is_keyword_c_identifier(keyword)
    filepath = find_target_file(filename)
    source_file = SourceFile(filepath)

    linenum = 0
    for line in source_file.get_lines(comment_remove):
        linenum += 1
        if (find_keyword_in_line(keyword, line, as_c_identifier)):
            set_breakpoint_at_keyword(filename, linenum + offset)
            return 


def break_at_keyword_in_function(keyword, fname, offset=0, comment_remove=True):
    as_c_identifier = is_keyword_c_identifier(keyword)

    tuple = gdb.decode_line(fname)
    if (tuple == None):
        die("Can't find " + fname)

    symtab_and_line = tuple[0]
    filepath = symtab_and_line.symtab.fullname()
    source_file = SourceFile(filepath)

    linenum = 0
    for line in source_file.get_lines(comment_remove):
        linenum += 1
        if (linenum < symtab_and_line.line):
            continue
        if (find_keyword_in_line(keyword, line, as_c_identifier)):
            set_breakpoint_at_keyword(symtab_and_line.symtab.filename, linenum + offset)
            return


def break_at_keyword_after_position(keyword, position, filename, offset=0, comment_remove=True):
    as_c_identifier = is_keyword_c_identifier(keyword)
    filepath = find_target_file(filename)
    source_file = SourceFile(filepath)

    linenum = 0
    appear_position_word = False
    raw_lines = source_file.get_lines(False)

    for line in source_file.get_lines(comment_remove):
        linenum += 1

        if (raw_lines[linenum - 1].find(position) != -1):
            appear_position_word = 1
            
        if (appear_position_word and find_keyword_in_line(keyword, line, as_c_identifier)):
            set_breakpoint_at_keyword(filename, linenum + offset)
            return 


def break_at_keyword_before_position(keyword, position, filename, offset=0, comment_remove=True):
    as_c_identifier = is_keyword_c_identifier(keyword)
    filepath = find_target_file(filename)
    source_file = SourceFile(filepath)

    linenum = 0
    linenum_includes_keyword = 0
    raw_lines = source_file.get_lines(False)

    for line in source_file.get_lines(comment_remove):
        linenum += 1

        if (find_keyword_in_line(keyword, line, as_c_identifier)):
            linenum_includes_keyword = linenum

        if (raw_lines[linenum - 1].find(position) != -1):
            set_breakpoint_at_keyword(filename, linenum_includes_keyword + offset)
            return


def is_keyword_c_identifier(keyword):
    m = re.search(r"([a-zA-Z_][a-zA-Z_0-9]*)", keyword)
    if (m != None and len(m.groups()) == 1):
        if (keyword == m.groups()[0]):
            return True
    return False


def find_keyword_in_line(keyword, line, as_c_identifier):
    if (as_c_identifier):
        for word in re.findall(r"([a-zA-Z_][a-zA-Z_0-9]*)", line):
            if (word == keyword):
                return True
        return False
    else:
        return line.find(keyword) != -1;


def set_breakpoint_at_keyword(filename, linenum):
    command =  "b " + filename + ":" + str(linenum)
    break_point = gdb.execute(command, False, True)
    bnum = re.sub(r"Breakpoint (\d+) at .*", r"\1", break_point)
    # gdb.parse_and_eval("set $bnum = " + str(bnum))
    return


# This function searches `target_file' in the directory tree from `rootdir' and
# returns the full file path.
def find_target_file(target_file):
    for dirpath, dirs, files in os.walk(rootdir, topdown=True):
        for filename in files:
            if (target_file == filename):
                return dirpath + '/' + filename
    die("Can't find " + target_file + " in " + rootdir)


def replace_func(m, source_file):
    if (m.group(3) != None):
        start = m.start(3)
        end   = m.end(3)
        for i in range(start, end):
            source_file.output_str_list[i] = source_file.orig_str_list[i]
    return ""


# Read C/C++ source file
class SourceFile(object):

    def __init__(self, target_file):
        self.contents = ""
        for line in open(target_file, 'r'):
            self.contents += line
        self.orig_str_list   = list(self.contents); 
        self.output_str_list = list(re.sub(r".", " ", self.contents));

    def get_lines(self, comment_remove):
        if (comment_remove == False):
            return re.split(r"\n", self.contents)
        else:
            # Remove /* */ or // style comments 
            re.sub(r"(?s)/\*[^*]*\*+([^/*][^*]*\*+)*/|//([^\\]|[^\n][\n]?)*?\n|(\"(\\.|[^\"\\])*\"|'(\\.|[^'\\])*'|.[^/\"'\\]*)",
                       partial(replace_func, source_file = self), self.contents)
            return re.split(r"\n", "".join(self.output_str_list))
