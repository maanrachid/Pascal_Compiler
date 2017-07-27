# This file contains dependencies in addition to the ones that appear
# in the .c files.  This happens because the file  parse.h  specifies
# that  syntaxTree.h  be included, while  syntaxTree.h
# specifies that  scan.h  be included.

OBJS = testCompiler.o grammar.tab.o lex.yy.o tools.o makeSyntaxRecs.o \
 makeCheckerRecs.o checker.o symtab.o checkerTools.o stdObjects.o codegen.o codeDetails.o	

compiler:  $(OBJS)
	          gcc -o compiler $(OBJS) -lfl


testCompiler.o:	 testCompiler.c scan.h syntaxTree.h parse.h check.h codegen.h
	        gcc -c -g testCompiler.c


grammar.tab.o: grammar.tab.c tools.h syntaxTree.h parse.h scan.h
	gcc -c -g grammar.tab.c

grammar.tab.c grammar.tab.h: grammar.y
	bison -d grammar.y

lex.yy.o:  lex.yy.c syntaxTree.h grammar.tab.h scan.h tools.h
	gcc -c -g lex.yy.c

lex.yy.c:  scanSpec.l
	flex scanSpec.l

tools.o: tools.c tools.h
	gcc -g -c tools.c

makeSyntaxRecs.o: makeSyntaxRecs.c tools.h syntaxTree.h grammar.tab.h scan.h 
	gcc -g -c makeSyntaxRecs.c

makeCheckerRecs.o: makeCheckerRecs.c checkerGlobals.h tools.h syntaxTree.h check.h scan.h 
	gcc -g -c makeCheckerRecs.c  


checker.o: checker.c syntaxTree.h check.h typeinfo.h symtab.h tools.h \
grammar.tab.h scan.h checkerTools.h
	gcc -g -c checker.c

symtab.o: symtab.c tools.h syntaxTree.h check.h typeinfo.h symtab.h scan.h
	gcc -g -c symtab.c

checkerTools.o: checkerTools.c scan.h syntaxTree.h check.h typeinfo.h \
symtab.h checkerTools.h
	gcc -g -c checkerTools.c

stdObjects.o: stdObjects.c checkerGlobals.h tools.h scan.h syntaxTree.h check.h stdObjects.h procCodes.h checkerTools.h
	gcc -g -c stdObjects.c


codegen.o: codegen.c scan.h syntaxTree.h check.h codegen.h tools.h \
codeDetails.h opcodes.h procCodes.h grammar.tab.h checkerTools.h
	        gcc -g -c codegen.c

codeDetails.o: codeDetails.c scan.h syntaxTree.h check.h codeDetails.h \
tools.h opcodes.h
	        	gcc -g -c codeDetails.c


clean:
	rm -f $(OBJS) lex.yy.c grammar.tab.c grammar.tab.h
