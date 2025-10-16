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
	Comparison * comparison;
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
%token <token> TABLE
%token <token> PROJECTION
%token <token> RHO

%token <token> left
%token <token> right


%token <token> COLON
%token <token> COMMA

%token <token> UNION
%token <token> INTERSECTION
%token <token> DIFF
%token <token> JOIN

%token <token> NAME
%token <token> INPUT

%token <condition> LOWER    /* x<10*/
%token <condition> LOWER_EQUAL
%token <condition> HIGHER_EQUAL
%token <condition> HIGHER
%token <condition> EQUAL
%token <condition> NOT_EQUAL /* x!=10 */

%token <token> OPERATION
%token <token> UNKNOWN


/** Non-terminals. */
%type <constant> constant
%type <attributes> attributes

%type <expression> expression


%type <expression> column_list
%type <expression> table
%type <expression> input
%type <expression> projection
%type <expression> selection
%type <expression> side_input
%type <expression> rho



%type <condition> condition
%type <condition> simple_condition
%type <condition> compound_condition
%type <condition> condition_list
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
 | OPEN_BRACE expression CLOSE_BRACE											{ $$ = ExpressionProgramSemanticAction($2); }
	;

expression: 
	table
	|selection
	|projection
	|rho
	|relation
	|input
	;


table: TABLE COLON OPEN_BRACE  STRING CLOSE_BRACE {$$= BaseRelationSemanticAction($4)};
selection: SELECTION COLON OPEN_BRACE comparison COMMA input CLOSE_BRACE {$$= SelectionSemanticAction($4,$6) };
projection: PROJECTION COLON OPEN_BRACE attributes_param COMMA input CLOSE_BRACE{$$= ProjectionSemanticAction($6, $4 )};
rho: RHO COLON OPEN_BRACE NAME  STRING COMMA input CLOSE_BRACE
input: INPUT COLON  OPEN_BRACE expression CLOSE_BRACE
side_input:
	left COLON OPEN_BRACE expression CLOSE_BRACE
	right COLON OPEN_BRACE expression CLOSE_BRACE

condition: 

	AND OPEN_BRACKET COMMA  


condition[left] AND condition[right]			{ $$ = BinaryConditionSemanticAction($left, $right, "AND"); }
	| condition[left] OR condition[right]				{ $$ = BinaryConditionSemanticAction($left, $right, "OR"); }
	| NOT condition	[expr]								{ $$ = UnaryConditionSemanticAction($expr); }
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
JOIN COLON OPEN_BRACE condition COMMA side_input COMMA side_input {$$= BinaryRelationSemanticAction(JOIN, $5, $6, $4 )}
| UNION COLON OPEN_BRACE side_input COMMA side_input {$$= BinaryRelationSemanticAction(UNION, $4, $5, null )}
| INTERSECTION COLON OPEN_BRACE side_input COMMA side_input {$$= BinaryRelationSemanticAction(INTERSECTION, $4, $5, null )}
| DIFF COLON OPEN_BRACE side_input COMMA side_input {$$= BinaryRelationSemanticAction(DIFF, $4, $5, null )}
;

attribute_list:
	STRING
	STRING COMMA attribute_list

attributes_param:
	OPEN_BRACKET attribute_list CLOSE_BRACKET
	
constant: INTEGER										{ $$ = IntegerConstantSemanticAction($1); }
	;

%%
