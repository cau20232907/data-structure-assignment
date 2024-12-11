from collections import deque


class Node:
    def __init__(self, i):
        self.i = i


class Vertex:
    def __init__(self, node1, node2):
        self.node1 = node1
        self.node2 = node2


def BFS(vertexes, start):
    res = list()
    next = deque()
    target = start
    while target is not None:
        res.append(target)
        for vertex in vertexes:
            if vertex.node1 == target and vertex.node2 not in res:
                next.append(vertex.node2)
            elif vertex.node2 == target and vertex.node1 not in res:
                next.append(vertex.node1)
        target = None
        while target is None and len(next) > 0:
            target = next.popleft()
            if target in res:
                target = None
    return res
