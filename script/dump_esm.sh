#!/bin/bash

if [[ $# -lt 2 ]]; then
    echo "it should be $0 esm_file target_dir"
    exit 0
fi

esm=$1
dir=$2

records="\
ACTI \
ALCH \
APPA \
ARMO \
BODY \
BOOK \
BSGN \
CELL \
CLAS \
CLOT \
CNTC \
CONT \
CREA \
CREC \
DIAL \
DOOR \
ENCH \
FACT \
GLOB \
GMST \
INFO \
INGR \
LAND \
LEVC \
LEVI \
LIGH \
LOCK \
LTEX \
MGEF \
MISC \
NPC_ \
NPCC \
PGRD \
PROB \
RACE \
REGN \
REPA \
SCPT \
SKIL \
SNDG \
SOUN \
SPEL \
SSCR \
STAT \
WEAP \
SAVE \
JOUR \
QUES \
GSCR \
PLAY \
CSTA \
GMAP \
DIAS \
WTHR \
KEYS \
DYNA \
ASPL \
ACTC \
MPRJ \
PROJ \
DCOU \
MARK \
ENAB \
CAM_ \
STLN \
INPU \
FILT \
DBGP \
"

for record in $records ; do
    echo "dumping $record"
    esmtool dump "$esm" -t "$record">"$dir"/"$record"
    esmtool dump -r "$esm" -t "$record">"$dir"/"$record"_r
done
