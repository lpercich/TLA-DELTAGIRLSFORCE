#ifndef __SQL__
#define __SQL__

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"


void initializeSqlModule();
void shutdownSqlModule();

boolean validateSql(Program *program);
boolean validateExpression(Expression* expression);
boolean validateOrder(Order* order);


#endif