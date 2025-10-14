#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum ConditionType ConditionType;
typedef enum RelationType RelationType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Condition Condition;
typedef struct Factor Factor;
typedef struct Relation Relation;
typedef struct Program Program;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ConditionType {
	AND,
    OR,
    NOT,
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL
};

enum ExpressionType{
	SELECTION,
	PROJECTION,
	RHO
	
};

enum RelationType{
	BASE_TABLE,
	JOIN,
	UNION,
	INTERSECTION,
	DIFF
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};

enum OrderByType
{
	ASC,
	DESC
};


struct Factor {
	union {
		Constant * constant;
		Expression * expression;
		Condition * condition
	};
	FactorType type;
};

 struct Expression {
    union {
        struct {                // expresión aritmética
            struct Expression *leftExpression;
            struct Expression *rightExpression;
        };
        struct {                // expresión simple: columna o constante
            char *columnName;   // nombre de la columna
            int constant;       // valor constante
        };
    };
    ExpressionType type;        // ADD, SUB, MUL, DIV, COLUMN, CONSTANT
};


struct Condition {
	union {
		Factor * factor; //no c para q usamos esto!!!!
		struct {
			Condition * leftCondition;
			Condition * rightCondition;
		};
	};
	ConditionType type;
};

 struct Relation {
    union {
        struct {                // relación derivada
            struct Relation *leftRelation;
            struct Relation *rightRelation;
        };
        struct {                // relación base o operación unaria
            char *tableName;    // para BASE_TABLE
            struct Condition *condition; // para SELECTION
            struct Expression *projectionList; // para PROJECTION
        };
    };
    RelationType type;          // BASE_TABLE, SELECTION, PROJECTION, JOIN, UNION, ...
} Relation;


struct Program {
	Expression * expression;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);

#endif
