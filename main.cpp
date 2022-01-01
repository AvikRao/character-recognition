#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/SVD>
#define IMAGE_SIZE 28

const std::string CHARACTERS("0123456789");

// Convert a 2-D vector<vector<double> > into an Eigen MatrixXd.
// Throws exception if rows do not have same length.
Eigen::MatrixXf convert_vvd_to_matrix(std::vector<std::vector<float> > vvd) {

    Eigen::MatrixXf result(IMAGE_SIZE, IMAGE_SIZE);
    result.row(0) = Eigen::VectorXf::Map(&vvd[0][0], IMAGE_SIZE);

    // Add each vector row to the MatrixXd. 
    for (std::size_t i = 1; i < IMAGE_SIZE; i++) {

        result.row(i) = Eigen::VectorXf::Map(&vvd[i][0], IMAGE_SIZE);
    }

    return result;
}

void construct_bases() {

    std::vector<Eigen::MatrixXf> bases;

    std::string basis_dir("./basis_images/training/");
    for (char c : CHARACTERS) {
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

            ifile.close();
            std::cout << img_matrix.size() << std::endl;

            Eigen::MatrixXf eigen_matrix = convert_vvd_to_matrix(img_matrix);
            bases.push_back(eigen_matrix);
        }
    }
}

int main(void) {

    construct_bases();
    return 0;
}