from collections import deque

def bfs(graph, start):
    visited = []
    queue = deque([start]) #Place A in queue
    visited.append(start)

    while queue:
        node = queue.popleft() #node = A and was removed from the queue
        print(node, end=" ")

        for neighbor in graph[node]: #B->C
            if neighbor not in visited: 
                visited.append(neighbor) #round1 - B->C  #round2 - C->D->E   #round3 - D->E->F   #round4 - E->F   #round5 -
                queue.append(neighbor)
                
my_graph = {
    'A': ['B', 'C'],
    'B': ['A', 'D', 'E'],
    'C': ['A', 'F'],
    'D': ['B'],
    'E': ['B', 'F'],
    'F': ['C', 'E']
}

print("BFS Result:")
bfs(my_graph, 'A')

