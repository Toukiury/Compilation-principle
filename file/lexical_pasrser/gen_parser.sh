#!/bin/bash
flex --outfile lex.cpp --header-file=lex.hpp lex.l
`which bison` -d -Wcounterexamples --output yacc.cpp yacc.y
