#include "SQL.h"


Logger * _logger=NULL;


bool validateAttsExpressionAndOrder(Order* order);
bool expressionHasProjection(Expression* expression);
bool attributesExist(Attributes * orderAtts, Attributes* projAtts);
Attributes* getProjectionAttributes(Expression * expression);
bool validateExpression(Expression* expression);
bool validateAttributes(Attributes *attributes);
bool validateAggregation(Aggregation *aggregation);
bool validateCondition(Condition* condition);
bool validateAttsBinary(Expression * e1,Expression * e2);
bool validateAttsSelection(Expression * expr, Condition * cond);
void getSelectAtts(Condition * cond,Attributes ** out);
Attributes* getGroupByAttributes(Expression * expression);
bool expressionHasAggregation(Expression* e);




void initializeSqlModule()
{
    _logger = createLogger("Sql");
    logDebugging(_logger, "Start Logger");
}

void shutdownSqlModule(){
    if (_logger!=NULL){
        logDebugging(_logger,"Destroy logger");
        destroyLogger(_logger); //preguta si es != a null y lo destruye
        //Deberiamos dejarlo en NULL? TIPO _logger=NULL?
    }
        
}

bool validateProgram(Program *program){
    if (!program){
         logError(_logger, "Program is NULL");
         return false;
    }
    if(program->expression && program->order){
        logError(_logger, "Program has expression and orden");

        return false;

    }else if (program->expression){
        return validateExpression(program->expression);

    }else if (program->order){
        return validateOrder(program->order);
    }
    logError(_logger, "program doesnt have expression nor order");
    return false;

}

bool validateExpression(Expression* expression){
    logDebugging(_logger, "Validating expression");

    if (expression == NULL){
        logError(_logger, "Expression is null");
        return false;
    }


    switch (expression->type)
    {
    case SELECTION:
    //FALTA CHEQUEO DE SI HAY UN PROJECTION QUE LOS ATTS DE PROJECTION COINCIDAN CON LOS DE SELECTION
        logDebugging(_logger, "Validating SELECTION");
        return validateCondition(expression->selection.condition) 
        && (validateExpression(expression->selection.input))
        && validateAttsSelection(expression->selection.input, expression->selection.condition);
            
       
        
    case PROJECTION:
        logDebugging(_logger, "Validating PROJECTION");

        return validateAttributes(expression->projection.attributes) && 
        validateExpression(expression->projection.input);
        

       
    case RHO:
        logDebugging(_logger, "Validating RENAMING");
    
        if (expression->renaming.newName == NULL )
        {
            logError(_logger, "Alias for RENAMING is null");
            return false;
        }
        return validateExpression(expression->renaming.input);
        
    case AGGR:

        if(!validateAttributes(expression->aggregation.group_by)){
             logError(_logger, "Invalid attributes for AGGREGATION");
            return false;
        }
        return validateAggregation(expression->aggregation.aggregations)
        && validateExpression(expression->aggregation.input)
        && !(expression->aggregation.aggregations == NULL && expression->aggregation.group_by == NULL) ;
        
    case JOIN:
            logDebugging(_logger, "Validating JOIN");

            return validateCondition(expression->join.condition) 
            && validateExpression(expression->join.left) 
            && validateExpression(expression->join.right);
    case UNION:
        logDebugging(_logger, "Validating UNION");
         return (
        validateExpression(expression->binary.left) && 
        validateExpression(expression->binary.right)
    && validateAttsBinary(expression->binary.left, expression->binary.right));
    case INTERSECTION:
        logDebugging(_logger, "Validating INTERSECTION");
         return (
        validateExpression(expression->binary.left) && 
        validateExpression(expression->binary.right)
    && validateAttsBinary(expression->binary.left, expression->binary.right));
    case DIFF:
        logDebugging(_logger, "Validating DIFFERENCE");

        return (
        validateExpression(expression->binary.left) && 
        validateExpression(expression->binary.right)
    && validateAttsBinary(expression->binary.left, expression->binary.right));
    case BASE_TABLE:
        logDebugging(_logger, "Validating TABLE");

        if( expression->base.tableName==NULL){
            logError(_logger, "No name for TABLE");
            return false;
        }
        return true;
    case PRODUCT:
        logDebugging(_logger, "Validating PRODUCT");
        return validateExpression(expression->binary.left) && validateExpression(expression->binary.right)
        && validateAttsBinary(expression->binary.left, expression->binary.right);

    default: 
        logError(_logger, "Expression has no type");
        return false;
    }
}

bool validateOrder(Order* order){
     logDebugging(_logger, "Validating ORDER");

    Attributes* a = order->attributes;
    Directions* d = order->directions;

    while (a && d) {
        a = a->next;
        d = d->next;
    }
    if (a || d){
        logError(_logger, "Number of attributes and directions for ORDER does not match");
        return false;
    }

    if (!validateExpression(order->input))
        return false;


    if (!validateAttsExpressionAndOrder(order)) {
        return false;
    }
    return true;
 
}

