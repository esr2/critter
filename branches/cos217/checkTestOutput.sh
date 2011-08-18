#!/bin/bash

# This file takes the tested output and compares whether each check's output
# has changed. This is done by greping both outputs for the relevant error
# string. In order to keep this working, always add each check's function name
# and error message in the corresponding locations in each array. Currently 
# this is organized by the order each check appears in the checks.c file.

OLD="output_old.txt"
NEW="output.txt"
tempOLD="temp_output_old.txt"
tempNEW="temp_output.txt"

name=(
 "isFileTooLong"
 "hasBraces"
 "isFunctionTooLongByLines"
 "isFunctionTooLongByStatements"
 "tooManyParameters"
 "neverUseCPlusPlusComments"
 "hasComment"
 "switchHasDefault"
 "switchCasesHaveBreaks"
 "isTooDeeplyNested"
 "useEnumNotConstOrDefine"
 "neverUseGotos"
 "isVariableNameTooShort"
 "isMagicNumber"
 "globalHasComment"
 "isLoopTooLong"
 "isCompoundStatementEmpty"
 "tooManyFunctionsInFile"
 "isIfElsePlacementValid-multipleLines"
 "isIfElsePlacementValid-bracketElseSameLine"
 "isIfElsePlacementValid-noneElseNextLine"
 "isFunctionCommentValid-noComment"
 "isFunctionCommentValid-eachParameter"
 "isFunctionCommentValid-return"
 "isFunctionCommentValid-commentAbove"
 "arePointerParametersValidated"
 "doFunctionsHaveCommonPrefix"
 "functionHasEnoughLocalComments"
 "structFieldsHaveComments"
)


error=(
 "File is too long"
 "Please use braces after all"
 "Function is too long by line count"
 "Function is too long by statement count"
 "function parameters"
 "Do not use C++ style comments"
 "Please include a descriptive comment above each"
 "Always include a default in switch statements"
 "Each case/default in a switch statement should have a break statement"
 "This area is too deeply nested"
 "use enum to define integral constants"
 "Never use GOTO statements"
 "Variable/function name"
 "Do not use magic numbers"
 "Please include a descriptive comment above each global variable"
 "Loop is too long"
 "Do not use empty"
 "There are too many functions in this file"
 "When using braces with an if statement, use multiple lines"
 "Please put the else on the same line as the closing if brace"
 "Please put the else on the line after the if"
 "Please include a descriptive comment above each function"
 "A function's comment should refer to each parameter by name"
 "A function's comment should explicitly state what the function returns"
 "Please put function comments above the function declaration"
 "Do you want to validate"
 "Please prefix all function names with a reasonable module name"
 "This function probably needs more local comments"
 "Please comment all the fields in a struct"
)



for i in "${!error[@]}"
do
	grep "${error[$i]}" $OLD > $tempOLD
	grep "${error[$i]}" $NEW > $tempNEW
	DIFF=`diff $tempOLD $tempNEW | wc -l`
	if [ $DIFF -ne 0 ]; then
		echo "${name[$i]}" "FAILED"
#else
#		echo "${name[$i]}" "PASSED"
	fi
done
