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
	bool boolean;
	char * string;
	


	/** Non-terminals. */

	
	Expression * expression; /*operadores*/
	Condition* condition;
	Constant * constant;
	Condition * comparison;
	Attributes * attributes;
	Attributes attribute;
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
%token <string> VALUE

%token <token> CLOSE_BRACE  /* o sea {} */
%token <token> CLOSE_BRACKET  /* o sea [] */
%token <token> CLOSE_PARENTHESIS  /* o sea () */
%token <token> OPEN_BRACE /* {} */
%token <token> OPEN_BRACKET /* [] */
%token <token> OPEN_PARENTHESIS /* () */
%token <token> IGNORED

%token <string> OR
%token <string> AND 
%token <token> NOT 

%token <token> SELECT
%token <token> TABLE
%token <token> PROJECT
%token <token> RENAME

%token <token> left
%token <token> right

%token <string> table
%token <token> COLON
%token <token> COMMA

%token <token> UN
%token <token> INTER
%token <token> DIF
%token <token> JOINTOKEN
%token <token> CARTESIAN_PRODUCT
%token <token> ATTRIBUTES


%token <token> NAME
%token <token> INPUT

%token <token> LOWER    /* x<10*/
%token <token> LOWER_EQUAL
%token <token> HIGHER_EQUAL
%token <token> HIGHER
%token <token> EQUAL
%token <token> NOT_EQUAL /* x!=10 */

%token <token> UNKNOWN



/** Non-terminals. */
%type <constant> constant
%type <attributes> attributes
%type <attributes> attributes_param

%type <expression> expression


%type <expression> input
%type <relation> side_input

%type <condition> condition
%type <comparison> comparison
%type <relation> relation
%type <program> program


/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: 
  OPEN_BRACE expression CLOSE_BRACE											{ $$ = ExpressionProgramSemanticAction($2); }
	|OPEN_BRACE relation CLOSE_BRACE										{$$ = RelationProgramSemanticAction($2);}
	;

expression: 
	SELECT COLON OPEN_BRACE condition COMMA input CLOSE_BRACE {$$= SelectionSemanticAction($4,$6); };
	|PROJECT COLON OPEN_BRACE attributes_param COMMA input CLOSE_BRACE{$$= ProjectionSemanticAction($6, $4);};
	|RENAME COLON OPEN_BRACE NAME COLON STRING COMMA input CLOSE_BRACE {$$= RenameSemanticAction($6, $8);};
	|input 
	;


 input: INPUT COLON OPEN_BRACE expression CLOSE_BRACE {$$ = $4;};


side_input:
	left COLON OPEN_BRACE relation CLOSE_BRACE {$$=$4;}
	|right COLON OPEN_BRACE relation CLOSE_BRACE {$$=$4;}
;


condition: 
	AND COLON OPEN_BRACKET condition COMMA condition CLOSE_BRACKET	{ $$ = BinaryConditionSemanticAction($4, $6, $1); }
	| OR COLON OPEN_BRACKET condition COMMA condition CLOSE_BRACKET			{ $$ = BinaryConditionSemanticAction($4, $6, "OR"); }
	| NOT COLON OPEN_BRACKET condition CLOSE_BRACKET							{ $$ = UnaryConditionSemanticAction($4); }
	| comparison                           				{ $$ = $comparison; }
	;

comparison:
	 EQUAL COLON OPEN_BRACKET VALUE COMMA VALUE CLOSE_BRACKET	 { $$ = ComparisonConditionSemanticAction($4, "=", $6); }
	| LOWER COLON OPEN_BRACKET VALUE COMMA VALUE CLOSE_BRACKET	 { $$ = ComparisonConditionSemanticAction($4, "<", $6); }
	| HIGHER COLON OPEN_BRACKET VALUE COMMA VALUE CLOSE_BRACKET	 { $$ = ComparisonConditionSemanticAction($4, ">", $6); }
	| LOWER_EQUAL COLON OPEN_BRACKET VALUE COMMA VALUE CLOSE_BRACKET { $$ = ComparisonConditionSemanticAction($4, "<=", $6); }
	| HIGHER_EQUAL COLON OPEN_BRACKET VALUE COMMA VALUE CLOSE_BRACKET { $$ = ComparisonConditionSemanticAction($4, ">=", $6); }
	| NOT_EQUAL COLON OPEN_BRACKET VALUE COMMA VALUE CLOSE_BRACKET { $$ = ComparisonConditionSemanticAction($4, "!=", $6); }

;

relation:
TABLE COLON table {$$= BaseRelationSemanticAction($3);};
|JOINTOKEN COLON OPEN_BRACE condition COMMA side_input COMMA side_input {$$= BinaryRelationSemanticAction(JOIN, $6, $8, $4 );}
|CARTESIAN_PRODUCT COLON OPEN_BRACE side_input COMMA side_input CLOSE_BRACE {$$= BinaryRelationSemanticAction(PRODUCT, $4, $6, NULL );}
| UN COLON OPEN_BRACE side_input COMMA side_input {$$= BinaryRelationSemanticAction(UNION, $4, $6, NULL );}
| INTER COLON OPEN_BRACE side_input COMMA side_input {$$= BinaryRelationSemanticAction(INTERSECTION, $4, $6, NULL);}
| DIF COLON OPEN_BRACE side_input COMMA side_input {$$= BinaryRelationSemanticAction(DIFF, $4, $6, NULL);}
;

attributes:
	STRING {$$= AtributeSemanticAction($1, NULL);}
	|STRING COMMA attributes {$$= AtributeSemanticAction($1, $3);}
;
attributes_param:
	 ATTRIBUTES COLON OPEN_BRACKET attributes CLOSE_BRACKET {$$=$2;}
	;
	
constant: INTEGER										{ $$ = IntegerConstantSemanticAction($1); }
	;

%%
