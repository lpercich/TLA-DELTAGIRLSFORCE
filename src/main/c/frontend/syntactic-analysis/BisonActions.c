#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Condition *BinaryConditionSemanticAction(Condition *left, Condition *right, const char *operand) {
    Condition *cond = calloc(1, sizeof(Condition));
    cond->type = BINARY;
    cond->binary.left = left;
    cond->binary.right = right;
    cond->binary.operator = strdup(operand);
    return cond;
}

Condition *UnaryConditionSemanticAction(Condition *expression) {
    Condition *condition = calloc(1, sizeof(Condition));
    condition->type = NOT;
    condition->unary.expr = expression;
    return condition;
}

Condition *ComparisonConditionSemanticAction(char *left, char *operand, char *right) {
    Condition *condition = calloc(1, sizeof(Condition));
    condition->type = COMPARISON;

    condition->comparison.leftOperand  = left ;
    condition->comparison.operator     =  strdup(operand);
    condition->comparison.rightOperand = right ;
    return condition;
}

Expression * SelectionSemanticAction( Condition * condition, Expression *input){
	Expression * expression = calloc(1, sizeof(Expression));
	expression->type= SELECTION;
	expression->selection.condition= condition; //preg si hay que reservar espacio ????
	expression->selection.input=input;
	return expression;
}

Expression * ProjectionSemanticAction(Expression *input, Attributes * attributes){
	Expression * expression = calloc(1, sizeof(Expression));
	expression->type= PROJECTION;
	expression->projection.attributes=attributes;
	expression->projection.input=input;
	return expression;
}

Expression * RenameSemanticAction(Expression* input, char * newName){
	Expression * expression = calloc(1, sizeof(Expression));
	expression->type=RHO;
	expression->renaming.input=input;
	 expression->renaming.newName = newName ;
	return expression;
}

/*
Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}*/

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ExpressionProgramSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Program * OrderProgramSemanticAction(Order * order){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->order = order;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}


Expression *BinaryExpressionSemanticAction(ExpressionType type, Expression *left, Expression *right, Condition * condition) {
    Expression *expression = calloc(1, sizeof(Expression));
	expression->type=type;
    switch (type) {
        case JOIN:
			expression->join.left = left;
            expression->join.right = right;
            expression->join.condition = condition;
			break;
        case PRODUCT:
        case UNION:
        case INTERSECTION:
        case DIFF:
            expression->binary.left = left;
            expression->binary.right = right;
			break;
	}
		return expression;
}



Expression *BaseExpressionSemanticAction(char *tableName) {
    Expression *expression = calloc(1, sizeof(Expression));
    expression->type = BASE_TABLE;
    expression->base.tableName = tableName ;
    return expression;
}

Attributes * AtributeSemanticAction(char * next, Attributes * list){
	logDebugging(_logger, "Adding %s to attributes list", next);
		Attributes * attributes= calloc(1, sizeof(Attributes));
		 attributes->value = next;                       
		 attributes->next = list; 
		return attributes;	
}

Attributes *AttributesPrepend(Attributes *item, Attributes *list) {
	logDebugging(_logger, "Adding %s to attributes list", item->value);
    if (item == NULL) return list;
    item->next = list;
    return item;
}

Expression * AggregationSemanticAction(Attributes *group_by, Aggregation *aggregations, Expression *input) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression *expression = calloc(1, sizeof(Expression));
  	expression->type = AGGR;
    expression->aggregation.input = input;
    expression->aggregation.group_by = group_by;
    expression->aggregation.aggregations = aggregations;
    return expression;
}

Aggregation * AggregationFunctionSemanticAction(char *function, char *attribute) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Aggregation *aggregation = calloc(1, sizeof(Aggregation));
    aggregation->function = function ? strdup(function) : NULL;
	if(attribute!=NULL){
		Attributes *attributes=calloc(1,sizeof(Attributes));
		attributes->value=attribute;
		attributes->next=NULL;

		aggregation->attribute=attributes;

	}
	else{
    	aggregation->attribute = NULL;
	}
	aggregation->next=NULL;
    return aggregation;
}

Order * OrderSemanticAction(Attributes * attributes, Directions* directions, Expression * input){
	Order * order = calloc(1, sizeof(Order));
	order->attributes=attributes;
	order->directions=directions;
	order->input=input;
	return order;
}

Directions * DirectionsSemanticAction(DirectionType next, Directions * tail){
		Directions * direction = calloc(1, sizeof(Directions));
		 direction->value = next ;                       
		 direction->next = NULL;
		return direction;	
}


char * IntegerSemanticAction(int integer){
	char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", integer);

    char *result = calloc(1,strlen(buffer) + 1);
    if (!result) {
        exit(1);
    }

    strcpy(result, buffer);
    return result;
}




