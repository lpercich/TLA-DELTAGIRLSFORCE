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

Condition *BinaryConditionSemanticAction(Condition *left, Condition *right, const char *operand);
Condition *UnaryConditionSemanticAction(Condition *expression);
Condition *ComparisonConditionSemanticAction(char *left, char *operand, char *right);



Expression *BinaryRelationSemanticAction(ExpressionType type, Relation *left, Relation *right, Condition * condition);
Expression *BaseExpressionSemanticAction(char *tableName);

Expression * SelectionSemanticAction(Condition *condition, Expression *input);
Expression * RenameSemanticAction(Expression *input, char *newName);
Expression * ProjectionSemanticAction(Expression *input, Attributes *attributes);

Expression * BaseRelationSemanticAction(char *tableName);
Expression * BinaryExpressionSemanticAction(ExpressionType type, Expression *left, Expression *right,Condition *condition);

Expression * AggregationSemanticAction(Attributes *group_by, Aggregation *aggregations, Expression *input);
Aggregation * AggregationFunctionSemanticAction(char *function, char *attribute);

Attributes * AtributeSemanticAction(char * next, Attributes * list);
Attributes *AttributesPrepend(Attributes *item, Attributes *list);
Order * OrderSemanticAction(Attributes * attributes, Directions* direction, Expression * input);
Program * OrderProgramSemanticAction(Order * order);

char * IntegerSemanticAction(int i);

Directions * DirectionsSemanticAction(DirectionType next, Directions * tail);

#endif
