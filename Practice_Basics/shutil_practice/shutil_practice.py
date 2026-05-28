import shutil

print("\n")
#Move + copy or copy to the same destination a folder or a file
shutil.copy(src="C:/Users/tomas/Desktop/Practice-Projects/Practice/txt_practice.txt", 
            dst="C:/Users/tomas/Desktop/Practice-Projects/Practice/shutil_practice/txt_practice_COPY.txt")

print("The file was copied and moved to destination")


print("\n")
#Move the file to the specified path
shutil.move#(src, dst) #src is what file you want to move - dst is where do you want to move it to

print("\n")
#Remove folder (for file removal there is udes os)
shutil.rmtree#(name of the folder)

print("\n")
#How much Storage is in specified path
usage = (shutil.disk_usage("C:"))
print(f"Usage -> \nTotal: {usage[0]}\nUsed: {usage[1]}\nFree: {usage[2]}")

