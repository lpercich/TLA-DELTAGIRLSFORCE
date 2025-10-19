#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule();

/**
 * Bison semantic actions.
 */
Program * ExpressionProgramSemanticAction(Expression * expression);


Constant * IntegerConstantSemanticAction(const int value);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);

Condition *BinaryConditionSemanticAction(Condition *left, Condition *right, const char *op);
Condition *UnaryConditionSemanticAction(Condition *expr);
Condition *ComparisonConditionSemanticAction(char *left, char *op, char *right);



Expression *BinaryRelationSemanticAction(ExpressionType type, Relation *left, Relation *right, Condition * condition);
Expression *BaseExpressionSemanticAction(char *tableName);

Expression * SelectionSemanticAction(Condition *condition, Expression *input);
Expression * RenameSemanticAction(Expression *input, char *newName);
Expression * ProjectionSemanticAction(Expression *input, Attributes *atts);

Expression * BaseRelationSemanticAction(char *tableName);
Expression * BinaryExpressionSemanticAction(ExpressionType type, Expression *left, Expression *right,Condition *cond);

Expression * AggregationSemanticAction(Attributes *group_by, Aggregation *aggs, Expression *input);
Aggregation * AggregationFunctionSemanticAction(char *func, char *attribute);

Attributes * AtributeSemanticAction(char * next, Attributes * list);
Order * OrderSemanticAction(Attributes * atts, Orders* orders, Expression * input);

Program * ExpressionProgramSemanticAction(Expression * relation);
#endif
