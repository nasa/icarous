
import sys

fd = open(sys.argv[1],'r')
className = sys.argv[2]
completed = False
start = False

inputHFile   = sys.argv[1].split("/")[-1]

outFileName1 = className+"Wrapper.h"
outFileName2 = className+"Wrapper.cpp"

output1 = open(outFileName1,"w")
output2 = open(outFileName2,"w")

pretext1 = "#ifndef "+className.upper()+"WRAPPER_H \n" \
          +"#define "+className.upper()+"WRAPPER_H \n\n" \
          +"#ifdef __cplusplus\n" \
          +"extern \"C\"{\n#endif\n\n"

postext1 = "#ifdef __cplusplus \n};\n#endif\n\n#endif"

cClassName = "c_"+className

maintext1 = "struct c_"+className+";\n\n"


pretext2 = "#include \""+inputHFile+"\"\n" \
        +  "#include \""+outFileName1+"\"\n\n"\
        +  "#define ToCPP(a) (reinterpret_cast<"+className+"*>(a))\n" \
        +  "#define ToC(a) (reinterpret_cast<"+cClassName+"*>(a))\n\n\n"

maintext2 = ""

while not completed:
    line = fd.readline()

    line = line.rstrip("\n")
    line = line.rstrip(";")
    line = line.lstrip(" ")
    line = line.rstrip(")")
    elements = line #.split(" ")

    if len(elements) == 0:
        continue

    if elements[0] == "/" or elements[0]=="*":
        continue

    elif elements == "__CWRAP__":
        if start is False:
            start = True
        else:
            completed = True
        continue

    if start is True:
        function = elements.split("(")
        funcElements = function[0].split(" ")
        if len(funcElements) == 1:
            argsList1 = function[1].split(",")
            outArgs1 = ""
            for arg in argsList1:
                if arg == " ":
                    continue
                arg = arg.lstrip(" ")
                arg = arg.rstrip(" ")
                outArgs1 = outArgs1 + arg.lstrip()
                outArglist = outArgs1.split(",")
                inArgslist = []
                for argname in outArglist:
                    argname = outArgs1.split(" ")[1]
                    argname = argname.rstrip("]")
                    argname = argname.rstrip("[")
                    argname = argname.strip("*")
                    inArgslist = argname + ","
                inArgslist = inArgslist.rstrip(",")
            maintext1 = maintext1 + "struct " + cClassName + "* c_init" + className + "("+outArgs1 +");\n\n"
            temp  =  "struct " + cClassName + "* c_init" + className + "("+outArgs1 +");\n\n"
            maintext2 = maintext2 + temp.rstrip("\n")
            maintext2 = maintext2.rstrip(";")
            maintext2 = maintext2 + "{\n    " \
            + className + "* ccptr = new "+className+"("+inArgslist+");\n" \
            + "    return ToC(ccptr);\n}\n\n"


        elif len(funcElements) == 2:
            returnType = funcElements[0]
            funcName   = funcElements[1]

            argsList1 = function[1].split(",")
            outArgs1 = "struct " + cClassName + "* cptr"
            outArgs2 = ""
            if len(argsList1) > 0:
                for arg in argsList1:
                    if arg == "":
                        continue
                    arg = arg.lstrip(" ")
                    arg = arg.rstrip(" ")
                    outArgs1  = outArgs1 + ","+arg
                    outArgs2  = outArgs2 + arg.split(" ")[1] + ","

            outArgs2 = outArgs2.rstrip(",")

            maintext1 = maintext1 + returnType + " c_"+funcName + "("+outArgs1+");\n\n"
            temp = returnType + " c_"+funcName + "("+outArgs1+");\n\n"
            maintext2 = maintext2 + temp.rstrip("\n")
            maintext2 = maintext2.rstrip(";")
            maintext2 = maintext2 + "{\n    " \
            + className + "* ccptr = ToCPP(cptr);\n" \
            + "    return ccptr->"+funcName+"("+outArgs2+");\n}\n\n"
    else:
        continue


output1.write(pretext1)
output1.write(maintext1)
output1.write(postext1)

output2.write(pretext2)
output2.write(maintext2)






