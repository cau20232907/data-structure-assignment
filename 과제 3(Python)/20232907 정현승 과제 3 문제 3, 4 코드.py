import time
import random


class Word:
    def __init__(self, line):
        self.en, self.ko = str(line).split(':', 1)
        self.en = self.en.strip()
        self.ko = self.ko.strip()


class WordLinkedList:
    def __init__(self, line, /, start=None):
        self.word = Word(line)
        if start is None:
            self.prev = None
            self.next = None
        else:
            start.putword(self)

    def putword(self, word):
        cur = self
        while cur.prev is not None and cur.word.en > word.word.en:
            cur = cur.prev
        if cur.word.en > word.word.en:
            word.prev = cur.prev
            word.next = cur
            if cur.prev is not None:
                cur.prev.next = word
            cur.prev = word
            return word
        while cur.next is not None and cur.word.en < word.word.en:
            cur = cur.next
        if cur.word.en < word.word.en:
            word.next = None
            word.prev = cur
            cur.next = word
            return self
        word.next = cur
        word.prev = cur.prev
        cur.prev.next = word
        cur.prev = word
        return self

    def search(self, target):
        cur = self
        while cur is not None:
            if cur.word.en < target:
                cur = cur.next
                if cur is None or cur.word.en > target:
                    return None
            elif cur.word.en > target:
                cur = cur.prev
                if cur is None or cur.word.en < target:
                    return None
            else:
                return cur
        return cur

    '''
    
        while cur is not None and cur.word.en != target:
            cur = cur.next
        if cur is None:
            return None
        else:
            return cur.word
        
    '''

    def getcharhead(self, chs):
        cur = self
        while cur.next is not None and cur.word.en < chs:
            cur = cur.next
        return cur


def chartoidx(ch):
    asciicode = ord(ch)
    if asciicode >= ord('a') and asciicode <= ord('z'):
        return asciicode - ord('a') + 26
    elif asciicode >= ord('A') and asciicode <= ord('Z'):
        return asciicode - ord('A')
    elif asciicode < ord('A'):
        return 0
    elif asciicode > ord('Z') and asciicode < ord('a'):
        return 26
    else:
        return 51


file = open("D:/OneDrive - 중앙대학교/2학년 1학기/자료구조/randdict.TXT", "rt")
words = None
ens = list()
for wordline in file.readlines():
    ens.append((wordline.split(":"))[0].strip())
    if words is None:
        words = WordLinkedList(wordline)
    else:
        words = words.putword(WordLinkedList(wordline))
file.close()

letterindex = list()  # ['A','B',...,'Z','a',...,'z']
for ch in range(ord('A'), ord('Z') + 1):
    letterindex.append(chr(ch))
for ch in range(ord('a'), ord('z') + 1):
    letterindex.append(chr(ch))
cur = words
wordindex = list()
for ch1 in letterindex:
    innerindex = list()
    for ch2 in letterindex:
        cur = cur.getcharhead(ch1 + ch2)  # 커서를 같이 옮겨주면 시간이 절약됨
        innerindex.append(cur)
    wordindex.append(innerindex)

random.shuffle(ens)

# 3번
startc = time.time()
for singleword in ens:
    res3 = words.search(singleword)
endc = time.time()
time3 = endc - startc

# 4번
startc = time.time()
for singleword in ens:
    temp = singleword + 'AA'  # 1글자만 입력되었으면 문제가 발생하기 때문에 처리해줌
    res4 = wordindex[chartoidx(temp[0])][chartoidx(temp[1])].search(singleword)
endc = time.time()
time4 = endc - startc

print("48406개의 단어 모두 찾기:\n"+
      f"기존 연결리스트 저장 구조 소요시간: {time3:.5f}초, 개선된 연결리스트 저장 구조 소요시간: {time4:.5f}초")

# 3번(검색)
while True:
    wordtofind = input(">> ")

    # 3번(처음부터 검색)
    res3 = words.search(wordtofind)

    '''4번(앞 두 글자 이용)
    temp = wordtofind + 'AA'  # 1글자만 입력되었으면 문제가 발생하기 때문에 처리해줌
    res4 = wordindex[chartoidx(temp[0])][chartoidx(temp[1])].search(wordtofind)
    '''

    if res3 is not None:
        print(res3.word.ko)
    else:
        print("해당하는 단어가 없습니다.")
