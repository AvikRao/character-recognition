import os

class Preprocess() :

    def __init__(self) :
        self._cwd = os.getcwd()
        self._chars = "0123456789"
        self._basisdir = os.path.join(self._cwd, "basis_images")

    def _resetcwd(self) :
        os.chdir(self._cwd)

    def convert_to_ppm(self, dataset) :
        dataset_dir = os.path.join(self._basisdir, dataset)
        for character in self._chars :
            char_dir = os.path.join(dataset_dir, character)
            os.chdir(char_dir)
            char_png_files = os.listdir()
            for i in range(len(char_png_files)) :
                os.system(f"convert {char_png_files[i]} -compress none {i:04}.ppm")
        
        self._resetcwd()

    

def main() :
    preprocess = Preprocess()
    preprocess.convert_to_ppm("testing")
    preprocess.convert_to_ppm("training")

if __name__ == '__main__' :
    main()