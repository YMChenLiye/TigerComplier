for file in ./*
do
	if test -d $file
		find $file -regex ".*.[hc]" | xargs clang-format -i style=file
	fi
done
