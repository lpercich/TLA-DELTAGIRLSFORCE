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
	Program * program;
	Aggregation * aggregation; /*funciones de agregacion*/
	Order * order;
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

%token <token> CLOSE_BRACE  /* o sea {} */
%token <token> CLOSE_BRACKET  /* o sea [] */
%token <token> CLOSE_PARENTHESIS  /* o sea () */
%token <token> OPEN_BRACE /* {} */
%token <token> OPEN_BRACKET /* [] */
%token <token> OPEN_PARENTHESIS /* () */
%token <token> IGNORED

%token <token> OR
%token <token> AND 
%token <token> NOT 

%token <token> SELECT
%token <token> TABLE
%token <token> PROJECT
%token <token> RENAME

%token <token> left
%token <token> right


%token <token> COLON
%token <token> COMMA

%token <token> UN
%token <token> INTER
%token <token> DIF
%token <token> JOINTOKEN
%token <token> CARTESIAN_PRODUCT
%token <token> ATTRIBUTES
%token <token> AGGREGATION 
%token <token> GROUP_BY 
%token <token> AGGREGATIONS 
%token <token> AVG
%token <token> SUM 
%token <token> COUNT 
%token <token> MIN 
%token <token> MAX


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
%type <attributes> attributes_list
%type <attributes> attributes_item
%type <attributes> attributes_param


%type <expression> expression

%type <expression> input
%type <expression> side_input
%type <expression> relation


%type <expression> aggregation
%type <aggregation> aggregation_function
%type <aggregation> aggregation_list

%type <condition> condition
%type <comparison> comparison
%type <program> program

%type <string>     value


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
	;

expression: 
	SELECT COLON OPEN_BRACE condition COMMA input CLOSE_BRACE {$$= SelectionSemanticAction($4,$6); };
	|PROJECT COLON OPEN_BRACE attributes_param COMMA input CLOSE_BRACE{$$= ProjectionSemanticAction($6, $4);};
	|RENAME COLON OPEN_BRACE NAME COLON STRING COMMA input CLOSE_BRACE {$$= RenameSemanticAction($6, $8);};
	|input 
	|TABLE COLON STRING                                           { $$ = BaseExpressionSemanticAction($3); }
	|relation
	|aggregation

	;


 input: INPUT COLON OPEN_BRACE expression CLOSE_BRACE { $$ = $4;};


side_input:
	left COLON OPEN_BRACE expression CLOSE_BRACE { $$=$4; }
	|right COLON OPEN_BRACE expression CLOSE_BRACE { $$=$4;}
;

aggregation_list:
      aggregation_function
    | aggregation_list COMMA aggregation_function
    ;

aggregation_function:
      OPEN_BRACE AVG COLON STRING CLOSE_BRACE  { $$ = AggregationFunctionSemanticAction("AVG", $4); }
    | OPEN_BRACE SUM COLON STRING CLOSE_BRACE  { $$ = AggregationFunctionSemanticAction("SUM", $4); }
    | OPEN_BRACE COUNT COLON STRING CLOSE_BRACE { $$ = AggregationFunctionSemanticAction("COUNT", $4); }
    | OPEN_BRACE MIN COLON STRING CLOSE_BRACE  { $$ = AggregationFunctionSemanticAction("MIN", $4); }
    | OPEN_BRACE MAX COLON STRING CLOSE_BRACE  { $$ = AggregationFunctionSemanticAction("MAX", $4); }
    ;

aggregation:
    AGGREGATION COLON OPEN_BRACE
        GROUP_BY COLON OPEN_BRACKET attributes_list CLOSE_BRACKET COMMA
        AGGREGATIONS COLON OPEN_BRACKET aggregation_list CLOSE_BRACKET COMMA
        INPUT COLON input
    CLOSE_BRACE
    {
        $$ = AggregationSemanticAction($6, $10, $14);
    };

condition: 
	AND COLON OPEN_BRACKET condition COMMA condition CLOSE_BRACKET	{ $$ = BinaryConditionSemanticAction($4, $6, "AND"); }
	| OR COLON OPEN_BRACKET condition COMMA condition CLOSE_BRACKET			{ $$ = BinaryConditionSemanticAction($4, $6, "OR"); }
	| NOT COLON OPEN_BRACKET condition CLOSE_BRACKET							{ $$ = UnaryConditionSemanticAction($4); }
	| comparison { $$ = $1; }

	;

comparison:
	 EQUAL COLON OPEN_BRACKET value COMMA value CLOSE_BRACKET	 { $$ = ComparisonConditionSemanticAction($4, "=", $6); }
	| LOWER COLON OPEN_BRACKET value COMMA value CLOSE_BRACKET	 { $$ = ComparisonConditionSemanticAction($4, "<", $6); }
	| HIGHER COLON OPEN_BRACKET value COMMA value CLOSE_BRACKET	 { $$ = ComparisonConditionSemanticAction($4, ">", $6); }
	| LOWER_EQUAL COLON OPEN_BRACKET value COMMA value CLOSE_BRACKET { $$ = ComparisonConditionSemanticAction($4, "<=", $6); }
	| HIGHER_EQUAL COLON OPEN_BRACKET value COMMA value CLOSE_BRACKET { $$ = ComparisonConditionSemanticAction($4, ">=", $6); }
	| NOT_EQUAL COLON OPEN_BRACKET value COMMA value CLOSE_BRACKET { $$ = ComparisonConditionSemanticAction($4, "!=", $6); }

;

relation:
  JOINTOKEN COLON OPEN_BRACE condition COMMA side_input COMMA side_input CLOSE_BRACE
                                                                 { $$ = BinaryExpressionSemanticAction(JOIN, $6, $8, $4 ); }
  | CARTESIAN_PRODUCT COLON OPEN_BRACE side_input COMMA side_input CLOSE_BRACE
                                                                 { $$ = BinaryExpressionSemanticAction(PRODUCT, $4, $6, NULL ); }
  | UN COLON OPEN_BRACE side_input COMMA side_input CLOSE_BRACE  { $$ = BinaryExpressionSemanticAction(UNION, $4, $6, NULL ); }
  | INTER COLON OPEN_BRACE side_input COMMA side_input CLOSE_BRACE
                                                                 { $$ = BinaryExpressionSemanticAction(INTERSECTION, $4, $6, NULL); }
  | DIF COLON OPEN_BRACE side_input COMMA side_input CLOSE_BRACE { $$ = BinaryExpressionSemanticAction(DIFF, $4, $6, NULL); }
;


attributes_param:
    ATTRIBUTES COLON OPEN_BRACKET attributes_list CLOSE_BRACKET { $$ = $4; }
;

attributes_list:
      attributes_item                           { $$ = $1; }
    | attributes_item COMMA attributes_list     { $1->next = $3; $$ = $1; }
;

attributes_item:
      STRING                                    { $$ = AtributeSemanticAction($1, NULL); }
;

value:
	STRING { $$ = $1; }
    | INTEGER {
        char buf[32];
        snprintf(buf, sizeof buf, "%d", $1);
        $$ = strdup(buf);
      }
    ;
	
constant: INTEGER										{ $$ = IntegerConstantSemanticAction($1); }
	;


%%
