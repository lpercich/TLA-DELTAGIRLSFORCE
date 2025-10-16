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

Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Condition *BinaryConditionSemanticAction(Condition *left, Condition *right, const char *op);
Condition *UnaryConditionSemanticAction(Condition *expr);
Program * ExpressionProgramSemanticAction(Expression * expression);
Relation *BinaryRelationSemanticAction(RelationType type, Relation *left, Relation *right, Condition * condition);
Relation *BaseRelationSemanticAction(char *tableName);
#endif
