import os

def list_files(directory):
    # Create an empty list to store file names
    files_list = []
    
    # Walk through the directory
    for root, dirs, files in os.walk(directory):
        for file in files:
            # Append the full file path
            files_list.append(os.path.join(root, file))
    
    return files_list

if __name__ == "__main__":
    # Specify the directory you want to list files from
    directory = '../bvh'  # Change this to your target directory
    
    # Get the list of files
    files = list_files(directory)
    
    # Print the file names
    for file in files:
        print(file[3:])
