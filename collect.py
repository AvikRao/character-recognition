import os 

ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

cwd = os.getcwd()
recdir = os.path.join(cwd, "google-images-download")
recdir = os.path.join(recdir, "google_images_download")

os.chdir(recdir)

for letter in ALPHABET :
    os.system(f"python google_images_download.py -k \"Letter {letter}\" -l 20 -f \"jpg\" -ct \"black-and-white\" -o ../../basis_images/{letter} -p")

