import time


class Word:
    def __init__(self, line):
        self.en, self.ko = str(line).split(':', 1)
        self.en = self.en.strip()
        self.ko = self.ko.strip()

    def __cmp__(self, other):
        if self.en < other.en:
            return -1
        elif self.en > other.en:
            return 1
        else:
            return 0

    def __eq__(self, other):
        return self.en == other.en

    def __ne__(self, other):
        return self.en != other.en

    def __lt__(self, other):
        return self.en < other.en

    def __gt__(self, other):
        return self.en > other.en

    def __le__(self, other):
        return self.en <= other.en

    def __ge__(self, other):
        return self.en >= other.en


def readDict(filename):
    file = open(filename, "rt")
    res = list()
    for wordline in file.readlines():
        res.append(Word(wordline))
    file.close()
    return res


def listSortedCheck(targetList):
    for i in range(1, len(targetList)):
        if targetList[i - 1] >= targetList[i]:
            print(f"정렬 실패: index {i - 1}의 단어: {sortedDictionary[i - 1]}, " +
                  f"index {i}의 단어: {sortedDictionary[i]}")
            raise ValueError


def listInsertionsort(targetList):
    for i in range(1, len(targetList)):
        j = i
        tmp = targetList[i]
        while j > 0 and tmp < targetList[j - 1]:
            targetList[j] = targetList[j - 1]
            j -= 1
        targetList[j] = tmp


def listQuicksort(targetList):
    listQuicksortStacks(targetList, 0, len(targetList))


def listQuicksortStacks(targetList, start, end):
    low = start + 1
    high = end - 1
    while low < end and targetList[start] > targetList[low]:
        low += 1
    while high > start and targetList[start] < targetList[high]:
        high -= 1
    while low < high:
        targetList[low], targetList[high] = targetList[high], targetList[low]
        while low < end and targetList[start] > targetList[low]:
            low += 1
        while high > start and targetList[start] < targetList[high]:
            high -= 1
    targetList[start], targetList[high] = targetList[high], targetList[start]

    if high - start > 1:
        listQuicksortStacks(targetList, start, high)
    if end - low > 1:
        listQuicksortStacks(targetList, low, end)


def listHeapsort(targetList):
    for i in range(len(targetList) // 2 - 1, -1, -1):
        listHeapsortAdjustments(targetList, i, len(targetList))
    for i in range(len(targetList) - 1, 0, -1):
        targetList[0], targetList[i] = targetList[i], targetList[0]
        listHeapsortAdjustments(targetList, 0, i)


def listHeapsortAdjustments(targetList, root, targetLength):
    rootValue = targetList[root]
    childIndex = root * 2 + 1
    while childIndex < targetLength:
        if childIndex + 1 < targetLength and targetList[childIndex] < targetList[childIndex + 1]:
            childIndex += 1
        if rootValue > targetList[childIndex]:
            break
        targetList[root] = targetList[childIndex]
        root = childIndex
        childIndex = root * 2 + 1
    targetList[root] = rootValue


dictionary = readDict("D:/OneDrive - 중앙대학교/2학년 1학기/자료구조/randdict.TXT")

sortedDictionary = dictionary.copy()
startc = time.time()
listInsertionsort(sortedDictionary)
endc = time.time()
listSortedCheck(sortedDictionary)
insertionsortTime = endc - startc

sortedDictionary = dictionary.copy()
startc = time.time()
listQuicksort(sortedDictionary)
endc = time.time()
listSortedCheck(sortedDictionary)
quicksortTime = endc - startc

sortedDictionary = dictionary.copy()
startc = time.time()
listHeapsort(sortedDictionary)
endc = time.time()
listSortedCheck(sortedDictionary)
heapsortTime = endc - startc

print(f"삽입 정렬: {insertionsortTime}초")
print(f"퀵 정렬: {quicksortTime}초")
print(f"힙 정렬: {heapsortTime}초")
