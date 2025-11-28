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


extern CompilationStatus symbolLexemeAction(TokenLabel label);
extern CompilationStatus logicalConditionLexemeAction(TokenLabel label);
extern CompilationStatus stringLexemeAction();
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus ignoredLexemeAction();
extern CompilationStatus integerLexemeAction();
extern CompilationStatus unknownLexemeAction();
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
whitespace        [ \t\n\r\f\v]+

digit       [0-9]
digits      {digit}+
sign        [+-]?
fraction    \.{digit}+
exponent    [eE]{sign}{digit}+

number      {sign}{digits}({fraction})?({exponent})?

string              \"([^\\\"]|\\.)*\"



%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}


"{"                                     { return newContextSymbolLexemeAction(OPEN_BRACE); }
"}"                                     { return closeContextSymbolLexemeAction(CLOSE_BRACE); }
"["						            	{ return newContextSymbolLexemeAction(OPEN_BRACKET); }
"]"						            	{ return closeContextSymbolLexemeAction(CLOSE_BRACKET); }
"("						            	{ return symbolLexemeAction(OPEN_PARENTHESIS); }
")"						            	{ return symbolLexemeAction(CLOSE_PARENTHESIS); }
":"						            	{ return symbolLexemeAction(COLON); }
","						            	{ return symbolLexemeAction(COMMA); }
"\"PROJECTION\""                        { return symbolLexemeAction(PROJECT); }
"\"TABLE\""                             { return symbolLexemeAction(TABLE); }
"\"SELECT\""                            { return symbolLexemeAction(SELECT); }
"\"JOIN\""                              { return symbolLexemeAction(JOIN_TOKEN); }
"\"UNION\""                             { return symbolLexemeAction(UNION_TOKEN); }
"\"INTERSECTION\""                      { return symbolLexemeAction(INTERSECTION_TOKEN); }
"\"DIFF\""                              { return symbolLexemeAction(DIFFERENCE_TOKEN); }
"\"PRODUCT\""                           { return symbolLexemeAction(CARTESIAN_PRODUCT);}
"\"RHO\""                               { return symbolLexemeAction(RENAME); }
"\"AND\""                               { return logicalConditionLexemeAction(AND); }
"\"OR\""                                { return logicalConditionLexemeAction(OR); }
"\"NOT\""                               { return logicalConditionLexemeAction(NOT); }
"\"<\""                                 { return logicalConditionLexemeAction(LOWER); }
"\"<=\""                                { return logicalConditionLexemeAction(LOWER_EQUAL); }
"\">\""                                 { return logicalConditionLexemeAction(HIGHER); }
"\">=\""                                { return logicalConditionLexemeAction(HIGHER_EQUAL); }
"\"=\""                                 { return logicalConditionLexemeAction(EQUAL); }
"\"!=\""                                { return logicalConditionLexemeAction(NOT_EQUAL); }

"\"AGGREGATION\""                       { return symbolLexemeAction(AGGREGATION_TOKEN); }
"\"group_by\""                          { return symbolLexemeAction(GROUP_BY); }
"\"aggregations\""                      { return symbolLexemeAction(AGGREGATIONS); }
"\"AVG\""                               { return symbolLexemeAction(AVERAGE); }
"\"SUM\""                               { return symbolLexemeAction(SUM); }
"\"COUNT\""                             { return symbolLexemeAction(COUNT); }
"\"MIN\""                               { return symbolLexemeAction(MIN); }
"\"MAX\""                               { return symbolLexemeAction(MAX); }
"\"ORDER\""                             { return symbolLexemeAction(ORDER); }
"\"directions\""                        { return symbolLexemeAction(DIRECTIONS); }
"\"ASC\""                               { return symbolLexemeAction(ASCENDANT_TOKEN); }
"\"DESC\""                              { return symbolLexemeAction(DESCENDANT_TOKEN); }

"\"attributes\""                        { return symbolLexemeAction(ATTRIBUTES); }
"\"input\""                             { return symbolLexemeAction(INPUT); }
"\"left\""                              { return symbolLexemeAction(left); }
"\"right\""                             { return symbolLexemeAction(right); }
"\"NAME\""                              { return symbolLexemeAction(NAME); }


[[:digit:]]+						    { return integerLexemeAction(); }

{string}                                { return stringLexemeAction(); }
{number}                                { return integerLexemeAction(); }
{whitespace}						    { return ignoredLexemeAction(); }


<<EOF>>								    { return EOFLexemeAction(); }
.								    	{ return unknownLexemeAction(); }
%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
