with open("C:/Users/tomas/Desktop/Practice-Projects/Practice/File_practice/text1.txt", "w") as file: #First clear the txt file then write into it
    file.write("Hello")

with open("C:/Users/tomas/Desktop/Practice-Projects/Practice/File_practice/text2.txt", "+a") as file: #Write into the txt file but now delete it
    file.write("Hi")

with open("C:/Users/tomas/Desktop/Practice-Projects/Practice/File_practice/text2.txt", "r") as file: #Read the txt file
    one_line = file.read()
    print(f"\nOne line: {one_line}\n")


#w = First clear the txt file then write into it
#a = Write into the txt file but now delete it
#r = Read the txt file
#r+ = r + w