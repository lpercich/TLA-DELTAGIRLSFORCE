%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	TokenLabel token;
	boolean boolean;
	char * string;
	


	/** Non-terminals. */

	
	Expression * expression; /*operadores*/
	Condition* condition;
	ConditionList * condition_list;
	Constant * constant;
	Attribute * attributes;
	Program * program;
	Aggregation * aggregation; /*funciones de agregacion*/
	Order * order;
	Relation *relation;   /*tablas*/
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyCondition($$); } <condition>

/** Terminals. */
%token <integer> INTEGER
%token <string> STRING
%token <boolean> BOOL
%token <string> IDENTIFIER
%token <string> VALUE
%token <token> CLOSE_BRACE  /* o sea {} */
%token <token> CLOSE_BRACKET  /* o sea [] */
%token <token> CLOSE_PARENTHESIS  /* o sea () */
%token <token> OPEN_BRACE /* {} */
%token <token> OPEN_BRACKET /* [] */
%token <token> OPEN_PARENTHESIS /* () */
%token <condition> OR
%token <condition> AND 
%token <condition> NOT 
%token <token> SELECTION
%token <token> PROJECTION
%token <token> COLON
%token <token> COMMA
%token <token> UNION
%token <token> INTERSECTION
%token <token> DIFF
%token <token> JOIN
%token <condition> LOWER    /* x<10*/
%token <condition> LOWER_EQUAL
%token <condition> HIGHER_EQUAL
%token <condition> EQUAL
%token <condition> NOT_EQUAL /* x!=10 */
%token <token> RHO
%token <token> UNKNOWN
%

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <attributes> attributes
%type <expression> column_list
%type <expression> table
%type <expression> input
%type <condition> condition
%type <condition> simple_condition
%type <condition> compound_condition
%type <condition> condition_list



/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression											{ $$ = ExpressionProgramSemanticAction($1); }
	;

condition: condition[left] AND condition[right]			{ $$ = BinaryExpressionSemanticAction($left, $right, "AND"); }
	| condition[left] OR condition[right]				{ $$ = BinaryExpressionSemanticAction($left, $right, "OR"); }
	| NOT condition	[expr]								{ $$ = UnaryExpressionSemanticAction($expr); }
	| comparison                           				{ $$ = $comparison; }
	;

comparison:
      IDENTIFIER[left] EQUAL VALUE[right]          { $$ = ComparisonConditionSemanticAction($left, "=", $right); }
    | IDENTIFIER[left] NOT_EQUAL VALUE[right]      { $$ = ComparisonConditionSemanticAction($left, "!=", $right); }
    | IDENTIFIER[left] LOWER VALUE[right]          { $$ = ComparisonConditionSemanticAction($left, "<", $right); }
    | IDENTIFIER[left] LOWER_EQUAL VALUE[right]    { $$ = ComparisonConditionSemanticAction($left, "<=", $right); }
    | IDENTIFIER[left] HIGHER VALUE[right]         { $$ = ComparisonConditionSemanticAction($left, ">", $right); }
    | IDENTIFIER[left] HIGHER_EQUAL VALUE[right]   { $$ = ComparisonConditionSemanticAction($left, ">=", $right); }
    ;



constant: INTEGER											{ $$ = IntegerConstantSemanticAction($1); }
	;

	


%%
