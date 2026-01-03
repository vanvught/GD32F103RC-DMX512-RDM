#!/bin/bash
cd ..

DIR=gd32_*
MAKEFILE=Makefile*.GD32

for f in $DIR
do
	rm -rf /tmp/$f
	mkdir /tmp/$f
done

for f in $DIR
do
	echo -e "\e[32m[$f]\e[0m"
	if [ -d $f ]; then
		cd "$f"
		
			for m in $MAKEFILE
			do
				make -f $m -j clean
				make -f $m
				retVal=$?
				
				if [ $retVal -ne 0 ]; then
				 	echo "Error : " "$f" " : " "$m"
					exit $retVal
				fi
				
				SUFFIX1=$(echo $m | cut -d '-' -f 2 | cut -d '.' -f 1)
				SUFFIX2=$(echo $m | cut -d '-' -f 3 | cut -d '.' -f 1)
			
				if [ $SUFFIX1 == 'Makefile' ]
				then
					cp gd32f103rc.bin /tmp/$f/$i
				else
					echo "[" $SUFFIX1 "][" $SUFFIX2 "]"
					
					if [ -z "$SUFFIX2" ]
					then
						mkdir /tmp/$f/$i/$SUFFIX1/
						cp gd32f103rc.bin /tmp/$f/$i/$SUFFIX1
					else
						mkdir -p /tmp/$f/$i/$SUFFIX1/$SUFFIX2/
						cp gd32f103rc.bin /tmp/$f/$i/$SUFFIX1/$SUFFIX2/
					fi
				fi
							
			done
		
		cd -
	fi
done

find . -name gd32f103rc.bin | sort | xargs ls -al
find . -name gd32f103rc.bin | xargs ls -al | wc -l
