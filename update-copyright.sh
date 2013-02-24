#!/bin/bash
# Copyright (C) 2001-2010 Wormux Team.
# Copyright (C) 2010 The Mana World Development Team.
# Copyright (C) 2012 The Mana Developers
# Copyright (C) 2013 Bertram

new_year="$1"
copyright_notice="Bertram"
[[ -z $new_year ]] && echo "Missing parameter: year" && exit 1

[[ ! -e src ]] && echo "This script should be ran from the top repository dir" && exit 2

tmp_file="w$RANDOM$RANDOM$RANDOM$RANDOM"
[[ -e $tmp_file ]] && tmp_file="w$RANDOM$RANDOM$RANDOM$RANDOM"

# update the dates, creating the interval if it doesn't exist yet
find -iname "*.cpp" -or -iname "*.h" -or -iname "*.hpp" | 
  xargs sed -i "/Copyright.*$copyright_notice/ s,\(20[0-9]*\) \|\(20[0-9]*\)-20[0-9]* ,\1\2-$new_year ,"

# do a semi-automated commit check
git diff > $tmp_file
echo "The next +/- counts mentioning copyrights should match:"
grep "^[-+][^-+]" $tmp_file | sort | uniq -c
echo "If they don't, try finding the offending files with grep -rl <\$bad_line>"

# Remove temp file
[[ -e $tmp_file ]] && rm $tmp_file

# Indicate the source file that may miss the copyright notice.
echo "Those files are missing the given Copyright notice."
echo "You might want to check them:"
find src/ -type f -name "*.[Cc][Pp][Pp]" -or -name "*.[Hh]" -or -name "*.[Cc]" -or -name "*.[Hh][Pp][Pp]" | xargs grep -RiL "$copyright_notice"
echo "End of $0 script."
