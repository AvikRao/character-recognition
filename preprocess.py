import os 

ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
cwd = os.getcwd()
basis_dir = os.path.join(cwd, "basis_images")

for letter in ALPHABET :
    letter_dir = os.path.join(basis_dir, letter)
    os.chdir(letter_dir)
    files = os.listdir()
    for i in range(len(files)) :
        os.system(f"convert -resize 400x400\! {files[i]} -compress none {str(i).zfill(3)}.ppm")
        # print(files[i])