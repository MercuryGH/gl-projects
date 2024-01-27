#pragma once

#include <tinynurbs.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>

#include <util/types.hpp>
#include <util/math.hpp>

namespace Eigen {

template<typename ScalarType>
using MatrixX = Eigen::Matrix<ScalarType, -1, -1>;

template<typename ScalarType>
using VectorX = Eigen::Matrix<ScalarType, -1, 1>;

}

namespace nurbs {
    using namespace tinynurbs;
    using std::vector;
    using std::pair;

namespace {
    vector<Vector4> lift_to_homogenous(const vector<Vector3>& control_points, const vector<ScalarType>& weights) {
        vector<Vector4> ret(control_points.size());
        for (int i = 0; i < control_points.size(); i++) {
            ret.at(i).x = control_points.at(i).x * weights.at(i);
            ret.at(i).y = control_points.at(i).y * weights.at(i);
            ret.at(i).z = control_points.at(i).z * weights.at(i);
            ret.at(i).w = weights.at(i);
        }
        return ret;
    }

    array2<Vector4> lift_to_homogenous(const array2<Vector3>& control_points, const array2<ScalarType>& weights) {
        array2<Vector4> ret(control_points.rows(), control_points.cols());
        for (int i = 0; i < control_points.rows(); i++) {
            for (int j = 0; j < control_points.cols(); j++) {
                ret(i, j).x = control_points(i, j).x * weights(i, j);
                ret(i, j).y = control_points(i, j).y * weights(i, j);
                ret(i, j).z = control_points(i, j).z * weights(i, j);
                ret(i, j).w = weights(i, j);
            }
        }
        return ret;
    }

    Vector3 lower_to_cartesian(Vector4 v) {
        Vector3 ret(v.x, v.y, v.z);
        if (v.w != 0.0) {
            ret /= v.w;
        }
        return ret;
    }
    /**
     * Find the knot interval
     * ref: Algorithm A2.1
    */
    template<typename ScalarType>
    int knot_span_index_bin_search(ScalarType u, int degree, const vector<ScalarType>& knots) {
        const int n = knots.size() - degree - 2; // index of last control point

        if (std::abs(u - knots.at(n + 1)) < k_eps) { // last knot value
            return n;
        }

        int l = degree;
        int r = n + 1;
        int mid = (l + r) / 2;

        while (u < knots.at(mid) || u >= knots.at(mid + 1)) {
            if (u < knots.at(mid)) {
                r = mid;
            }
            else {
                l = mid;
            }
            mid = (l + r) / 2;
        }
        return mid;
    }

    /**
     * De-boor algorithm
     * return N_{i, 0}(u), N_{i, 1}(u), ..., N_{i, p}(u)
     * ref: Algorithm A2.2
     * index is the index of basis function (i)
     * constraint: index + 1 >= degree
    */
    vector<ScalarType> basis_functions(ScalarType u, int index, int degree, const vector<ScalarType>& knots) {
        assert(index + 1 >= degree); 

        vector<ScalarType> vals(degree + 1, 0.0);
        vals.at(0) = 1.0;

        vector<ScalarType> u_minus_ui_term(degree + 1, 0.0); // u - u_i 
        vector<ScalarType> uip1_minus_u_term(degree + 1, 0.0); // u_{i+p+1} - u
        for (int j = 1; j <= degree; j++) { // N_{i, p} second param p
            u_minus_ui_term.at(j) = u - knots.at(index + 1 - j);
            uip1_minus_u_term.at(j) = knots.at(index + j) - u;
            ScalarType left_i_term = 0.0;
            for (int r = 0; r < j; r++) { // downcounting i
                ScalarType cur_term = vals.at(r) / (uip1_minus_u_term.at(r + 1) + u_minus_ui_term.at(j - r));
                ScalarType right_i_inc_term = uip1_minus_u_term.at(r + 1) * cur_term;

                vals.at(r) = left_i_term + right_i_inc_term;

                left_i_term = u_minus_ui_term.at(j - r) * cur_term; // save for next (i + 1 -> i)
            }

            vals.at(j) = left_i_term; // right is 0
        }

        return vals;
    }

