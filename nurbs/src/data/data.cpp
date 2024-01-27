#include <data/data.hpp>

namespace nurbs {

void assign_data_11(std::vector<RationalCurve<ScalarType>> &curves)
{
    RationalCurve<ScalarType> crv; // Planar curve using float32
    crv.control_points = {
        glm::vec3(0, 0, 5), // std::vector of 3D points
        glm::vec3(0, 3, 4),
        glm::vec3(0, 4, 3),
        glm::vec3(0, 5, 6),
        glm::vec3(0, 7, 5),
        glm::vec3(0, 11, 3),
        glm::vec3(0, 13, 4),
        glm::vec3(0, 15, 5),

    };
    crv.knots = {0, 0, 0, 0, 1.0 / 5, 2.0 / 5, 3.0 / 5, 4.0 / 5, 1, 1, 1, 1}; // std::vector of floats

    crv.degree = 3;

    crv.weights = {0.2, 0.3, 0.4, 1, 0.5, 0.6, 0.7, 0.8};

    RationalCurve<ScalarType> crv1; // Planar curve using float32
    crv1.control_points = {glm::fvec3(0, 0, 5), glm::fvec3(0, 3, 5), glm::fvec3(0, 4, 5), glm::fvec3(0, 5, 5), glm::fvec3(0, 7, 5), glm::fvec3(0, 11, 5), glm::fvec3(0, 13, 5), glm::fvec3(0, 15, 5)};
    crv1.knots = {0, 0, 0, 0, 0.2, 0.4, 0.6, 0.8, 1, 1, 1, 1};
    crv1.degree = 3;
    crv1.weights = {1, 1, 1, 1, 1, 1, 1, 1};

    RationalCurve<ScalarType> crv2; // Planar curve using float32
    crv2.control_points = {glm::fvec3(2, 0, 5), glm::fvec3(2, 3, 4), glm::fvec3(2, 4, 3), glm::fvec3(2, 5, 4), glm::fvec3(2, 7, 6), glm::fvec3(2, 11, 5), glm::fvec3(2, 13, 3), glm::fvec3(2, 15, 5)};
    crv2.knots = {0, 0, 0, 0, 0.2, 0.4, 0.6, 0.8, 1, 1, 1, 1};
    crv2.degree = 3;
    crv2.weights = {1, 1, 1, 1, 1, 1, 1, 1};

    RationalCurve<ScalarType> crv3; // Planar curve using float32
    crv3.control_points = {glm::fvec3(3, 0, 5), glm::fvec3(3, 3, 3), glm::fvec3(3, 4, 1), glm::fvec3(3, 5, 4), glm::fvec3(3, 7, 7), glm::fvec3(3, 11, 9), glm::fvec3(3, 13, 7), glm::fvec3(3, 15, 5)};
    crv3.knots = {0, 0, 0, 0, 0.2, 0.4, 0.6, 0.8, 1, 1, 1, 1};
    crv3.degree = 3;
    crv3.weights = {1, 1, 1, 1, 1, 1, 1, 1};

    RationalCurve<ScalarType> crv4; // Planar curve using float32
    crv4.control_points = {glm::fvec3(5, 0, 5), glm::fvec3(5, 3, 4), glm::fvec3(5, 4, 3), glm::fvec3(5, 5, 3), glm::fvec3(5, 7, 2), glm::fvec3(5, 11, 3), glm::fvec3(5, 13, 4), glm::fvec3(5, 15, 5)};
    crv4.knots = {0, 0, 0, 0, 0.2, 0.4, 0.6, 0.8, 1, 1, 1, 1};
    crv4.degree = 3;
    crv4.weights = {1, 1, 1, 1, 1, 1, 1, 1};

    RationalCurve<ScalarType> crv5; // Planar curve using float32
    crv5.control_points = {glm::fvec3(8, 0, 5), glm::fvec3(8, 3, 5), glm::fvec3(8, 4, 5), glm::fvec3(8, 5, 5), glm::fvec3(8, 7, 5), glm::fvec3(8, 11, 5), glm::fvec3(8, 13, 5), glm::fvec3(8, 15, 5)};
    crv5.knots = {0, 0, 0, 0, 0.2, 0.4, 0.6, 0.8, 1, 1, 1, 1};
    crv5.degree = 3;
    crv5.weights = {1, 1, 1, 1, 1, 1, 1, 1};

    // assign data_11
    curves.push_back(crv1);
    curves.push_back(crv2);
    curves.push_back(crv3);
    curves.push_back(crv4);
    curves.push_back(crv5);
}


void assign_data_12(std::vector<RationalCurve<ScalarType>> &curves)
{
    std::vector<glm::vec<3, ScalarType>> P0 = {{0, 0, 0}, {2, -2, -1}, {4, -4, -2}, {5, -2, -3}, {7, -1, -4}, {8, 0, -5}, {10, 1, -6}, {11, 0, -7}};
    std::vector<glm::vec<3, ScalarType>> P1 = {{0, 4, 3}, {2, 5, 4}, {5, 1, 3}, {6, 2, 0}, {8, 4, -4}, {9, 3, -6}, {11, 4, -8}, {12, 4, -9}};
    std::vector<glm::vec<3, ScalarType>> P2 = {{0, 8, 6}, {3, 9, 7}, {5, 8, 5}, {6, 8, -1}, {8, 9, -5}, {10, 7, -7}, {12, 9, -10}, {13, 8, -12}};
    std::vector<glm::vec<3, ScalarType>> P3 = {{1, 12, 2}, {3, 13, 4}, {6, 14, 3}, {7, 14, 0}, {8, 13, -4}, {10, 11, -5}, {12, 14, -8}, {13, 12, -10}};
    std::vector<glm::vec<3, ScalarType>> P4 = {{2, 16, 0}, {3, 17, -1}, {6, 20, -2}, {7, 19, -3}, {8, 18, -4}, {11, 17, -5}, {13, 17, -6}, {14, 16, -7}};
    std::vector<glm::vec<3, ScalarType>> control_points_arr[] = { P0, P1, P2, P3, P4 };
    std::vector<ScalarType> W0 = {1.0, 1.1, 1.0, 0.9, 0.9, 1.0, 1.0, 1.0};
    std::vector<ScalarType> W1 = {1.0, 1.1, 1.0, 0.9, 0.9, 1.0, 1.0, 1.0};
    std::vector<ScalarType> W2 = {1.0, 1.2, 1.2, 1.0, 1.0, 1.0, 1.0, 1.0};
    std::vector<ScalarType> W3 = {1.0, 1.2, 1.2, 1.0, 1.0, 1.0, 1.0, 1.0};
    std::vector<ScalarType> W4 = {1.0, 1.1, 1.1, 1.1, 1.1, 1.0, 1.0, 1.0};
    std::vector<ScalarType> weights_arr[] = { W0, W1, W2, W3, W4 };

    for (int i = 0; i < 5; i++) {
        RationalCurve<ScalarType> crv;
        crv.control_points = control_points_arr[i];
        crv.knots = {0, 0, 0, 0, 0.2, 0.4, 0.6, 0.8, 1, 1, 1, 1};
        crv.degree = 3;
        crv.weights = weights_arr[i];

        curves.push_back(crv);
    }
}


void assign_data_21(std::vector<RationalCurve<ScalarType>> &curves) {
    std::vector<ScalarType> C0 = {0, 0, 0, 0, (ScalarType)1/3, (ScalarType)1/3, (ScalarType)1/3, (ScalarType)2/3, 1, 1, 1, 1};
    std::vector<ScalarType> C1 = {0, 0, 0, 0, (ScalarType)1/5, (ScalarType)1/4, (ScalarType)1/3, (ScalarType)1/2, 1, 1, 1, 1};
    std::vector<ScalarType> C2 = {0, 0, 0, 0, (ScalarType)1/5, (ScalarType)2/5, (ScalarType)3/5, (ScalarType)4/5, 1, 1, 1, 1};
    std::vector<ScalarType> C3 = {0, 0, 0, 0, (ScalarType)1/2, (ScalarType)2/3, (ScalarType)3/4, (ScalarType)4/5, 1, 1, 1, 1};
    std::vector<ScalarType> C4 = {0, 0, 0, 0, (ScalarType)1/3, (ScalarType)2/3, (ScalarType)2/3, (ScalarType)2/3, 1, 1, 1, 1};
    std::vector<ScalarType> knots_arr[] = { C0, C1, C2, C3, C4 };

    std::vector<glm::vec<3, ScalarType>> P0 = {{0, 0, 5}, {0, 3, 5}, {0, 4, 5}, {0, 5, 5}, {0, 7, 5}, {0, 11, 5}, {0, 13, 5}, {0, 15, 5}};
    std::vector<glm::vec<3, ScalarType>> P1 = {{2, 0, 5}, {2, 3, 4}, {2, 4, 3}, {2, 5, 4}, {2 ,7, 6}, {2, 11, 5}, {2, 13, 3}, {2, 15, 5}};
    std::vector<glm::vec<3, ScalarType>> P2 = {{3, 0, 5}, {3, 3, 3}, {3, 4, 1}, {3, 5, 4}, {3, 7, 7}, {3, 11, 9}, {3, 13, 7}, {3, 15, 5}};
    std::vector<glm::vec<3, ScalarType>> P3 = {{5, 0, 5}, {5, 3, 4}, {5, 4, 3}, {5, 5, 3}, {5, 7, 2}, {5, 11, 3}, {5, 13, 4}, {5, 15, 5}};
    std::vector<glm::vec<3, ScalarType>> P4 = {{8, 0, 5}, {8, 3, 5}, {8, 4, 5}, {8, 5, 5}, {8, 7, 5}, {8, 11, 5}, {8, 13, 5}, {8, 15, 5}};
    std::vector<glm::vec<3, ScalarType>> control_points_arr[] = { P0, P1, P2, P3, P4 };

    std::vector<ScalarType> W0 = {1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<ScalarType> W1 = {1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<ScalarType> W2 = {1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<ScalarType> W3 = {1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<ScalarType> W4 = {1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<ScalarType> weights_arr[] = { W0, W1, W2, W3, W4 };

    for (int i = 0; i < 5; i++) {
        RationalCurve<ScalarType> crv;
        crv.control_points = control_points_arr[i];
        crv.knots = knots_arr[i];
        crv.degree = 3;
        crv.weights = weights_arr[i];

        curves.push_back(crv);
    }
}


void assign_data_22(std::vector<RationalCurve<ScalarType>> &curves) {
    std::vector<ScalarType> C0 = {0, 0, 0, 0, (ScalarType)1/3, (ScalarType)1/3, (ScalarType)1/3, (ScalarType)2/3, 1, 1, 1, 1};
    std::vector<ScalarType> C1 = {0, 0, 0, 0, (ScalarType)1/5, (ScalarType)1/4, (ScalarType)1/3, (ScalarType)1/2, 1, 1, 1, 1};
    std::vector<ScalarType> C2 = {0, 0, 0, 0, (ScalarType)1/5, (ScalarType)2/5, (ScalarType)3/5, (ScalarType)4/5, 1, 1, 1, 1};
    std::vector<ScalarType> C3 = {0, 0, 0, 0, (ScalarType)1/2, (ScalarType)2/3, (ScalarType)3/4, (ScalarType)4/5, 1, 1, 1, 1};
    std::vector<ScalarType> C4 = {0, 0, 0, 0, (ScalarType)1/3, (ScalarType)2/3, (ScalarType)2/3, (ScalarType)2/3, 1, 1, 1, 1};
    std::vector<ScalarType> knots_arr[] = { C0, C1, C2, C3, C4 };

    std::vector<glm::vec<3, ScalarType>> P0 = {{0, 0, 0}, {2, -2, -1}, {4, -4, -2}, {5, -2, -3}, {7, -1, -4}, {8, 0, -5}, {10, 1, -6}, {11, 0, -7}};
    std::vector<glm::vec<3, ScalarType>> P1 = {{0, 2, 3}, {2, 3, 4}, {5, -1, 3}, {6, 0, 0}, {8 , 2, -4}, {9, 1, -6}, {11, 2, -8}, {12, 2, -6}};
    std::vector<glm::vec<3, ScalarType>> P2 = {{0, 4, 6}, {3, 5, 7}, {5, 4, 5}, {6, 4, -1}, {8, 5, -5}, {10, 3, -7}, {12, 5, -10}, {13, 4, -12}};
    std::vector<glm::vec<3, ScalarType>> P3 = {{1, 6, 2}, {3, 7, 4}, {6, 8, 3}, {7, 8, 0}, {8, 7, -4}, {10, 5, -5}, {12, 8, -8}, {13, 6, -10}};
    std::vector<glm::vec<3, ScalarType>> P4 = {{2, 8, 0}, {3, 9, -1}, {6, 12, -2}, {7, 11, -3}, {8, 10, -4}, {11, 9, -5}, {13, 9, -6}, {14, 8, -7}};
    std::vector<glm::vec<3, ScalarType>> control_points_arr[] = { P0, P1, P2, P3, P4 };

    std::vector<ScalarType> W0 = {1.0, 0.5, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    std::vector<ScalarType> W1 = {1.0, 0.6, 0.5, 0.5, 0.6, 0.7, 0.8, 1.0};
    std::vector<ScalarType> W2 = {1.0, 0.7, 0.6, 0.5, 0.5, 0.6, 0.7, 1.0};
    std::vector<ScalarType> W3 = {1.0, 0.8, 0.7, 0.6, 0.5, 0.5, 0.6, 1.0};
    std::vector<ScalarType> W4 = {1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.5, 1.0};
    std::vector<ScalarType> weights_arr[] = { W0, W1, W2, W3, W4 };

    for (int i = 0; i < 5; i++) {
        RationalCurve<ScalarType> crv;
        crv.control_points = control_points_arr[i];
        crv.knots = knots_arr[i];
        crv.degree = 3;
        crv.weights = weights_arr[i];

        curves.push_back(crv);
    }
}

}