bool validateAttsExpressionAndOrder(Order* order){
    if (!order) return true;

    if (!expressionHasProjection(order->input) && !expressionHasAggregation(order->input)) {
        return true;
    }

    Attributes* projected= getProjectionAttributes(order->input);
    Attributes* groupBy= getGroupByAttributes(order->input);


    if (!attributesExist(order->attributes, projected) && !attributesExist(order->attributes, groupBy)) {
        logError(_logger, "Attributes in ORDER must be in PROJECTION or in AGGREGATION group by");
        return false;
    }

    return true;

}

bool attributesEqual(Attributes * a1, Attributes * a2){
    Attributes *x = a1;
    Attributes *y = a2;

    while (x != NULL && y != NULL) {
        if (strcmp(x->value, y->value) != 0)
            return false;

        x = x->next;
        y = y->next;
    }

    return x == NULL && y == NULL;
}

bool validateAttsBinary(Expression * e1,Expression * e2){
    Attributes* p1= getProjectionAttributes(e1);
    Attributes* p2= getProjectionAttributes(e2);
    if((p1 == NULL && p2 != NULL) || (p1 != NULL && p2 == NULL)){
        return false;
    }
    if (!attributesEqual(p1, p2)) {
        logError(_logger, "Attributes in binary relation must be projected in both subexpressions");
        return false;
    }

    return true;
}

Attributes* getProjectionAttributes(Expression * expression){
    if (!expression){
        return NULL;
    }
    switch (expression->type) {


        case SELECTION:
            return getProjectionAttributes(expression->selection.input);

        case PROJECTION:
            return expression->projection.attributes;
        case RHO:
            return getProjectionAttributes(expression->renaming.input);
        case BASE_TABLE:
            return NULL;
        case JOIN: {
            Attributes *leftAttr= getProjectionAttributes(expression->join.left);
            if(leftAttr!=NULL ){
                return leftAttr;
            }
		    return getProjectionAttributes(expression->join.right);
        }
		case UNION:
		case INTERSECTION:
		case DIFF:
        case PRODUCT:{
			Attributes *leftAttr= getProjectionAttributes(expression->binary.left);
            if(leftAttr!=NULL ){
                return leftAttr;
            }
		    return getProjectionAttributes(expression->binary.right);
        }
        case AGGR: 
                return getProjectionAttributes(expression->aggregation.input);
        default:
            return NULL;
    }
}

Attributes* getGroupByAttributes(Expression * expression){
     if (!expression){
        return NULL;
    }
    switch (expression->type) {


        case SELECTION:
            return getProjectionAttributes(expression->selection.input);

        case PROJECTION:
            return getProjectionAttributes(expression->projection.input);
        case RHO:
            return getProjectionAttributes(expression->renaming.input);
        case BASE_TABLE:
            return NULL;
        case JOIN: {
            Attributes *leftAttr= getProjectionAttributes(expression->join.left);
            if(leftAttr!=NULL ){
                return leftAttr;
            }
		    return getProjectionAttributes(expression->join.right);
        }
		case UNION:
		case INTERSECTION:
		case DIFF:
        case PRODUCT:{
			Attributes *leftAttr= getProjectionAttributes(expression->binary.left);
            if(leftAttr!=NULL ){
                return leftAttr;
            }
		    return getProjectionAttributes(expression->binary.right);
        }
        case AGGR: 
                return expression->aggregation.group_by;
        default:
            return NULL;
    }
}

bool attributesExist(Attributes * orderAtts, Attributes* projAtts){
    for (Attributes *o = orderAtts; o != NULL; o = o->next) {
        bool found = false;

        // Buscar o->value dentro de la lista projAtts
        for (Attributes *p = projAtts; p != NULL; p = p->next) {
            if (strcmp(o->value, p->value) == 0) {
                found = true;
                break;
            }
        }

    if (!found) return false;
    }

    return true;
}



bool expressionHasProjection(Expression* e) {
    if (e) {
        
        switch (e->type) {
            case PROJECTION: return true;
            case SELECTION: return expressionHasProjection(e->selection.input); 
            case BASE_TABLE: return false;
            case JOIN: 
                return expressionHasProjection(e->join.left)
                    || expressionHasProjection(e->join.right);
		case UNION:
		case INTERSECTION:
		case DIFF:
        case PRODUCT:
                return expressionHasProjection(e->binary.left)
                    || expressionHasProjection(e->binary.right);
        }
    }
    return false;
}

bool expressionHasAggregation(Expression* e) {
    if (e) {
        
        switch (e->type) {
            case PROJECTION: return expressionHasAggregation(e->projection.input);
            case SELECTION: return expressionHasAggregation(e->selection.input); 
            case BASE_TABLE: return false;
            case JOIN: 
                return expressionHasAggregation(e->join.left)
                    || expressionHasAggregation(e->join.right);
		case UNION:
		case INTERSECTION:
		case DIFF:
        case PRODUCT:
                return expressionHasAggregation(e->binary.left)
                    || expressionHasAggregation(e->binary.right);
        case AGGR:
         return true;
        }
    }
    return false;
}


