class expressiontree:
    def __init__(self, left, this, right):
        self.left = left
        self.this = this
        self.right = right

    def strpreorder(self):
        if self.left is None:
            return str(self.this)
        else:
            return str(self.this) + " " + self.left.strpreorder() + " " + self.right.strpreorder()

    def strinorder(self):
        if self.left is None:
            return str(self.this)
        else:
            return self.left.strinorder() + " " + str(self.this) + " " + self.right.strinorder()

    def strpostorder(self):
        if self.left is None:
            return str(self.this)
        else:
            return self.left.strpostorder() + " " + self.right.strpostorder() + " " + str(self.this)

    def strlevelorder(self):
        queue = list()
        string = ""
        queue.append(self)
        while len(queue) > 0:
            obj = queue.pop(0)
            string += str(obj.this) + ' '
            if obj.left is not None:
                queue.append(obj.left)
            if obj.right is not None:
                queue.append(obj.right)
        return string

    def cal(self):
        if self.left is None:
            return self.this
        else:
            match self.this:
                case '+':
                    return self.left.cal() + self.right.cal()
                case '-':
                    return self.left.cal() - self.right.cal()
                case '*':
                    return self.left.cal() * self.right.cal()
                case '/':
                    return self.left.cal() / self.right.cal()


def maketree(instr):
    treelist = list()
    pointflag = 0
    startpt = -1
    endpt = -1
    i = 0
    while i < len(instr):
        if instr[i].isspace():
            if startpt >= 0 and endpt == -1:  # 이외는 for문 계속
                endpt = i
        elif instr[i].isdigit():
            if startpt == -3:
                raise ValueError(makeerroroutstr(i, "닫는 괄호 뒤에는 연산자가 와야 함"))
            elif startpt == -1:
                startpt = i
            elif endpt != -1:
                raise ValueError(makeerroroutstr(i, "연속한 숫자"))
        elif instr[i] == '.':
            if startpt == -3:
                raise ValueError(makeerroroutstr(i, "닫는 괄호 뒤에는 연산자가 와야 함"))
            elif startpt == -1:
                raise ValueError(makeerroroutstr(i, "숫자 없이 .이 먼저 올 수 없음"))
            elif pointflag == 0:
                pointflag = 1
            else:
                raise ValueError(makeerroroutstr(i, "잘못된 숫자"))
        else:
            if startpt == -1 and instr[i] != '(':
                raise ValueError(makeerroroutstr(i, "연산자 뒤에 숫자 또는 여는 괄호만 올 수 있음"))
            elif startpt != -3 and instr[i] != '(':
                if endpt == -1:
                    endpt = i
                if pointflag == 1:
                    treelist.append(expressiontree(None, float(instr[startpt:endpt]), None))
                else:
                    treelist.append(expressiontree(None, int(instr[startpt:endpt]), None))
                endpt = -1
            elif startpt >= 0 and instr[i] == '(':
                raise ValueError(makeerroroutstr(i, "연속한 숫자"))
            startpt = -1
            pointflag = 0
            match (instr[i]):
                case '+' | '-' | '*' | '/':
                    treelist = calculatestack(treelist, instr[i])
                    treelist.append(instr[i])
                case '(':

                    open = 1
                    startpt = i + 1
                    try:
                        while open > 0:
                            i += 1
                            if instr[i] == '(':
                                open += 1
                            elif instr[i] == ')':
                                open -= 1
                    except IndexError:
                        raise ValueError(makeerroroutstr(i, "괄호가 닫히지 않았습니다"))
                    try:
                        treelist.append(maketree(instr[startpt:i]))
                    except ValueError as error:
                        raise ValueError(" "*startpt+str(error))
                    startpt = -3
                case _:
                    raise ValueError(makeerroroutstr(i, "해석할 수 없는 기호"))
        i += 1

    if startpt != -1 and startpt != -3:
        if endpt == -1:
            endpt = len(instr)
        if pointflag == 1:
            treelist.append(expressiontree(None, float(instr[startpt:endpt]), None))
        else:
            treelist.append(expressiontree(None, int(instr[startpt:endpt]), None))
    treelist = calculatestack(treelist, "fin")
    if len(treelist) == 1:
        return treelist.pop()
    else:
        raise ValueError


def makeerroroutstr(index, message):
    restr = " " * index
    restr += "^ 이 위치에 오류가 있습니다. (" + message + ")"
    return restr


def calculatestack(stack, char):
    while (len(stack) > 2) or (char == "fin" and len(stack) != 1):
        right = stack.pop()
        code = stack.pop()
        left = stack.pop()
        match code:
            case '+' | '-' if char != '*' and char != '/':
                stack.append(expressiontree(left, code, right))
            case '*' | '/':
                stack.append(expressiontree(left, code, right))
            case _:
                stack.append(left)
                stack.append(code)
                stack.append(right)
                return stack
    return stack


try:
    tree = maketree(input("수식 입력:\n"))
    print("전위 순회: " + tree.strpreorder())
    print("중위 순회: " + tree.strinorder())
    print("후위 순회: " + tree.strpostorder())
    print("레벨 순회: " + tree.strlevelorder())
    print(tree.cal())
except ValueError as error:
    print(error)
