import sys
import os

# Check if at least one argument is provided (excluding the script name)
if len(sys.argv) > 2:
    # Read the first parameter (excluding the script name)
    lab_sub = sys.argv[1]
    # 0 represents converting to website version
    # 1 represents converting to editor version
    direction = int(sys.argv[2])

# read all the file names in a directory
files = os.listdir("_posts")

for file in files:
    if lab_sub.lower() in file.lower():
        print(f"Processing file: {file}")

# combine directory name and file name to create the full path
full_path = os.path.join("_posts", file)

# Open the file and read its contents
with open(full_path, 'r') as file:
    print("Reading file...")
    content = file.read()

# Perform the substitution
if direction == 0:
    print("Converting to website version...")
    updated_content = content.replace("../images", "/FastRobots/images")
else:
    print("Converting to editor version...")
    updated_content = content.replace("/FastRobots/images", "../images")

print(updated_content)

# Open the file again in write mode and save the updated content
with open(full_path, 'w') as file:
    print("Updating file...")
    file.write(updated_content)
    print("File write end.")

print("File has been updated.")