    /**
     * construct v_k for v-directional interpolations
     * ref: Equation 10.8
    */
    vector<ScalarType> construct_vk_by_chord_length(
        const vector<RationalCurve<ScalarType>>& crvs
    ) {
        const int n = crvs.at(0).control_points.size() - 1;

        vector<ScalarType> vk(crvs.size(), 0.0);
        vk.at(crvs.size() - 1) = 1.0;
        vector<ScalarType> d(n + 1, 0.0);

        // initialize d_i
        for (int i = 0; i <= n; i++) {
            for (int k = 1; k < crvs.size(); k++) {
                d.at(i) += glm::distance(crvs.at(k).control_points.at(i), crvs.at(k - 1).control_points.at(i));
            }
        }

        // compute v_k
        for (int k = 1; k < crvs.size() - 1; k++) {
            ScalarType sum = 0.0;
            for (int i = 0; i <= n; i++) {
                sum += glm::distance(crvs.at(k).control_points.at(i), crvs.at(k - 1).control_points.at(i)) / d.at(i);
            }
            vk.at(k) = vk.at(k - 1) + (1.0 / (n + 1.0)) * sum;
        }

        return vk;
    }

    /**
     * construct knot vector using v_k
     * ref: Equation 9.8
    */
    vector<ScalarType> construct_knots_by_averging(
        int n_control_points,
        int degree,
        const vector<ScalarType>& vk
    ) {
        // n is the number of control_points (in v dir) - 1, which is the number of interpolated curves - 1
        const int n_v = n_control_points - 1;
        const int m = degree + n_v + 1;
        vector<ScalarType> knots(m + 1, 0.0);
        for (int i = m - degree; i <= m; i++) {
            knots.at(i) = 1.0;
        }

        for (int j = 1; j <= n_v - degree; j++) {
            ScalarType tmp = 0.0;
            for (int i = j; i <= j + degree - 1; i++) {
                tmp += vk.at(i);
            }
            knots.at(j + degree) = tmp / degree;
        } 

        return knots;
    }

    /**
     * Knot refinement can be accomplished by multiple applications of knot insertion. 
     * However, we distinguish the two problems here 
     * because there exist more efficient algorithms for knot refinement.
     * Note: Algorithm for single knot insertion is not implmented.
     * ref: Algorithm A5.4
    */
    pair<vector<ScalarType>, vector<Vector4>> knot_refinement(
        int degree,
        const vector<ScalarType>& original_knots,
        const vector<Vector4>& original_control_points,
        const vector<ScalarType>& refined_knots // X, the inserted knots that to be refined
    ) {
        const int n = original_control_points.size() - 1;
        const int r = refined_knots.size() - 1; // index of last knot to be inserted
        const int m = n + degree + 1;
        const int a = knot_span_index_bin_search(refined_knots.at(0), degree, original_knots);
        const int b = knot_span_index_bin_search(refined_knots.at(r), degree, original_knots) + 1;

        vector<ScalarType> new_knots(original_knots.size() + refined_knots.size());
        vector<Vector4> new_control_points(original_control_points.size() + refined_knots.size());

        for (int j = 0; j <= a - degree; j++) {
            new_control_points.at(j) = original_control_points.at(j);
        }
        for (int j = b - 1; j <= n; j++) {
            new_control_points.at(j + r + 1) = original_control_points.at(j);
        }
        for (int j = 0; j <= a; j++) {
            new_knots.at(j) = original_knots.at(j);
        }
        for (int j = b + degree; j <= m; j++) {
            new_knots.at(j + r + 1) = original_knots.at(j);
        }

        int i = b + degree - 1;
        int k = b + degree + r;
        for (int j = r; j >= 0; j--, k--) {
            while (refined_knots.at(j) <= original_knots.at(i) && i > a) {
                new_control_points.at(k - degree - 1) = original_control_points.at(i - degree - 1);
                new_knots.at(k) = original_knots.at(i);
                k--;
                i--;
            }

            new_control_points.at(k - degree - 1) = new_control_points.at(k - degree);

            for (int l = 1; l <= degree; l++) {
                int ind = k - degree + l;

                ScalarType t = new_knots.at(k + l) - refined_knots.at(j);
                if (std::abs(t) < k_eps) {
                    new_control_points.at(ind - 1) = new_control_points.at(ind);
                } else {
                    t /= (new_knots.at(k + l) - original_knots.at(i - degree + l));

                    // lerp
                    new_control_points.at(ind - 1) = t * new_control_points.at(ind - 1) + (1.0f - t) * new_control_points.at(ind);
                }
            }

            new_knots.at(k) = refined_knots.at(j);
        }

        return { new_knots, new_control_points };
    }