// struct Condition {
//     ConditionType type;

//     union {
//         struct { // Comparación simple: columna operador valor
//             char *leftOperand;   // ej: "edad"
//             char *operator;      // ej: ">"
//             char *rightOperand;  // ej: "18"
//         } comparison;

//         struct { // Binaria: AND / OR
//             Condition *left;
//             Condition *right;
//             char *operator;     // "AND" o "OR"
//         } binary;

//         struct { // Unaria: solo NOT
//             Condition *expr;
//         } unary;
//     };
// };


bool validateCondition(Condition* condition){
    logDebugging(_logger, "Validating CONDITION");

    if (condition==NULL)
    {
        
        return false; 
    }
    switch (condition->type)
    {
    case COMPARISON:
     if (condition->comparison.leftOperand == NULL || condition->comparison.rightOperand  == NULL || condition->comparison.operator  == NULL)
     {
        logError(_logger, "COMPARISON is null");
        return false;
     }
     return true;

     case BINARY:   
    if (condition->binary.left == NULL || condition->binary.right  == NULL || condition->binary.operator  == NULL)
    
     {
        logError(_logger, "BINAY CONDITION is null");
        return false;
     }
        return true;

     case UNARY:
        if (condition->unary.expr==NULL)
     {
        logError(_logger, "NOT CONDITION  is null");
        return false;  
    }
        return true;

    default:
        logError(_logger, "CONDITION type is null or not valid");
        return false;
    }

}
void destroySelectAtts(Attributes *attrs) {
    while (attrs != NULL) {
        Attributes *next = attrs->next;  
        free(attrs);
        attrs = next;
    }
}
bool validateAttsSelection(Expression * expr, Condition * cond){
    if (!expressionHasProjection(expr) && !expressionHasAggregation(expr)) {
        return true;
    }

    Attributes* projected= getProjectionAttributes(expr);
    Attributes* groupBy= getGroupByAttributes(expr);
    Attributes* selectionAtts=NULL;
    getSelectAtts(cond, &selectionAtts);
    

    if (!attributesExist(selectionAtts, projected) && !attributesExist(selectionAtts, groupBy) ) {
        logError(_logger, "Attributes in select must be projected or in the group by clause for Aggregation");
        destroySelectAtts(selectionAtts);
        return false;
    }
    destroySelectAtts(selectionAtts);
    return true;
}

bool validateAggregation(Aggregation *aggregation){
    logDebugging(_logger, "Validating AGGREGATION");

    while (aggregation!= NULL)
    {
        if (aggregation->function==NULL || !validateAttributes(aggregation->attribute)){
            logError(_logger, "Agreggation function is null or ");

            return false;
        }
        aggregation= aggregation->next;
    }
    return true;
    
}


bool validateAttributes(Attributes *attributes){
     logDebugging(_logger, "Validating ATTRIBUTES");

    Attributes *a = attributes;
      if (attributes ==NULL){
         logError(_logger, "ATTRIBUTES are null");

        return false;
        }
    while (a!=NULL)
    {
        if(a->value == NULL){
            logError(_logger, " Attribute value is null");

            return false;
        }
        a = a->next;

    }
    return true;

}

bool isQuotedString(const char *s) {
    int len = strlen(s);
    if (len < 4) return false;
    return ( ( s[0] == '\'') &&
             (s[len - 1] == '\'') );
}
bool isNumber(const char *s) {
    if (*s == '\0') return false;

    if (*s == '-' ) s++;

    bool has_digit = false;

    while (*s) {
        if (*s >= '0' && *s <= '9') {
            has_digit = true;
        } else {
            return false;
        }
        s++;
    }
    return has_digit;
}

bool isColumn(const char *operand) {
    return !isNumber(operand) && !isQuotedString(operand);
}

Attributes* AttributesList(char *next, Attributes *list) {
    Attributes *ats = calloc(1, sizeof(Attributes));
    ats->value = next;
    ats->next = list;
    return ats;
}

void getSelectAtts(Condition * cond,Attributes ** out) {
    if (cond == NULL) return;

    switch (cond->type) {
        case COMPARISON:
            if (isColumn(cond->comparison.leftOperand))
                 *out= AttributesList(cond->comparison.leftOperand, *out);

            if (isColumn(cond->comparison.rightOperand))
                *out= AttributesList(cond->comparison.rightOperand, *out);
            break;

        case BINARY:
            getSelectAtts(cond->binary.left, out);
            getSelectAtts(cond->binary.right, out);
            break;

        case UNARY:
            getSelectAtts(cond->unary.expr, out);
            break;
    }
}


