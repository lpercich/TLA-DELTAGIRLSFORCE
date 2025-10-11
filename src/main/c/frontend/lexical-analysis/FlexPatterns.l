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

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x IMPORT_EXPRESSION

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


<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); }

"{"                                 {return SymbolLexemeAction(OPEN_BRACE);}
"}"                                 {return SymbolLexemeAction(CLOSE_BRACE);}
"["									{ return SymbolLexemeAction(OPEN_BRACKETS); }
"]"									{ return SymbolLexemeAction(CLOSE_BRACKETS); }
"("									{ return SymbolLexemeAction(OPEN_PARENTHESIS); }
")"									{ return SymbolLexemeAction(CLOSE_PARENTHESIS); }
":"									{ return SymbolLexemeAction(COLON); }
","									{ return SymbolLexemeAction(COMMA); }
"PROJECTION"                        { return SymbolLexemeAction(PROJECTION); }
"SELECTION"                         { return SymbolLexemeAction(SELECTION); }
"JOIN"                              { return SymbolLexemeAction(JOIN); }
"UNION"                             { return SymbolLexemeAction(UNION); }
"INTERSECTION"                      { return SymbolLexemeAction(INTERSECTION); }
"DIFF"                              { return SymbolLexemeAction(DIFF); }
"RHO"                               { return SymbolLexemeAction(RHO); }
"AND"                               { return LogicalConditionLexemeAction(AND); }
"OR"                                { return LogicalConditionLexemeAction(OR); }
"NOT"                               { return LogicalConditionLexemeAction(OR); }
"LOWER"                             { return LogicalConditionLexemeAction(LOWER); }
"LOWER_EQUAL"                       { return LogicalConditionLexemeAction(LOWER_EQUAL); }
"HIGHER_EQUAL"                      { return LogicalConditionLexemeAction(HIGHER_EQUAL); }
"EQUAL"                             { return LogicalConditionLexemeAction(EQUAL); }
"NOT_EQUAL"                         { return LogicalConditionLexemeAction(NOT_EQUAL); }




[[:digit:]]+						{ return IntegerLexemeAction(); }

{number}                            { return IntegerLexemeAction(); }
{string}                            { return StringLexemeAction(); }
{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
