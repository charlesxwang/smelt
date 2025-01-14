#include <cmath>
#include <iostream>
#include <vector>
#include <catch/catch.hpp>
#include <Eigen/Dense>
#include "numeric_utils.h"

TEST_CASE("Test correlation to covariance functionality", "[Helpers]") {
  SECTION("Correlation is diagonal matrix with values of 1.0 along diagonal") {
    Eigen::MatrixXd test_corr = Eigen::MatrixXd::Zero(3, 3);
    test_corr(0, 0) = 1.0;
    test_corr(1, 1) = 1.0;
    test_corr(2, 2) = 1.0;
    Eigen::MatrixXd std_dev = Eigen::VectorXd::Ones(3);

    std_dev << 2.0, 3.0, 4.0;

    auto cov = numeric_utils::corr_to_cov(test_corr, std_dev);

    Eigen::MatrixXd expected_matrix = Eigen::MatrixXd::Zero(3, 3);
    expected_matrix(0, 0) = 4.0;
    expected_matrix(1, 1) = 9.0;
    expected_matrix(2, 2) = 16.0;

    REQUIRE(cov(0, 0) == expected_matrix(0, 0));
    REQUIRE(cov(1, 1) == expected_matrix(1, 1));
    REQUIRE(cov(2, 2) == expected_matrix(2, 2));
    REQUIRE(expected_matrix.lpNorm<2>() == Approx(cov.lpNorm<2>()).epsilon(0.01));
  }
}

TEST_CASE("Test one dimensional convolution", "[Helpers][Convolution]") { 
  SECTION("One dimensional convolution of vectors with length 1") {
    std::vector<double> input_x{1.0};
    std::vector<double> input_y{2.0};
    std::vector<double> response;

    bool status;
    try {
      status = numeric_utils::convolve_1d(input_x, input_y, response);
    } catch (std::exception &exception) {
      std::cout << "Convolution error: " << exception.what() << std::endl;      
      FAIL("One dimensional convolution function throws exception, check where "
           "exception is being generated interally");
    }

    REQUIRE(status);
    REQUIRE(response.size() == 1);
    REQUIRE(response[0] == Approx(2.0).epsilon(0.01));
  }

  SECTION("One dimensional convolution of vectors with length 2 and 3") {
    std::vector<double> input_x{3.0, 4.0, 5.0};
    std::vector<double> input_y{2.0, 1.0};
    std::vector<double> response(1);

    bool status;
    try {
      status = numeric_utils::convolve_1d(input_x, input_y, response);
    } catch (std::exception &exception) {
      std::cout << "Convolution error: " << exception.what() << std::endl;      
      FAIL("One dimensional convolution function throws exception, check where "
           "exception is being generated interally");
    }

    REQUIRE(status);
    REQUIRE(response.size() == input_x.size() + input_y.size() - 1);
    REQUIRE(response[0] == Approx(6.0).epsilon(0.01));
    REQUIRE(response[1] == Approx(11.0).epsilon(0.01));
    REQUIRE(response[2] == Approx(14.0).epsilon(0.01));
    REQUIRE(response[3] == Approx(5.0).epsilon(0.01));
  }  
}

TEST_CASE("Test trapazoid rule", "[Helpers][Trapazoid]") {

  SECTION("STL vector with unit spacing") {
    std::vector<double> input_vector{1, 4, 9, 16, 25};

    auto integral = numeric_utils::trapazoid_rule(input_vector, 1.0);
    REQUIRE(integral == 42);
  }

  SECTION("STL vector with non-unit spacing") {
    std::vector<double> input_vector(101, 0.0);

    double accumulator = 0.0;
    for (unsigned int i = 1; i < input_vector.size(); ++i) {
      accumulator += M_PI / 100.0;
      input_vector[i] = std::sin(accumulator);
    }

    auto integral = numeric_utils::trapazoid_rule(input_vector, M_PI / 100.0);
    REQUIRE(integral == Approx(1.9998).epsilon(0.01));
  }

  SECTION("Eigen vector with unit spacing") {
    Eigen::VectorXd input_vector(5);
    input_vector << 1, 4, 9, 16, 25;

    auto integral = numeric_utils::trapazoid_rule(input_vector, 1.0);
    REQUIRE(integral == 42);
  }

  SECTION("Eigen vector with non-unit spacing") {
    Eigen::VectorXd input_vector = Eigen::VectorXd::Zero(101);

    double accumulator = 0.0;
    for (unsigned int i = 1; i < input_vector.size(); ++i) {
      accumulator += M_PI / 100.0;
      input_vector[i] = std::sin(accumulator);
    }

    auto integral = numeric_utils::trapazoid_rule(input_vector, M_PI / 100.0);
    REQUIRE(integral == Approx(1.9998).epsilon(0.01));
  }    
}

TEST_CASE("Test 1-D inverse Fast Fourier Transform", "[Helpers][FFT]") {
  SECTION("Calculate real portion of one-dimesional inverse FFT") {
    std::vector<std::complex<double>> input_vector = {
        {15.0, 0.0},
        {-2.5, 3.440954801177933},
        {-2.5, 0.812299240582266},
        {-2.5, -0.812299240582266},
        {-2.5, -3.440954801177933}};

    std::vector<double> output_vector(4);
    auto status = numeric_utils::inverse_fft(input_vector, output_vector);

    REQUIRE(status);
    REQUIRE(output_vector[0] == Approx(1.0).epsilon(0.01));
    REQUIRE(output_vector[1] == Approx(2.0).epsilon(0.01));
    REQUIRE(output_vector[2] == Approx(3.0).epsilon(0.01));
    REQUIRE(output_vector[3] == Approx(4.0).epsilon(0.01));
    REQUIRE(output_vector[4] == Approx(5.0).epsilon(0.01));
  }

  SECTION("Calculate real portion of one-dimesional inverse FFT") {
    Eigen::VectorXcd input_vector(5);
    input_vector << std::complex<double>(15.0, 0.0),
      std::complex<double>(-2.5, 3.440954801177933),
      std::complex<double>(-2.5, 0.812299240582266),
      std::complex<double>(-2.5, -0.812299240582266),
      std::complex<double>(-2.5, -3.440954801177933);

    Eigen::VectorXd output_vector;
    auto status = numeric_utils::inverse_fft(input_vector, output_vector);

    REQUIRE(status);
    REQUIRE(output_vector[0] == Approx(1.0).epsilon(0.01));
    REQUIRE(output_vector[1] == Approx(2.0).epsilon(0.01));
    REQUIRE(output_vector[2] == Approx(3.0).epsilon(0.01));
    REQUIRE(output_vector[3] == Approx(4.0).epsilon(0.01));
    REQUIRE(output_vector[4] == Approx(5.0).epsilon(0.01));
  }  
}
