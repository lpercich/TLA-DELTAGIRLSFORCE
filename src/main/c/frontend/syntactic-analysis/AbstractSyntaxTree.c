#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case SELECTION:
			destroyExpression(expression->selection.input);
			destroyCondition(expression->selection.condition);
			case PROJECTION:
			destroyExpression(expression->projection.input);
			destroyAtributes(expression->projection.attributes,expression->projection.attrCount);
			case RHO:
				destroyExpression(expression->renaming.input);
				free(expression->renaming.newName);
				break;
			case BASETABLE:
				destroyRelation(expression->base.relation);
				break;
		}
		free(expression);
	}
}

void destroyCondition(Condition *condition){
	if (condition != NULL) {
		switch (condition->type) {
			case UNARY:
			destroyCondition(condition->unary.expr);
			break;
			case BINARY:
			destroyCondition(condition->binary.left);
			destroyCondition(condition->binary.right);
			free(condition->binary.operator);
			break;
			case COMPARISON:
			free(condition->comparison.leftOperand);
			free(condition->comparison.rightOperand);
			free(condition->comparison.operator);
			break;
		}
		free(condition);
	}
}

void destroyRelation(Relation *relation){
	if (relation != NULL) {
		switch (relation->type) {
			case BASE_TABLE:
			free(relation->base.tableName);
			break;
			case JOIN: 
			case UNION:
			case INTERSECTION:
			case DIFF:
			destroyRelation(relation->binary.left);
			destroyRelation(relation->binary.right);
			break;
		}
		free(relation);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyExpression(program->expression);
		free(program);
	}
}
