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
Program * expressionProgramSemanticAction(Expression * expression);


Constant * integerConstantSemanticAction(const int value);
Expression * factorExpressionSemanticAction(Factor * factor);
Factor * constantFactorSemanticAction(Constant * constant);
Factor * expressionFactorSemanticAction(Expression * expression);

Condition *binaryConditionSemanticAction(Condition *left, Condition *right, const char *operator);
Condition *unaryConditionSemanticAction(Condition *expression);
Condition *comparisonConditionSemanticAction(char *left, char *operator, char *right);



Expression *binaryRelationSemanticAction(ExpressionType type, Relation *left, Relation *right, Condition * condition);
Expression *baseExpressionSemanticAction(char *tableName);

Expression * selectionSemanticAction(Condition *condition, Expression *input);
Expression * renameSemanticAction(Expression *input, char *newName);
Expression * projectionSemanticAction(Expression *input, Attributes *attributes);

Expression * baseRelationSemanticAction(char *tableName);
Expression * binaryExpressionSemanticAction(ExpressionType type, Expression *left, Expression *right,Condition *condition);

Expression * aggregationSemanticAction(Attributes *group_by, Aggregation *aggregations, Expression *input);
Aggregation * aggregationFunctionSemanticAction(char *function, char *attribute);

Attributes * atributeSemanticAction(char * next, Attributes * list);
Attributes *attributesPrepend(Attributes *item, Attributes *list);
Order * orderSemanticAction(Attributes * attributes, Directions* direction, Expression * input);
Program * orderProgramSemanticAction(Order * order);

AggregationField *makeAggregationField(AggregationFieldKind kind, void *value);
AggregationFieldList *appendAggregationField(AggregationFieldList *list, AggregationField *field);
Expression *buildAggregationFromFields(AggregationFieldList *list);

char * integerSemanticAction(int i);

Directions * directionsSemanticAction(DirectionType next, Directions * tail);

#endif
