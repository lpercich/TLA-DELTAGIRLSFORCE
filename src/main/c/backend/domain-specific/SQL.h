#ifndef __SQL__
#define __SQL__

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
 


void initializeSqlModule();

void shutdownSqlModule();

bool validateProgram(Program *program);

bool validateExpression(Expression* expression);

bool validateOrder(Order* order);


#endif