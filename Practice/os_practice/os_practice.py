import os

print("\n \n")


#Get cwd (what directory am I right now)
cwd = os.getcwd()
print(f"CWD: {cwd}")

print("\n")

#Get all files inside a specified folder
inside_files = os.listdir(cwd)
print(f"List of files: {inside_files}")

print("\n")
#Removes a file (for removed folder there is shutil)
os.remove#(name of the file)

print("\n")
#Check if the path (to that file or folder) exists
exists = os.path.exists(cwd)
print(f"Path exists: {exists}")

print("\n")
#Doesn't move the file physicly but creates the path so someone can copy it and move it there (to string)
moved_string = os.path.join("c:/Users/tomas/Desktop/Practice-Projects/Practice/os_practice", "os_text.txt")
print(f"Moved string: {moved_string}")
#More is on the shutil folder

print("\n")
#Tells the full path of a file (or folder)
full_path_file = os.path.abspath("os_practice.py")
print(f"Total file path: {full_path_file}")


print("\n")
#Creates a folder or a subfolder
#a) only folder
#       os.makedirs("c:/Users/tomas\Desktop/Practice-Projects/Practice/os_practice/os_test_only")
#b) with subfolders
#       os.makedirs("c:/Users/tomas\Desktop/Practice-Projects/Practice/os_practice/os_test_onlyos_test_sub")



print("\n")
#Walk through the whole hardrive if wanted and returns current root, dirs, files ( files are returned in a list )
#To find specific file type this code
#If searching the whole Users it lasts a longer time

file_name = "os_practice.py"
for root, dirs, files in os.walk("C:/Users"): #Type in walk() where you want to start ("." is cwd)
    if file_name in files: #IF FINDING FOLDER TYPE DIRS INSTEAD OF FILES
        print(f"File '{file_name}' was found in this directory: {root}")


print("\n")
#Type anything in the terminal using code
connectivity = os.system("PING google.com")
#Type -> "PING google.com" <- type url or IP address for checking the connectivity


print("\n \n")