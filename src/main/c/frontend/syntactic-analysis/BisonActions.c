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

Condition *BinaryConditionSemanticAction(Condition *left, Condition *right, const char *op) {
    Condition *cond = calloc(1, sizeof(Condition));
    cond->type = BINARY;
    cond->binary.left = left;
    cond->binary.right = right;
    cond->binary.operator = strdup(op);
    return cond;
}

Condition *UnaryConditionSemanticAction(Condition *expr) {
    Condition *cond = calloc(1, sizeof(Condition));
    cond->type = NOT;
    cond->unary.expr = expr;
    return cond;
}

Condition *ComparisonConditionSemanticAction(char *left, char *op, char *right) {
    Condition *cond = calloc(1, sizeof(Condition));
    cond->type = COMPARISON;

    cond->comparison.leftOperand  = left ;
    cond->comparison.operator     =  strdup(op)  ;
    cond->comparison.rightOperand = right ;
    return cond;
}

Expression * SelectionSemanticAction( Condition * condition, Expression *input){
	Expression * exp = calloc(1, sizeof(Expression));
	exp->type= SELECTION;
	exp->selection.condition= condition; //preg si hay que reservar espacio ????
	exp->selection.input=input;
	return exp;
}

Expression * ProjectionSemanticAction(Expression *input, Attributes * atts){
	Expression * exp = calloc(1, sizeof(Expression));
	exp->type= PROJECTION;
	exp->projection.attributes=atts;
	exp->projection.input=input;
	return exp;
}

Expression * RenameSemanticAction(Expression* input, char * newName){
	Expression * exp = calloc(1, sizeof(Expression));
	exp->type=RHO;
	exp->renaming.input=input;
	 exp->renaming.newName = newName ? strdup(newName) : NULL;
	return exp;
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


Expression *BinaryExpressionSemanticAction(ExpressionType type, Expression *left, Expression *right, Condition * cond) {
    Expression *e = calloc(1, sizeof(Expression));
    switch (type) {
        case JOIN:
            e->join.condition = cond;
        case PRODUCT:
        case UNION:
        case INTERSECTION:
        case DIFF:
		  	e->type = type;
            e->binary.left = left;
            e->binary.right = right;
		break;
	}
		return e;
}



Expression *BaseExpressionSemanticAction(char *tableName) {
    Expression *e = calloc(1, sizeof(Expression));
    e->type = BASE_TABLE;
    e->base.tableName = tableName ? strdup(tableName) : NULL;
    return e;
}

Attributes * AtributeSemanticAction(char * next, Attributes * list){
		Attributes * ats= calloc(1, sizeof(Attributes));
		 ats->value = next ? strdup(next) : NULL;                       
		 ats->next = NULL; 
		return ats;	
}

Expression * AggregationSemanticAction(Attributes *group_by, Aggregation *aggs, Expression *input) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression *expr = calloc(1, sizeof(Expression));
  	expr->type = AGGR;
    expr->aggregation.input = input;
    expr->aggregation.group_by = group_by;
    expr->aggregation.aggregations = aggs;
    return expr;
}

Aggregation * AggregationFunctionSemanticAction(char *func, char *attribute) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Aggregation *agg = calloc(1, sizeof(Aggregation));
    agg->function = func ? strdup(func) : NULL;
    agg->attribute = (Attributes *) (attribute ? strdup(attribute): NULL);
    return agg;
}

Order * OrderSemanticAction(Attributes * atts, Directions* directions, Expression * input){
	Order * o = calloc(1, sizeof(Order));
	o->attributes=atts;
	o->directions=directions;
	o->input=input;
	return o;
}

Directions * DirectionsSemanticAction(DirectionType next, Directions * tail){
		Directions * dir= calloc(1, sizeof(Directions));
		 dir->value = next ;                       
		 dir->next = NULL;
		return dir;	
}


char * IntegerSemanticAction(int i){
	char buf[32];  
	snprintf(buf, sizeof buf, "%d", i); 
	return strdup(buf);
}