def dfs(graph, start):
    visited = []
    stack = [start]

    while stack:
        node = stack.pop()
        
        if node not in visited:
            visited.append(node)
            print(node, end=" ")

            # We use reversed() so neighbors are processed in the same order as BFS
            for neighbor in reversed(graph[node]): #C->B
                if neighbor not in visited:
                    stack.append(neighbor)  #round1 - C->B  #round2 - F->B  #round3 - E->B  #round4 - B->D  #round5 -

my_graph = {
    'A': ['B', 'C'],
    'B': ['A', 'D', 'E'],
    'C': ['A', 'F'],
    'D': ['B'],
    'E': ['B', 'F'],
    'F': ['C', 'E']
}

print("BFS Result:")
dfs(my_graph, 'A')
