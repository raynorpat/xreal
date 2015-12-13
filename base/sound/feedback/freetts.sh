#!/bin/sh
FREETTS_HOME=/opt/freetts-1.2.1
VOICE=mbrola_us1

for i in `ls *.txt`;
        do java -Dmbrola.base=$FREETTS_HOME/mbrola -jar $FREETTS_HOME/lib/freetts.jar -voice $VOICE -file $i -dumpAudio `basename $i .txt`_.wav;
        sox `basename $i .txt`_.wav -v 5.0 `basename $i .txt`.wav;
        play `basename $i .txt`.wav;
        rm `basename $i .txt`_.wav;
done
