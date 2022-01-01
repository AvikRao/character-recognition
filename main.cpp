#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/SVD>
#include <algorithm>
#define IMAGE_SIZE 28
#define BASIS_SIZE 28

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

void add_column_to_matrix(Eigen::MatrixXf &matrix, std::vector<float> &column) {

    matrix.conservativeResize(Eigen::NoChange, matrix.cols() + 1);
    matrix.col(matrix.cols() - 1) = Eigen::Map<Eigen::VectorXf>(column.data(), column.size());

}

std::vector<Eigen::MatrixXf> get_image_matrices() {

    std::vector<Eigen::MatrixXf> image_matrices;
    std::string basis_dir("./basis_images/training/");

    std::cout << "Constructing matrices for each image...\n";

    for (char c : CHARACTERS) {

        Eigen::MatrixXf character_vectors(IMAGE_SIZE*IMAGE_SIZE, 0);
        std::string letter_dir = basis_dir + c;

        for (auto &ifile_name : std::filesystem::directory_iterator(letter_dir)) {

            std::vector<std::vector<float>> img_matrix;
            img_matrix.push_back({});
            std::string line;


            // std::cout << ifile_name.path() << std::endl;
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
            // std::cout << img_matrix.size() << std::endl;

            std::vector<float> vectorized_image;

            for (size_t i = 0; i < IMAGE_SIZE; i++) {
                for (size_t j = 0; j < IMAGE_SIZE; j++) {
                    vectorized_image.push_back(img_matrix.at(j).at(i));
                }
            }

            add_column_to_matrix(character_vectors, vectorized_image);
        }

        image_matrices.push_back(character_vectors);
    }

    std::cout << "Done constructing images.\n";

    return image_matrices;
} 

std::vector<Eigen::MatrixXf> construct_bases(std::vector<Eigen::MatrixXf> image_matrices) {

    std::vector<Eigen::MatrixXf> bases;

    std::cout << "Constructing character bases...\n";

    for (Eigen::MatrixXf &image_matrix : image_matrices) {
        Eigen::BDCSVD svd = image_matrix.bdcSvd(Eigen::ComputeThinU);
        Eigen::MatrixXf character_basis = svd.matrixU();
        // std::cout << character_basis.col(0) << std::endl;
        // Eigen::VectorXf sings = svd.singularValues();
        // float preserve = 0.0;
        // float total = 0.0;
        // for (size_t i = 0; i < svd.nonzeroSingularValues(); i++) {
        //     if (i < 28) {
        //         preserve += sings(i)*sings(i);
        //     }
        //     total += sings(i)*sings(i);
        // }

        // std::cout << preserve/total << std::endl;
        return bases;
    }

    std::cout << "Done constructing bases.\n";

    return bases;
}

int main(void) {

    std::vector<Eigen::MatrixXf> image_matrices = get_image_matrices();
    construct_bases(image_matrices);
    return 0;
}