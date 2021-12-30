#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/SVD>
#define IMAGE_SIZE 400

const std::string ALPHABET("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

void construct_bases() {

    std::string basis_dir("./basis_images/");
    for (char c : ALPHABET) {
        std::string letter_dir = basis_dir + c;
        for (auto &ifile_name : std::filesystem::directory_iterator(letter_dir)) {

            std::vector<std::vector<float>> img_matrix;
            img_matrix.push_back({});
            std::string line;


            std::cout << ifile_name.path() << std::endl;
            std::ifstream ifile(ifile_name.path());

            std::getline(ifile, line);
            std::getline(ifile, line);
            std::getline(ifile, line);

            int skip_ctr = 0; 
            while (std::getline(ifile, line)) {
                // std::cout << line << std::endl;
                std::stringstream ss(line);
                   
                std::string piece;
                while (std::getline(ss, piece, ' ')) {
                    if (skip_ctr == 0) {
                        if (img_matrix.back().size() >= IMAGE_SIZE) {
                            img_matrix.push_back({});
                        }

                        img_matrix.back().push_back(std::stof(piece)/255.0);
                        skip_ctr++;

                    } else if (skip_ctr == 1) {
                        skip_ctr ++;
                    } else if (skip_ctr >= 2) {
                        skip_ctr = 0;
                    }
                }
            }
            // std::getline(ifile, line);
            // std::cout << img_matrix.size() << std::endl;

            ifile.close();
            // return;
        }
    }
}

int main(void) {

    construct_bases();
    return 0;
}