stack = []

#add element
for x in range(1, 11):
    stack.append(x)
print(stack)

#pick top element
top = stack[-1]
print(top)

#pop the first element
stack.pop()
print(stack)

#count the size
size = len(stack)
print(size)