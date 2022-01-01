#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <random>
#include <limits>
#include <ctime>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/SVD>
#include <eigen3/Eigen/Dense>

#define IMAGE_SIZE 28
#define BASIS_SIZE 28
#define TEST_COUNT 10

const std::string CHARACTERS("0123456789");

Eigen::VectorXf std_to_eigen(std::vector<float> &v) {
    return Eigen::Map<Eigen::VectorXf>(v.data(), v.size());
}

void add_column_to_matrix(Eigen::MatrixXf &matrix, Eigen::VectorXf column) {
    matrix.conservativeResize(Eigen::NoChange, matrix.cols() + 1);
    matrix.col(matrix.cols() - 1) = column;
}

Eigen::VectorXf vectorize_image(std::filesystem::directory_entry filename) {

    std::vector<std::vector<float>> img_matrix;
    img_matrix.push_back({});
    std::string line;


    // std::cout << ifile_name.path() << std::endl;
    std::ifstream ifile(filename.path());

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

    return std_to_eigen(vectorized_image);
}

std::vector<Eigen::MatrixXf> get_image_matrices() {

    std::vector<Eigen::MatrixXf> image_matrices;
    std::string basis_dir("./basis_images/training/");

    std::cout << "Constructing matrices for each image...\n";

    for (char c : CHARACTERS) {

        Eigen::MatrixXf character_vectors(IMAGE_SIZE*IMAGE_SIZE, 0);
        std::string letter_dir = basis_dir + c;

        for (auto &ifile_name : std::filesystem::directory_iterator(letter_dir)) {

            Eigen::VectorXf vectorized = vectorize_image(ifile_name);
            add_column_to_matrix(character_vectors, vectorized);
        }

        image_matrices.push_back(character_vectors);
    }

    std::cout << "Done constructing images.\n";

    return image_matrices;
} 

std::vector<Eigen::MatrixXf> construct_bases(std::vector<Eigen::MatrixXf> image_matrices) {

    std::vector<Eigen::MatrixXf> bases;

    std::cout << "Constructing character bases...\n";

    int i = 0;
    for (Eigen::MatrixXf &image_matrix : image_matrices) {
        Eigen::BDCSVD svd = image_matrix.bdcSvd(Eigen::ComputeThinU);
        Eigen::MatrixXf character_U = svd.matrixU();
        bases.push_back(character_U.leftCols(BASIS_SIZE));
        
        // std::cout << "Constructed base for " << i++ << std::endl;
    }

    std::cout << "Done constructing bases.\n";

    return bases;
}

int pick_random_fileno(std::string dir) {

    auto dir_iter = std::filesystem::directory_iterator(dir);
    int fileCount = std::count_if(
        begin(dir_iter),
        end(dir_iter),
        [](auto& entry) { return entry.is_regular_file(); }
    );

    return rand() % fileCount;
}

double basis_distance(Eigen::MatrixXf &basis, Eigen::VectorXf &char_vector) {
    return (char_vector - basis*basis.transpose()*char_vector).norm();
}

int make_prediction(std::vector<Eigen::MatrixXf> &character_bases, Eigen::VectorXf input) {

    int min_basis = 0;
    double min_distance = std::numeric_limits<double>::max();
    for (int i = 0; i < 10; i++) {
        Eigen::MatrixXf basis = character_bases.at(i);
        double distance = basis_distance(basis, input);
        if (distance < min_distance) {
            min_basis = i;
            min_distance = distance;
        }
    }

    return min_basis;
}

int main(void) {

    std::srand(std::time(nullptr));

    std::vector<Eigen::MatrixXf> image_matrices = get_image_matrices();
    std::vector<Eigen::MatrixXf> character_bases = construct_bases(image_matrices);

    std::string testing_dir = "./basis_images/testing/";

    for (int i = 0; i < TEST_COUNT; i++) {

        std::string correct = std::to_string(rand() % 10);
        std::string character_dir = testing_dir + correct;

        std::cout << "\nPicking " << correct << ": ";

        int random_fileno = pick_random_fileno(character_dir);
        int count = 0;
        for (auto &ifile_name : std::filesystem::directory_iterator(character_dir)) {
            if (count++ == random_fileno) {

                std::cout << ifile_name.path().filename() << std::endl;
                Eigen::VectorXf vectorized_test = vectorize_image(ifile_name);

                int prediction = make_prediction(character_bases, vectorized_test);
                std::cout << "Prediction: " << prediction << std::endl;
                break;
            }
        }

    }
    return 0;
}