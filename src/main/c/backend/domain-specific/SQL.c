#include "SQL.h"


Logger * _logger=NULL;


bool validateAttsExpressionAndOrder(Order* order);
bool expressionHasProjection(Expression* expression);
bool attributesExist(Attributes * orderAttributes, Attributes* projectionAttributes);
Attributes* getProjectionAttributes(Expression * expression);
bool validateExpression(Expression* expression);
bool validateAttributes(Attributes *attributes);
bool validateAggregation(Aggregation *aggregation);
bool validateCondition(Condition* condition);
bool validateAttsBinary(Expression * expression1,Expression * expression2);
bool validateAttsSelection(Expression * expression, Condition * condition);
void getSelectAtts(Condition * condition,Attributes ** out);
Attributes* getGroupByAttributes(Expression * expression);
bool expressionHasAggregation(Expression* expression);




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
        
    case AGGREGATION:

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
    case DIFFERENCE:
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

    Attributes* attribute = order->attributes;
    Directions* direction = order->directions;

    while (attribute && direction) {
        attribute = attribute->next;
        direction = direction->next;
    }
    if (attribute || direction){
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

bool attributesEqual(Attributes * attributes1, Attributes * attributes2){
    Attributes *attribute1 = attributes1;
    Attributes *attribute2 = attributes2;

    while (attribute1 != NULL && attribute2 != NULL) {
        if (strcmp(attribute1->value, attribute2->value) != 0)
            return false;

        attribute1 = attribute1->next;
        attribute2 = attribute2->next;
    }

    return attribute1 == NULL && attribute2 == NULL;
}

bool validateAttsBinary(Expression * expression1,Expression * expression2){
    Attributes* attributes1= getProjectionAttributes(expression1);
    Attributes* attributes2= getProjectionAttributes(expression2);
    if((attributes1 == NULL && attributes2 != NULL) || (attributes1 != NULL && attributes2 == NULL)){
        return false;
    }
    if (!attributesEqual(attributes1, attributes2)) {
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
            Attributes *leftAttribute= getProjectionAttributes(expression->join.left);
            if(leftAttribute!=NULL ){
                return leftAttribute;
            }
		    return getProjectionAttributes(expression->join.right);
        }
		case UNION:
		case INTERSECTION:
		case DIFFERENCE:
        case PRODUCT:{
			Attributes *leftAttribute= getProjectionAttributes(expression->binary.left);
            if(leftAttribute!=NULL ){
                return leftAttribute;
            }
		    return getProjectionAttributes(expression->binary.right);
        }
        case AGGREGATION: 
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
            Attributes *leftAttribute= getProjectionAttributes(expression->join.left);
            if(leftAttribute!=NULL ){
                return leftAttribute;
            }
		    return getProjectionAttributes(expression->join.right);
        }
		case UNION:
		case INTERSECTION:
		case DIFFERENCE:
        case PRODUCT:{
			Attributes *leftAttribute= getProjectionAttributes(expression->binary.left);
            if(leftAttribute!=NULL ){
                return leftAttribute;
            }
		    return getProjectionAttributes(expression->binary.right);
        }
        case AGGREGATION: 
                return expression->aggregation.group_by;
        default:
            return NULL;
    }
}

bool attributesExist(Attributes * orderAttributes, Attributes* projectionAttributes){
    for (Attributes *orderAttribute = orderAttributes; orderAttribute != NULL; orderAttribute = orderAttribute->next) {
        bool found = false;

        // Buscar o->value dentro de la lista projAtts
        for (Attributes *projectionAttribute = projectionAttributes; projectionAttribute != NULL; projectionAttribute = projectionAttribute->next) {
            if (strcmp(orderAttribute->value, projectionAttribute->value) == 0) {
                found = true;
                break;
            }
        }

    if (!found) return false;
    }

    return true;
}



bool expressionHasProjection(Expression* expression) {
    if (expression) {
        
        switch (expression->type) {
            case PROJECTION: return true;
            case SELECTION: return expressionHasProjection(expression->selection.input); 
            case BASE_TABLE: return false;
            case JOIN: 
                return expressionHasProjection(expression->join.left)
                    || expressionHasProjection(expression->join.right);
		case UNION:
		case INTERSECTION:
		case DIFFERENCE:
        case PRODUCT:
                return expressionHasProjection(expression->binary.left)
                    || expressionHasProjection(expression->binary.right);
        }
    }
    return false;
}

