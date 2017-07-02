/*
 * Project: Arduino Binary Boolean Algebra Calculator
 * Author:  Amiran Ramishvili
 * Red ID:  819817616
 * Course:  COMPE271
 * Date:    2017-04-27
*/
#define __SFR_OFFSET 0
#define ERR       -1
#define OK        0
#define BUFF_SIZE 60
#define ARG_SIZE  16

/* Enum BinaryOperator assigns integer codes to binary operators*/
enum BinaryOperator{NOT, AND, OR, XOR, NAND, NOR, XNOR, IMP};

/* ToUpper() uppercases c string 
   so that we can process both uppercase and lowercase operators */
void ToUpper(char *s){
  for(int i=0; i<strlen(s); i++)
    s[i] = toupper(s[i]);
}

/* MapOperator() assigns integer code corresponding to string s to n
   as defined by BinaryOperator enum */
int MapOperator(char s[], int &n){
  if(strcmp(s, "NOT") == 0)       n = NOT;
  else if(strcmp(s, "AND") == 0)  n = AND;
  else if(strcmp(s, "OR") == 0)   n = OR;
  else if(strcmp(s, "XOR") == 0)  n = XOR;
  else if(strcmp(s, "NAND") == 0) n = NAND;
  else if(strcmp(s, "NOR") == 0)  n = NOR;
  else if(strcmp(s, "XNOR") == 0) n = XNOR;
  else if(strcmp(s, "->") == 0 || strcmp(s, "IMP") == 0) n = IMP; //implication operator, only zero when 1 -> 0
  else if(strcmp(s, "EQV") == 0) n = XNOR; //equivalency operator on two operands has the same truth table as xnor
  else return ERR;
  return OK;
}

/* BinaryStr2Int() checks if a string represents a binary number
   and converts it to integer */
int BinaryStr2Int(char *s, int &n){
  for(int i=0; i<strlen(s); i++)
    if(s[i] != '0' && s[i] != '1' && s[i] != '\n')
      return ERR;
  //strtol (const char* str, char** endptr, int base);
  n = strtol(s, 0, 2); 
  return OK;  
}

/* ParseCommand() parses the command given by the user
   into operator and operand(s) */
int ParseCommand (int &op, int arr[], const char *buff){
  int i = 0;
  char tmp[3][ARG_SIZE];

  //break the user input on spaces
  char* pch = strtok(buff, " ");

  //copy each part into tmp[] string array
  while (pch != NULL){
    strcpy(tmp[i], pch);
    tmp[i][strlen(pch)] = 0;
    pch = strtok (NULL, " ");
    i++;
  }

  //if count is 2 then we probably have the case of NOT operation
  if(i == 2){
    if(MapOperator(tmp[0], op) != OK){
      Serial.println("ERROR");
      Serial.println("Invalid operator.\n");
      return ERR;
    }
    if(BinaryStr2Int(tmp[1], arr[0]) != OK){
      Serial.println("ERROR");
      Serial.print("Invalid operand ");
      Serial.println(tmp[1]);
      Serial.println();
      return ERR;
    }
    arr[1] = arr[0]; //it will be ok if we don't do this
  }
  //if i is 3 then probably the other operations
  else if(i == 3){
    if(MapOperator(tmp[1], op) != OK){
      Serial.println("ERROR");
      Serial.println("Invalid operator.\n");
      return ERR;
    }
    if(BinaryStr2Int(tmp[0], arr[0]) != OK){
      Serial.println("ERROR");
      Serial.print("Invalid operand ");
      Serial.println(tmp[0]);
      Serial.println();
      return ERR;
    }
    if(BinaryStr2Int(tmp[2], arr[1]) != OK){
      Serial.println("ERROR");
      Serial.print("Invalid operand ");
      Serial.println(tmp[2]);
      Serial.println();
      return ERR;
    }
  }
  //if it is less than 2 or more than 3 than the input is incorrect
  else{
      Serial.println("ERROR");
      Serial.println("Invalid number of arguments.\n");
      return ERR;
    }
  return OK;
}

