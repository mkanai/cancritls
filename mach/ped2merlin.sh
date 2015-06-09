#!/bin/bash

CMDNAME=$(basename $0)

warning() {
    echo "$CMDNAME: $1" 1>&2
}

usage_exit () {
    warning "Usage: $CMDNAME [-p pedname] [-m mapname] [-d delimiter] [-o prefix] [filename]"
    exit 1
}

while getopts p:m:d:o: OPT
do
    case $OPT in
        o) PREFIX="$OPTARG" ;;
        p) PED="$OPTARG" ;;
        m) MAP="$OPTARG" ;;
        d) DELIMITER="$OPTARG" ;;
        *) usage_exit ;;
    esac
done

shift $((OPTIND - 1))

set -e

if [ -n "$1" ]
then
    FILENAME=$(basename $1)
    if [ -z "$PED" ]
    then
        PED="$FILENAME.ped"
    fi
    if [ -z "$MAP" ]
    then
        MAP="$FILENAME.map"
    fi
    if [ -z "$PREFIX" ]
    then
        PREFIX="$FILENAME"
    fi
fi

if [ -z "$DELIMITER" ]
then
    DELIMITER=' '
fi

if [ -r "$PED" ]
then
    cut -f6 --complement -d "$DELIMITER" "$PED" > "$PREFIX.merlin.ped"
else
    warning "Cannot Read $PED."
    exit 1
fi

if [ -r "$MAP" ]
then
    awk '{OFS="\t"}{print "M",$2}' "$MAP" > "$PREFIX.merlin.dat"
    cut -f2 -d "$DELIMITER" $PREFIX.merlin.dat > $PREFIX.merlin.snps
else
    warning "Cannot Read $MAP."
    exit 1
fi
