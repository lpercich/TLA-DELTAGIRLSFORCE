%top{


#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"


}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */


extern CompilationStatus SymbolLexemeAction(TokenLabel label);
extern CompilationStatus LogicalConditionLexemeAction(TokenLabel label);
extern CompilationStatus StringLexemeAction();
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus newContextSymbolLexemeAction(TokenLabel label);
extern CompilationStatus closeContextSymbolLexemeAction(TokenLabel label);

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%s IMPORT_EXPRESSION

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+
number              -?[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?
string              \"([^\"\\]|\\.)*\"

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}


"{"                                 {return newContextSymbolLexemeAction(OPEN_BRACE);}
"}"                                 {return closeContextSymbolLexemeAction(CLOSE_BRACE);}
"["									{ return newContextSymbolLexemeAction(OPEN_BRACKET); }
"]"									{ return closeContextSymbolLexemeAction(CLOSE_BRACKET); }
"("									{ return SymbolLexemeAction(OPEN_PARENTHESIS); }
")"									{ return SymbolLexemeAction(CLOSE_PARENTHESIS); }
":"									{ return SymbolLexemeAction(COLON); }
","									{ return SymbolLexemeAction(COMMA); }
"\"PROJECTION\""                        { return SymbolLexemeAction(PROJECT); }
"\"TABLE\""                        { return SymbolLexemeAction(TABLE); }
"\"SELECT\""                            { return SymbolLexemeAction(SELECT); }
"\"JOIN\""                              { return SymbolLexemeAction(JOINTOKEN); }
"\"UNION\""                             { return SymbolLexemeAction(UN); }
"\"INTERSECTION\""                      { return SymbolLexemeAction(INTER); }
"\"DIFF\""                              { return SymbolLexemeAction(DIF); }
"\"PRODUCT\""                           { return SymbolLexemeAction(CARTESIAN_PRODUCT);}
"\"RHO\""                               { return SymbolLexemeAction(RENAME); }
"\"AND\""                               { return LogicalConditionLexemeAction(AND); }
"\"OR\""                                { return LogicalConditionLexemeAction(OR); }
"\"NOT\""                               { return LogicalConditionLexemeAction(NOT); }
"\"<\""                                 { return LogicalConditionLexemeAction(LOWER); }
"\"<=\""                                { return LogicalConditionLexemeAction(LOWER_EQUAL); }
"\">\""                                 { return LogicalConditionLexemeAction(HIGHER); }
"\">=\""                                { return LogicalConditionLexemeAction(HIGHER_EQUAL); }
"\"=\""                                 { return LogicalConditionLexemeAction(EQUAL); }
"\"!=\""                                { return LogicalConditionLexemeAction(NOT_EQUAL); }

"\"attributes\""                        {return SymbolLexemeAction(ATTRIBUTES);}
"\"input\""                              {return SymbolLexemeAction(INPUT);}
"\"left\""                              {return SymbolLexemeAction(left);}
"\"right\""                              {return SymbolLexemeAction(right);}
"\"NAME\""                              {return SymbolLexemeAction(NAME);}


[[:digit:]]+						{ return IntegerLexemeAction(); }

{string}                            {return StringLexemeAction();}
{number}                            { return IntegerLexemeAction(); }
{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