    /**
     * interpolate a given point set, return a new control point set.
     * ref: Section 9.2.1, Algorithm A9.1
    */
    vector<Vector4> interpolate_homogeneous_points(
        const vector<Vector4>& points, // to be interpolated
        int degree, // target degree
        const vector<ScalarType>& u_bar, // interpolation params
        const vector<ScalarType>& knots // knot vector
    ) {
        assert(points.size() == u_bar.size());
        vector<Vector4> ret_control_points(points.size());

        const int n = points.size() - 1;
        const int m = degree + n + 1;
        assert(knots.size() == m + 1);

        // construct coefficient matrix
        Eigen::MatrixX<ScalarType> A(n + 1, n + 1);
        A.setZero();
        for (int i = 0; i <= n; i++) { // u_i
            int span_index = knot_span_index_bin_search(u_bar.at(i), degree, knots);
            vector<ScalarType> basis_func_val = basis_functions(u_bar.at(i), span_index, degree, knots); 
            // only fill those non-vanishing values
            for (int k = 0; k <= degree; k++) {
                A(i, span_index - degree + k) = basis_func_val.at(k);
            }
        }

        // LU decomposition 
        Eigen::FullPivLU<decltype(A)> lu(A);

        const auto solve_dir_linear_sys = [&](const auto& lu, int component_index) {
            Eigen::VectorX<ScalarType> b(n + 1);
            for (int i = 0; i <= n; i++) {
                b(i) = points.at(i)[component_index];
            }

            // solve
            Eigen::VectorX<ScalarType> x = lu.solve(b);
            
            for (int i = 0; i <= n; i++) {
                ret_control_points.at(i)[component_index] = x(i);
            }
        };

        // solve linear system at each component
        for (int i = 0; i < 4; i++) {
            solve_dir_linear_sys(lu, i);
        }

        return ret_control_points;
    }
}

/**
 * ref: Section 10.3
*/
RationalSurface<ScalarType> skinning(const vector<RationalCurve<ScalarType>>& crvs, int degree, vector<RationalCurve<ScalarType>>& new_crvs) {
    assert(degree < crvs.size());

    // lift control points
    vector<vector<Vector4>> homogenous_control_points(crvs.size());
    for (int k = 0; k < crvs.size(); k++) {
        homogenous_control_points.at(k) = lift_to_homogenous(crvs.at(k).control_points, crvs.at(k).weights);
    }

    // degree elevation
    // not implement...

    /**
     * knot alignment (multiple insertion)
     * if the knot vectors U1 and U2 are not identical, 
     * merge them to obtain the common knot vector 
     * e.g. U1 = {0,0,0,1,2,2,4,4,4} and U2 = {0,0,0,1,2,3,4,4,4},
     * then U = {0,0,0,1,2,2,3,4,4,4};
    */
    vector<ScalarType> common_knots(crvs.at(0).knots);
    for (int k = 1; k < crvs.size(); k++) {

        // sliding ptr
        for (int i = 0; i < crvs.at(k).knots.size(); ) {
            const auto ui = crvs.at(k).knots.at(i);

            // n ui in cur knots
            const auto cur_n_ui = std::count(crvs.at(k).knots.begin(), crvs.at(k).knots.end(), ui);
            const auto common_n_ui = std::count(common_knots.begin(), common_knots.end(), ui);
            const auto diff = cur_n_ui - common_n_ui;
            for (int j = 0; j < diff; j++) {
                common_knots.push_back(ui);
            }
            
            i += cur_n_ui;
        }
    }
    std::sort(common_knots.begin(), common_knots.end());

    // knot refinement based on difference on each curve
    new_crvs.resize(crvs.size());
    for (int k = 0; k < crvs.size(); k++) { 
        const auto& original_knots = crvs.at(k).knots;
        vector<ScalarType> refined_knots;
        for (int i = 0; i < common_knots.size(); ) {
            const auto ui = common_knots.at(i);

            const auto cur_n_ui = std::count(original_knots.begin(), original_knots.end(), ui);
            const auto common_n_ui = std::count(common_knots.begin(), common_knots.end(), ui);
            const auto diff = common_n_ui - cur_n_ui;
            for (int j = 0; j < diff; j++) {
                refined_knots.push_back(ui);
            }

            i += common_n_ui;
        }

        new_crvs.at(k) = crvs.at(k);

        // refine if any difference
        if (refined_knots.empty() == false) {
            auto new_knots_cp = knot_refinement(crvs.at(k).degree, original_knots, homogenous_control_points.at(k), refined_knots);
            auto new_knots = new_knots_cp.first;
            auto new_control_points = new_knots_cp.second;
            homogenous_control_points.at(k) = new_control_points;

            vector<Vector3> cartesian_new_control_points(new_control_points.size());
            vector<ScalarType> new_weights(new_control_points.size());

            for (int i = 0; i < new_control_points.size(); i++) {
                cartesian_new_control_points.at(i) = lower_to_cartesian(new_control_points.at(i));
                new_weights.at(i) = new_control_points.at(i).w;
            }

            new_crvs.at(k).knots = new_knots;
            new_crvs.at(k).control_points = cartesian_new_control_points;
            new_crvs.at(k).weights = new_weights;
        }
    }

    // construct vk
    vector<ScalarType> vk = construct_vk_by_chord_length(new_crvs);

    // n is the number of control_points (in v dir) - 1, which is the number of interpolated curves - 1
    vector<ScalarType> knots = construct_knots_by_averging(new_crvs.size(), degree, vk);

    const int n = new_crvs.at(0).control_points.size() - 1;
    // v-dir interpolation
    vector<vector<Vector4>> suf_control_points(n + 1);

    for (int i = 0; i <= n; i++) { // iterate over one curve's control point index
        suf_control_points.at(i).resize(new_crvs.size());

        vector<Vector4> points; // known control points in v dir
        // fetch all homogenous control points
        for (int k = 0; k < new_crvs.size(); k++) { // iterate over each curve, fetch i-th control point
            points.push_back(homogenous_control_points.at(k).at(i));
        }

        auto ret_control_points = interpolate_homogeneous_points(points, degree, vk, knots);

        for (int k = 0; k < new_crvs.size(); k++) {
            suf_control_points.at(i).at(k) = ret_control_points.at(k);
        }
    }

    // construct the surface

    RationalSurface<ScalarType> srf;
    srf.degree_u = new_crvs.at(0).degree;
    srf.degree_v = degree;
    srf.knots_u = new_crvs.at(0).knots;
    srf.knots_v = knots;

    array2<Vector3> cartesian_suf_control_points(suf_control_points.size(), suf_control_points.at(0).size());
    array2<ScalarType> weights(cartesian_suf_control_points.rows(), cartesian_suf_control_points.cols());
    for (int i = 0; i < cartesian_suf_control_points.rows(); i++) {
        for (int j = 0; j < cartesian_suf_control_points.cols(); j++) {
            cartesian_suf_control_points(i, j) = lower_to_cartesian(
                suf_control_points.at(i).at(j)
            );
            weights(i, j) = suf_control_points.at(i).at(j).w;
        }
    }
    srf.control_points = cartesian_suf_control_points;
    srf.weights = weights;

    return srf;
}

}