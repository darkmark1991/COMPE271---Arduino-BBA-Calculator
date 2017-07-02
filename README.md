# COMPE271 - Arduino Binary Boolean Algebra Calculator

Description:
The project implements a calculator that will do boolean algebraic calculations on binary
numbers. The user will enter the two operands as binary numbers (max. 8 digits) and an operator
(NOT, AND, OR, XOR, NAND, NOR, XNOR, ->, EQV) and output the result.
The setup outputs instructions to Arduino’s serial monitor and prompts the user to supply input.
The input is read as a c string through serial monitor. It is uppercased and printed to screen.
ParseCommand function parses the input string. It splits the input into chunks and uses
MapOperator function to set operator to its corresponding numerical code, as defined by
BinaryOperator enum. BinaryStr2Int is used to check if the binary number is indeed binary and
to read its string representation into an integer. If at any of these stages something goes wrong
(invalid operand or operator is supplied) the calculation will print an error message.
Once everything is represented by integers the rest is handled by inline assembly.
AssemblyBinCalc implements a switch/case statement by using conditional branching. It
compares supplied operator code to the indexes defined in BinaryOperator enum and executes
the corresponding chunks of assembly code. In the end the result is written to the output
argument and returned from the function.
AssemblyBinMask is used to create a mask that trims the supplied number to the length of the
longest binary argument. This makes the output of the program look better. Because while "NOT
1 = 11111110” is factually correct, as we're working with 8-bit integers and 1 is represented as
00000001, "NOT 1 = 0" looks much better for our purposes.
After printing the result, or error, the program waits for the next instruction.
Hardware: The necessary hardware to run this code is Arduino UNO.
Software: Arduino IDE.

Pseudocode:

ENUM BinaryOperator{NOT, AND, OR, XOR, NAND, NOR, XNOR, IMP}

MapOperator(opStr, opCode)
  SWITCH opStr:
    case ‘and’ : opCode = AND
    case ‘or’ : opCode = OR
    case ‘xor’ : opCode = XOR
    etc…
    default : RETURN ERROR
  RETURN OK
  
BinaryStr2Int(str, num)
  IF str contains something other than 1s and 0s RETURN ERROR
  num = strtol(str)
  RETURN OK
  
ParseCommand (op, argArr, input)
  SPLIT input > tmp[0], tmp[1], tmp[2]
  IF argCount == 2
    MapOperator(tmp[0], op)
    BinaryStr2Int(tmp[1], argArr[0])
    argArr[1] = argArr[0] //copy the same arg into argArr[1] as well
  IF argCount == 3
    MapOperator(tmp[1], op)
    BinaryStr2Int(tmp[0], argArr[1])
    BinaryStr2Int(tmp[2], argArr[2])
  ELSE
    RETURN ERROR
  RETURN OK

AssemblyBinCalc(op, a1, a2)
  Res = ERROR;
  asm(
    TEST op
    IF 0 branch to NOT
    CMP op, 1
    IF == branch to AND
    CMP op, 2
    IF == branch to OR
    ………ETC………
    JUMP to EXIT
    NOT: Complement a1
    JUMP to END
    AND: a1 = a1 AND a2
    JUMP to END
    OR: a1 = a1 OR a2
    JUMP to END
    ………ETC………
    END: MOV res, a1
    EXIT
  )
  RETURN res
AssemblyBinMask(res, a1, a2)
  asm(
    MAX = a1
    CMP a1, a2
    IF a1>=a2 branch to A1
    MAX = a2
    A1:
    MAX |= MAX >> 1;
    MAX |= MAX >> 2;
    MAX |= MAX >> 4;
    AND res, MAX
  )

SETUP
  INITIALIZE Arduino Serial Monitor
  PROMPT user to input command // ex. 1111 XOR 1011
  
LOOP
  IF input
    ParseCommand (op, argArr, input)
    res = AssemblyBinCalc(op, argArr)
    res = AssemblyBinMask(res, argArr)
    PRINT to Serial Monitor “UpperCase(input) = binary(res)”


Testing and development:
Bug: When one input is 8-digit binary number and second one is not the result gets truncated.
This is caused by the rounding to nearest higher 2’s power, because 8-bit register overflows.
Fix: I fixed the bug by skipping the masking if either argument has the highest bit set.
Future: I’m planning to implement some sort of 0 padding in the future, so that 1111 AND 0001
will display in 0001 and not 1.

Estimated time spent: 40~45 hrs.
