#!/bin/sh

# Copyright (C) 2016-2017  Nicola Spanti (RyDroid) <dev@nicola-spanti.info>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


EXIT_FAILURE=1
cmd='file --brief --mime-encoding'

for file_or_dir in "$@"
do
    if test ! -e "$file_or_dir"
    then
	>&2 echo "$file_or_dir does not exist!"
	exit $EXIT_FAILURE
    else
	files=$(find "$file_or_dir" \( \
		-name '*.h'  -o -name '*.c' -o \
		-name '*.hh' -o -name '*.hpp' -o -name '*.hxx' -o \
		-name '*.cc' -o -name '*.cpp' -o -name '*.cxx' -o \
		-name '*.java' -o -name '*.js' -o -name '*.php' -o \
		-name '*.xml' -o -name '*.xsd' -o -name '*.svg' -o \
		-name '*.xsl' -o -name '*.xslt' -o \
		-name '*.json' -o -name '*.yml' -o \
		-name '*.htm' -o -name '*.html' -o -name '*.xhtml' -o \
		-name '*.css' -o \
		-name '*.txt' -o -name '*.md' -o -name '*.mkd' -o \
		-name '.*config' -o -name '*.el' -o \
		-name Doxyfile -o -name makefile \))
	for file in $files
	do
	    nb_invalid=$($cmd "$file" | grep -ivc 'ascii\|utf-8' | tr -d '\r\n')
	    if test "$nb_invalid" != '0'
	    then
		>&2 echo "$file"
		exit $EXIT_FAILURE
	    fi
	done
    fi
done
