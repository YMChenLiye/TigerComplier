for dir in ./*
do
	if test -d $dir
	then
		for file in $dir/*
		do
			if test -f $file
			then
				if [ "${file##*.}"x = "h"x ]||[ "${file##*.}"x = "c"x ];	
				then
					#do something
					echo "format " $file
					clang-format -i -style=file $file
				fi
			fi
		done
		#find $dir -regex ".*\.[hc]" | xargs clang-format -i -style=file
	fi
done