/* AssemblyBinCalc() will implement assembly switch statement
   according to the supplied operator
   and conduct the corresponding binary operation on supplied operand(s)*/
int AssemblyBinCalc(int op, int arg1, int arg2){
  int res = ERR; //initialize the result with err
  
  /* inline assembly */
  asm (
    /* implement the switch statement on supplied operator */
    "mov R16, %3 \n\t"  //move contents of op to R16 register
    
    "tst R16 \n\t"      //check if op(R16) is zero
    "breq .NOT \n\t"    //if it is branch to case NOT
    
    "cpi R16, 0x1 \n\t" //compare op to 1
    "breq .AND \n\t"    //if equals branch to case AND
    
    "cpi R16, 0x2 \n\t" //compare op to 2
    "breq .OR \n\t"     //if equals branch to case OR
    
    "cpi R16, 0x3 \n\t" //compare op to 3
    "breq .XOR \n\t"    //if equals branch to case XOR
    
    "cpi R16, 0x4 \n\t" //compare op to 4
    "breq .NAND \n\t"   //if equals branch to case NAND
    
    "cpi R16, 0x5 \n\t" //compare op to 5
    "breq .NOR \n\t"    //if equals branch to case NOR
    
    "cpi R16, 0x6 \n\t" //compare op to 6
    "breq .XNOR \n\t"   //if equals branch to case XNOR
    
    "cpi R16, 0x7 \n\t" //compare op to 7
    "breq .IMP \n\t"    //if equals branch to case IMP
    
    "jmp .EXIT \n\t"    //default case, this should not happen
    
    ".NOT: \n\t"        //case NOT:
    "com %1 \n\t"       //one's complement %1 (arg1)
    "jmp .END \n\t"     //jump to END
    
    ".AND: \n\t"        //case AND:
    "and %1, %2 \n\t"   //arg1 = arg1 AND arg2
    "jmp .END \n\t"     //jump to END
    
    ".OR: \n\t"         //case OR:
    "or %1, %2 \n\t"    //arg1 = arg1 OR arg2
    "jmp .END \n\t"     //jump to END
    
    ".XOR: \n\t"        //case XOR:
    "eor %1, %2 \n\t"   //arh1 = arg1 XOR arg2
    "jmp .END \n\t"     //jump to END
    
    ".NAND: \n\t"       //case NAND:
    "and %1, %2\n\t"   //arg1 = arg1 AND arg2
    "com %1 \n\t"       //one's complement %1 (arg1)
    "jmp .END \n\t"     //jump to END
    
    ".NOR: \n\t"        //case NOR:
    "or %1, %2 \n\t"    //arg1 = arg1 OR arg2
    "com %1 \n\t"       //one's complement %1 (arg1)
    "jmp .END \n\t"     //jump to END
    
    ".XNOR: \n\t"       //case XNOR
    "eor %1, %2 \n\t"   //arh1 = arg1 XOR arg2
    "com %1 \n\t"       //one's complement %1 (arg1)
    "jmp .END \n\t"     //jump to END
    
    ".IMP: \n\t"        //case IMP
    "com %1 \n\t"       //one's complement %1 (arg1)
    "or %1, %2 \n\t"    //arg1 = arg1 -> arg2 = ~arg1 OR arg2
    "jmp .END \n\t"     //jump to END

    /* if everything went well the program will get here */
    ".END: \n\t"        //the end of "switch" statement
    "mov %0, %1 \n\t"   //move contents of arg1 to res
        
    ".EXIT: \n\t"       //jump here to exit without saving the result
    : "=r" (res)        //output argument res = %0
    : "r" (arg1), "r" (arg2), "r" (op) //input arguments %1 = arg1, %2 = arg1, %2 = op
  );

  if(res == ERR){
      Serial.println("ERROR");
      Serial.print("Something went wrong with assembly switch");
      return ERR;
  }
  return res;
}


