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
	BINARY, UNARY, COMPARISON
};

enum ExpressionType{
	SELECTION,
	PROJECTION,
	RHO,
    BASETABLE
	
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
		Condition * condition;
	};
	FactorType type;
};

struct Expression {
    ExpressionType type;

    union {
        struct { // SELECTION
            struct Expression *input;
            Condition *condition;
        } selection;

        struct { // PROJECTION
            struct Expression *input;
            char **attributes;
            size_t attrCount;
        } projection;

        struct { // RENAMING
            struct Expression *input;
            char *newName;
        } renaming;

        struct { // Entrada base
            Relation *relation;
        } base;
    };
};


//queda para preguntar xq con la correccion alfinal no me queda claro???
struct Condition {
    ConditionType type;

    union {
        struct { // Comparación simple: columna operador valor
            char *leftOperand;   // ej: "edad"
            char *operator;      // ej: ">"
            char *rightOperand;  // ej: "18"
        } comparison;

        struct { // Binaria: AND / OR
            Condition *left;
            Condition *right;
            char *operator;     // "AND" o "OR"
        } binary;

        struct { // Unaria: solo NOT
            Condition *expr;
        } unary;
    };
};

 struct Relation {
    RelationType type;

    union {
        struct { // Tabla base
            char *tableName;
        } base;

        struct { // Operaciones binarias: JOIN, UNION, etc.
            Relation *left;
            Relation *right;
        } binary;
    };
};


struct Program {
	Expression * expression;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyCondition(Condition *condition);
void destroyRelation(Relation *relation);
void destroyExpression(Expression *expression);
void destroyProgram(Program *program);
#endif
