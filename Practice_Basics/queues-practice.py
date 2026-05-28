queue = []

#Enqueue - add element to the top
queue.append("1")
queue.append("2")
queue.append("3")
print("Queue: ", queue)

#Find first element
frontElement = queue[0]
print("Peek: ", frontElement)

#Dequeue - remove first element
poppedElement = queue.pop(0)
print("Dequeue: ", poppedElement)

print("Queue after Dequeue: ", queue)

#isEmpty - check if it's empty and returt boolen
isEmpty = not bool(queue)
print("isEmpty: ", isEmpty)

#Size - how many elements are there
print("Size: ", len(queue))