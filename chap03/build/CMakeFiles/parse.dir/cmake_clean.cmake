FILE(REMOVE_RECURSE
  "lex.yy.c"
  "y.tab.h"
  "y.tab.c"
  "CMakeFiles/parse.dir/errormsg.c.o"
  "CMakeFiles/parse.dir/parsetest.c.o"
  "CMakeFiles/parse.dir/util.c.o"
  "CMakeFiles/parse.dir/lex.yy.c.o"
  "CMakeFiles/parse.dir/y.tab.c.o"
  "parse.pdb"
  "parse"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang C)
  INCLUDE(CMakeFiles/parse.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
