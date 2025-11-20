 #include "Generator.h"

 static Logger * _logger = NULL;

static void _outputSql(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}
void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}
void generate(Program *program) {
    if (program == NULL) {
        logError(_logger, "Program is NULL in generate().");
        return;
    }
    _generateProgram(program);
    _outputSql(";\n");
}

// /** PRIVATE FUNCTIONS */

static void _generateProgram(Program *program);
static void _generateExpression(Expression *expr);
static void _generateSelection(Expression *expr);
static void _generateProjection(Expression *expr);
static void _generateRenaming(Expression *expr);
static void _generateAggregationExpr(Expression *expr);
static void _generateBaseTable(Expression *expr);
static void _generateJoin(Expression *expr);
static void _generateUnion(Expression *expr);
static void _generateIntersection(Expression *expr);
static void _generateDiff(Expression *expr);
static void _generateProduct(Expression *expr);

static void _generateCondition(Condition *cond);
static void _generateComparison(Condition *cond);
static void _generateBinaryCondition(Condition *cond);
static void _generateUnaryCondition(Condition *cond);

static void _generateAttributes(Attributes *attrs);
static void _generateAggregationList(Aggregation *aggr);
static void _generateOrder(Order *order);
static void _generateDirectionsForAttribute(Directions **dirPtr);

static void _generateProgram(Program * program) {
	if(program->expression != NULL) {
		_generateExpression(program->expression);
	} else if(program->order != NULL) {
		_generateOrder(program->order);
	} else {
		logError(_logger, "Program has neither expression nor order.");
	}
}

static void _generateExpression(Expression *expr) {
	if(expr == NULL) {
		logError(_logger, "Expression is null.");
		return;
	}
	switch(expr->type) {
		case SELECTION:
			_generateSelection(expr);
			break;
		case PROJECTION:
			_generateProjection(expr);
			break;
		case RHO:
			_generateRenaming(expr);
			break;
		case AGGR:
			_generateAggregationExpr(expr);
			break;
		case BASE_TABLE:
			_generateBaseTableExpr(expr);
			break;
		case JOIN:
			_generateJoin(expr);
			break;
		case UNION:
			_generateUnion(expr);
			break;
		case INTERSECTION:
			_generateIntersection(expr);
			break;
		case DIFF:
			_generateDiff(expr);
			break;
		case PRODUCT:
			_generateProduct(expr);
			break;
		default:
		logError(_logger, "Unknown expression type: %d.", expr->type);
		break;
	}

}

static void _generateSelection(Expression *expr) {

}

static void _generateProjection(Expression *expr) {

}

static void _generateRenaming(Expression *expr) {

}

static void _generateAggregationExpr(Expression *expr) {

}

static void _generateBaseTable(Expression *expr) {

}

static void _generateJoin(Expression *expr) {

}

static void _generateUnion(Expression *expr) {

}

static void _generateIntersection(Expression *expr) {

}

static void _generateDiff(Expression *expr) {

}

static void _generateProduct(Expression *expr) {

}

static void _generateCondition(Condition *cond) {

}

static void _generateComparison(Condition *cond) {

}

static void _generateBinaryCondition(Condition *cond) {

}

static void _generateUnaryCondition(Condition *cond) {

}

static void _generateAttributes(Attributes *attrs) {

}

static void _generateAggregationList(Aggregation *aggr) {

}

static void _generateOrder(Order *order) {

}

static void _generateDirectionsForAttribute(Directions **dirPtr) {

}












