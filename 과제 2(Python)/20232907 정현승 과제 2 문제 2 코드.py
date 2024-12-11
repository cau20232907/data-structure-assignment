def makeerroroutstr(index, message):
    restr = " " * index
    restr += "^ 이 위치에 오류가 있습니다. (" + message + ")"
    return restr


def calculatestack(stack, char):
    while ((char == ')' or len(stack) > 2) and stack[-2] != '(') or (char == "fin" and len(stack) != 1):
        num2 = stack.pop()
        code = stack.pop()
        num1 = stack.pop()
        match code:
            case '+' if char != '*' and char != '/':
                stack.append(num1 + num2)
            case '-' if char != '*' and char != '/':
                stack.append(num1 - num2)
            case '*':
                stack.append(num1 * num2)
            case '/':
                stack.append(num1 / num2)
            case '^^':
                stack.append(num1 ** num2)
            case '(' if char == "fin":
                raise ValueError
            case _:
                stack.append(num1)
                stack.append(code)
                stack.append(num2)
                return stack
    return stack


instr = input("수식 입력:\n")
codelist = list()
pointflag = 0
outstr = ""
startpt = -1
endpt = -1
for i in range(len(instr)):
    if outstr != "":
        break
    elif startpt == -2 and instr[i] != '^':
        outstr = makeerroroutstr(i, "^는 하나만 쓰일 수 없음")
    elif instr[i].isspace():
        if startpt >= 0 and endpt == -1:  # 이외는 for문 계속
            endpt = i
    elif instr[i].isdigit():
        if startpt == -3:
            outstr = makeerroroutstr(i, "닫는 괄호 뒤에는 연산자가 와야 함")
        elif startpt == -1:
            startpt = i
        elif endpt != -1:
            outstr = makeerroroutstr(i, "연속한 숫자")
    elif instr[i] == '.':
        if startpt == -3:
            outstr = makeerroroutstr(i, "닫는 괄호 뒤에는 연산자가 와야 함")
        elif startpt == -1:
            outstr = makeerroroutstr(i, "숫자 없이 .이 먼저 올 수 없음")
        elif pointflag == 0:
            pointflag = 1
        else:
            outstr = makeerroroutstr(i, "잘못된 숫자")
    else:
        if startpt == -1 and instr[i] != '(':
            outstr = makeerroroutstr(i, "연산자 뒤에 숫자 또는 여는 괄호만 올 수 있음")
        elif startpt != -2 and startpt != -3 and instr[i] != '(':
            if endpt == -1:
                endpt = i
            codelist.append(float(instr[startpt:endpt]))
            endpt = -1
        if startpt != -2:
            startpt = -1
        pointflag = 0
        match (instr[i]):
            case '+' | '-' | '*' | '/':
                codelist = calculatestack(codelist, instr[i])
                codelist.append(instr[i])
            case '^':
                if startpt != -2:
                    startpt = -2
                else:
                    codelist.append('^^')
                    startpt = -1
            case '(':
                codelist.append('(')
            case ')':
                try:
                    codelist = calculatestack(codelist, instr[i])
                    codelist.pop(-2)  # '(' pop
                    startpt = -3
                except:
                    outstr = makeerroroutstr(0, "여는 괄호가 없음")
            case _:
                outstr = makeerroroutstr(i, "해석할 수 없는 기호")

if outstr == "":
    try:
        if startpt != -1 and startpt != -3:
            if endpt == -1:
                endpt = len(instr)
            codelist.append(float(instr[startpt:endpt]))
        codelist = calculatestack(codelist, "fin")
    except:
        outstr = makeerroroutstr(len(instr), "수식이 연산자로 종료되거나 닫는 괄호가 없음")

if outstr == "":
    outstr = str(round(codelist[-1], 13))  # 유효 범위로 자름
    if round(codelist[-1], 0) == codelist[-1]:
        outstr = str(int(codelist.pop()))


print(outstr)
