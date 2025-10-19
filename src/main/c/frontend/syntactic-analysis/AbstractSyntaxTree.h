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
typedef struct Attributes Attributes;
typedef struct Aggregation Aggregation;
typedef struct Order Order;

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
    AGGR,
    BASE_TABLE_EXP


};

enum RelationType{
	BASE_TABLE,
	JOIN,
	UNION,
	INTERSECTION,
	DIFF,
    PRODUCT
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

struct Attributes{
    char * value;
    Attributes * next;
};
typedef struct Aggregation {
    char *function;  
    Attributes *attribute;  
    struct Aggregation *next;
} Aggregation;

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
            Attributes *attributes;
        } projection;

        struct { // RENAMING
            struct Expression *input;
            char *newName;
        } renaming;
        struct { // AGGREGATION
        
        struct Expression *input;
            Attributes *group_by;
            Aggregation *aggregations;
        } aggregation;

        struct { // Entrada base
            char* tableName;
        } base;


        struct 
        {
           struct Expression *left;
            struct Expression *right;
        } binary;


        struct 
        {
             struct Expression *left;
            struct Expression *right;
             Condition *condition;
        }join;
        

     
      
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
            Condition * condition; //para el on del JOIN
        } binary;
    };
};


struct Program {
	Expression * expression;
    Relation * relation;
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
