from copy import deepcopy


class mazepixel:
    # 인접 칸과 연결되어 있으면 true
    def __init__(self, right, down):
        self.right = right
        self.down = down


class mazefind:
    def __init__(self):
        self.visit = False
        # -2 위 -1 왼쪽 1 오른쪽 2 아래쪽
        self.next = 0


class mazesearch:
    def __init__(self, mazefind):
        self.mazefind = mazefind
        self.positionx = 0
        self.positiony = 0
        self.finishy = len(mazefind) - 1
        self.finishx = len(mazefind[self.finishy]) - 1


def readfile(filename):
    file = open(filename, "rt")
    mazetext = file.readlines()
    file.close()
    across, down = mazetext[0].split(" ")
    maze = list()
    for linept in range(2, len(mazetext), 2):
        # linept가 메인, linept+1이 아래줄
        mazeline = list()
        for acrosspt in range(1, len(mazetext[linept]) - 1, 2):
            # acrosspt가 칸, acrosspt+1이 앞 연결통로
            mazeline.append(mazepixel(mazetext[linept][acrosspt + 1] != '|', mazetext[linept + 1][acrosspt] != '-'))
        maze.append(mazeline)
    return maze


def printmaze(maze, find=None):
    if find is None:
        find = list()
        for i in maze:
            line = list()
            for j in i:
                line.append(mazefind())
            find.append(line)
    else:
        find = find.mazefind
    printarray = list()
    printarray.append('+' + '-' * (len(maze[0]) * 2 - 1) + '+')
    for i in range(len(maze)):
        strline = "|"
        for j in range(len(maze[i])):

            if find[i][j].visit:
                strline += "0"
            else:
                strline += " "

            if maze[i][j].right is False:
                strline += "|"
            elif j + 1 < len(maze[i]) and find[i][j].visit and find[i][j + 1].visit and (
                    find[i][j].next == 1 or find[i][j + 1].next == -1):
                strline += "0"
            else:
                strline += " "
        printarray.append(strline)

        strline = "|"
        if maze[i][0].down is False:
            strline = "+"
        for j in range(len(maze[i])):
            if maze[i][j].down:
                if i + 1 < len(maze) and find[i][j].visit and find[i + 1][j].visit and (
                        find[i][j].next == 2 or find[i + 1][j].next == -2):
                    strline += "0"
                else:
                    strline += " "

                if maze[i][j].right:
                    if j + 1 < len(maze[i]) and maze[i][j + 1].down is False:
                        strline += "+"
                    else:
                        strline += " "
                else:
                    strline += "|"
            else:
                strline += "-"
                if maze[i][j].right:
                    strline += "-"
                else:
                    strline += "+"
        printarray.append(strline)

    for i in range(0, len(printarray), 2):
        for j in range(len(printarray[i])):
            if printarray[i][j] == '+' or printarray[i][j] == '0':
                continue
            elif i > 0 and printarray[i - 1][j] == '|':
                if printarray[i][j] == '+':
                    continue
                elif printarray[i][j] == '-' or (j + 1 < len(printarray[i]) and printarray[i][j + 1] == '-'):
                    printarray[i] = printarray[i][:j] + '+' + printarray[i][j + 1:]
                elif i + 1 < len(printarray) and printarray[i + 1][j] == '|':
                    printarray[i] = printarray[i][:j] + '|' + printarray[i][j + 1:]
                else:
                    printarray[i] = printarray[i][:j] + '+' + printarray[i][j + 1:]
            elif (i + 1 < len(printarray) and printarray[i + 1][j] == '|') or (
                    j + 1 < len(printarray[i]) and printarray[i][j + 1] == '-' and (
                    printarray[i][j] == ' ' or printarray[i][j] == '|')) or (
                    j + 1 < len(printarray[i]) and printarray[i][j] == '-' and (
                    printarray[i][j + 1] == ' ' or printarray[i][j + 1] == '0')):
                printarray[i] = printarray[i][:j] + '+' + printarray[i][j + 1:]

    return printarray


def findmaze(maze):
    result = list()
    stack = list()
    find = list()
    for i in maze:
        line = list()
        for j in i:
            line.append(mazefind())
        find.append(line)
    find[0][0].visit = True
    stack.append(mazesearch(find))

    while len(stack) > 0:
        find = stack.pop()

        # [y][x]
        # up
        if find.positiony > 0 and maze[find.positiony - 1][find.positionx].down and \
                find.mazefind[find.positiony - 1][find.positionx].visit is False:
            search = deepcopy(find)
            search.mazefind[search.positiony][search.positionx].next = -2
            search.positiony -= 1
            search.mazefind[search.positiony][search.positionx].visit = True
            stack.append(search)

        # left
        if find.positionx > 0 and maze[find.positiony][find.positionx - 1].right and \
                find.mazefind[find.positiony][find.positionx - 1].visit is False:
            search = deepcopy(find)
            search.mazefind[search.positiony][search.positionx].next = -1
            search.positionx -= 1
            search.mazefind[search.positiony][search.positionx].visit = True
            stack.append(search)

        # right
        if find.positionx + 1 < len(maze[find.positiony]) and maze[find.positiony][find.positionx].right and \
                find.mazefind[find.positiony][find.positionx + 1].visit is False:
            search = deepcopy(find)
            search.mazefind[search.positiony][search.positionx].next = 1
            search.positionx += 1
            search.mazefind[search.positiony][search.positionx].visit = True
            if search.positionx == search.finishx and search.positiony == search.finishy:
                result.append(search)
            else:
                stack.append(search)

        # down
        if find.positiony + 1 < len(maze) and maze[find.positiony][find.positionx].down and \
                find.mazefind[find.positiony + 1][find.positionx].visit is False:
            search = deepcopy(find)
            search.mazefind[search.positiony][search.positionx].next = 2
            search.positiony += 1
            search.mazefind[search.positiony][search.positionx].visit = True
            if search.positionx == search.finishx and search.positiony == search.finishy:
                result.append(search)
            else:
                stack.append(search)
    return result


maze = readfile(input("미로 파일의 이름을 입력하세요 : "))
result = findmaze(maze)
if len(result) == 0:
    print("길이 없음")
elif len(result) == 1:
    print("길이 1개 있음")
else:
    print("길이 2개 이상 있음")

if len(result) > 0:
    string = printmaze(maze, result[0])
    for i in string:
        print(i)
