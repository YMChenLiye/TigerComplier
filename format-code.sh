for dir in ./*
do
	if test -d $dir
	then
		find $dir -regex ".*\.[hc]" | xargs clang-format -i -style=file 
	fi
done