/* AssemblyBinMask() creates a mask that will allow us to "trim" the higher bits
   so that the length of the result in binary <= max(strlen(bin_arg1), strlen(bin_arg2))
   without this "NOT 1" will result in 11111110, 
   while this is factually correct, as we're working with 8-bit integers and 1 is represented as 00000001
   "NOT 1 = 0" looks better for our purposes. */
void AssemblyBinMask(int &r, int arg1, int arg2){
  /* inline assembly */
    asm (
    /* bugfix: no need to apply the mask if the highest bit in either binary operand is set
       if we did proceed with masking in this case the rounding operation would seek
       higher power of two than what would fit in the register */
    "sbrc %1, 7 \n\t"   //if highest bit is not set in arg1 skip the next instruction
    "jmp .NOMASK \n\t"  //jump to no mask
    "sbrc %2, 7 \n\t"   //if highest bit is not set in arg2 skip the next instruction
    "jmp .NOMASK \n\t"  //jump to no mask

    /* if we got here the highest bit is not set is either arguments*/
    /* find max(arg1, arg2) and write it to arg1 */
    "cp %1, %2 \n\t"    //compare arg1 and arg2
    "brge .A1 \n\t"     //if arg1 >= arg2 jump to A1 and skip the next line
    "mov %1, %2\n\t"    //else it seems that arg2 > arg1 and copy contents of %2 into %1
    ".A1: \n\t"         //%1 now contains max(arg1, arg2)

    /* roound %1 up to (nearest power of two-1) to create the mask */
    //x |= x >> 1
    "mov %2, %1 \n\t"
    "lsr %1 \n\t"       //>>1
    "or  %1, %2 \n\t"
    //x |= x >> 2      
    "mov %2, %1 \n\t"
    "lsr %1 \n\t"       //>>1
    "lsr %1 \n\t"       //>>2
    "or  %1, %2 \n\t"
    //x |= x >> 4     
    "mov %2, %1 \n\t"
    "lsr %1 \n\t"       //>>1
    "lsr %1 \n\t"       //>>2
    "lsr %1 \n\t"       //>>4
    "or  %1, %2 \n\t"   //%1 now contains the mask
    
    "and %0, %1 \n\t"   //apply mask to r
    
    ".NOMASK: \n\t"
    : "r+" (r)        //output argument r = %0
    : "r" (arg1), "r" (arg2) //input arguments %1 = arg1, %2 = arg2
  );
}

void setup() {
  //initialize Serial monitor and set data rate to 19200 bits per second
  Serial.begin(19200); 
  //Print the instructions
  Serial.println("Please enter desired operation in the following format:");
  Serial.println("NOT <binary_number> OR <binary_number> AND|OR|XOR|NAND|NOR|XNOR|->|EQV <binary_number>");
  Serial.println();
}

void loop() {
  //if anything comes in Serial (USB)
  if (Serial.available()){
    char buf[BUFF_SIZE]; //initialize buffer
    for(int i=0; i<BUFF_SIZE; i++) 
        buf[i] = '\0';
    
    Serial.readBytes(buf, BUFF_SIZE); //read to buffer
    //set last element, which was newline '\n' to '\0' instead
    for(int i=0; i<BUFF_SIZE; i++) 
      if(buf[i] == '\n'){ 
        buf[i] = '\0';
        break;
      }

    //uppercase and output user input
    ToUpper(buf);
    Serial.print(buf);
    Serial.print(" = ");

    int op, argArr[2], res;

    //if commands are parsed successfully run AssemblyBinCalc, AssemblyBinMask and output result
    if(ParseCommand(op, argArr, buf) == OK){
        res = AssemblyBinCalc(op, argArr[0], argArr[1]);
        AssemblyBinMask(res, argArr[0], argArr[1]);
        Serial.print(res, BIN); //print result in binary
        Serial.println("\n");
    }
  }
}