bool expressionHasAggregation(Expression* expression) {
    if (expression) {
        
        switch (expression->type) {
            case PROJECTION: return expressionHasAggregation(expression->projection.input);
            case SELECTION: return expressionHasAggregation(expression->selection.input); 
            case BASE_TABLE: return false;
            case JOIN: 
                return expressionHasAggregation(expression->join.left)
                    || expressionHasAggregation(expression->join.right);
		case UNION:
		case INTERSECTION:
		case DIFFERENCE:
        case PRODUCT:
                return expressionHasAggregation(expression->binary.left)
                    || expressionHasAggregation(expression->binary.right);
        case AGGREGATION:
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
        if (condition->unary.expression==NULL)
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
void destroySelectAtts(Attributes *attributes) {
    while (attributes != NULL) {
        Attributes *next = attributes->next;  
        free(attributes);
        attributes = next;
    }
}
bool validateAttsSelection(Expression * expression, Condition * condition){
    if (!expressionHasProjection(expression) && !expressionHasAggregation(expression)) {
        return true;
    }

    Attributes* projected= getProjectionAttributes(expression);
    Attributes* groupBy= getGroupByAttributes(expression);
    Attributes* selectionAttributes=NULL;
    getSelectAtts(condition, &selectionAttributes);
    

    if (!attributesExist(selectionAttributes, projected) && !attributesExist(selectionAttributes, groupBy) ) {
        logError(_logger, "Attributes in select must be projected or in the group by clause for Aggregation");
        destroySelectAtts(selectionAttributes);
        return false;
    }
    destroySelectAtts(selectionAttributes);
    return true;
}

bool validateAggregation(Aggregation *aggregation){
    logDebugging(_logger, "Validating AGGREGATION");

    while (aggregation!= NULL)
    {
        if (aggregation->function==NULL || !validateAttributes(aggregation->attribute)){
            logError(_logger, "Agreggation function is null or  invalid.");

            return false;
        }
        aggregation= aggregation->next;
    }
    return true;
    
}


bool validateAttributes(Attributes *attributes){
     logDebugging(_logger, "Validating ATTRIBUTES");

    Attributes *attribute = attributes;
      if (attributes ==NULL){
         logError(_logger, "ATTRIBUTES are null");

        return false;
        }
    while (attribute!=NULL)
    {
        if(attribute->value == NULL){
            logError(_logger, " Attribute value is null");

            return false;
        }
        attribute = attribute->next;

    }
    return true;

}

bool isQuotedString(const char *string) {
    int length = strlen(string);
    if (length < 4) return false;
    return ( ( string[0] == '\'') &&
             (string[length - 1] == '\'') );
}
bool isNumber(const char *string) {
    if (*string == '\0') return false;

    if (*string == '-' ) string++;

    bool has_digit = false;

    while (*string) {
        if (*string >= '0' && *string <= '9') {
            has_digit = true;
        } else {
            return false;
        }
        string++;
    }
    return has_digit;
}

bool isColumn(const char *operand) {
    return !isNumber(operand) && !isQuotedString(operand);
}

Attributes* AttributesList(char *next, Attributes *list) {
    Attributes *attributes = calloc(1, sizeof(Attributes));
    attributes->value = next;
    attributes->next = list;
    return attributes;
}

void getSelectAtts(Condition * condition,Attributes ** out) {
    if (condition == NULL) return;

    switch (condition->type) {
        case COMPARISON:
            if (isColumn(condition->comparison.leftOperand))
                 *out= AttributesList(condition->comparison.leftOperand, *out);

            if (isColumn(condition->comparison.rightOperand))
                *out= AttributesList(condition->comparison.rightOperand, *out);
            break;

        case BINARY:
            getSelectAtts(condition->binary.left, out);
            getSelectAtts(condition->binary.right, out);
            break;

        case UNARY:
            getSelectAtts(condition->unary.expression, out);
            break;
    }
}


