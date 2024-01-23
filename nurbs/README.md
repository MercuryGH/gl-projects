## Nurbs System

View Nurbs curves and surfaces, and generate skinning meshes.

Construct lofting/skinning surfaces described in Chapter 10, Section 10.3 of *the NURBS Book*.

This project is based on the algorithms provided in *the NURBS book*.

## Extra Thirdparties

* [tinynurbs](https://github.com/pradeep-pyro/tinynurbs/)

## Data Structure and Algorithm

> For detail please see `/src/util.h`, codes are **with comments**.

### Data Structure

The data structure for NURBS curves and surfaces are from thirdparty library `tinynurbs`.

```cpp
struct NurbsCurve {
    unsigned int degree;
    std::vector<ScalarType> knots;
    std::vector<glm::vec<3, ScalarType>> control_points;
    std::vector<ScalarType> weights;
};

struct NurbsSurface {
    unsigned int degree_u, degree_v;
    std::vector<ScalarType> knots_u, knots_v;
    array2<glm::vec<3, ScalarType>> control_points;
    array2<ScalarType> weights;
};
```

Note that the stored control points are in cartesian space with weights.

### Evaluate NURBS curve and surface

To evaluate a NURBS curve

$$
\bm C^w(u) = \sum_{i=0}^n N_{i, p}(u) \bm P_i^w,
$$

at a given parameter $u$, we need to lift the control points $\bm P_i$ to homogenous space $\bm P_i^w$ instead of cartesian space, and evaluate the basis function $N_{i, p}(u)$. Design a lift function

```cpp
lift_to_homogenous(control_points, weights) -> homogenous_control_points
```

solves the problem. After the computation, we also need to lower the homogenous point $\bm C^w(u)$ to cartesian space $\bm C(u)$, as to show it in the scene. This transform of coordinate space will be held in most of the NURBS-related operation.

The evaluation of NURBS curve is based on Algorithm A3.1 in *the NURBS book*, i.e.,

1. find the knot span in which $u$ lies (Algorithm A2.1);
2. compute the nonzero basis functions (Algorithm A2.2);
3. multiply the values of the nonzero basis functions with the corresponding control points.

The evaluation of NURBS surface is similar, based on Algorithm A3.5.

### Evaluate derivative and surface normal

For derivative of a NURBS curve, compared with non-rational B-splines, an extra denominator increases the computation complexity. Generally, 2 steps are involved:

1. Compute the derivative of $\bm C^w(u)$ (Algorithm A3.2);
2. Compute the derivative of $\bm C(u)$ from the derivative of $\bm C^w(u)$ (Algorithm A4.2).

For surface case, the steps are similar. For surface normal, we cross 2 orthogonal first order derivative vectors to obtain a normal vector.

Although the renderer only shows the first order derivative, the implemented algorithm supports computing $k$ order derivatives for any given $k$.

### Skinning

Given $K$ NURBS curves $\{\bm C^w_0(u), \dots, \bm C^w_{K-1}(u) \}$, as $u$-direction cross sections, the construction of a NURBS surface by blending them is called skinning or lofting of the given NURBS curves. The blending direction is of course $v$-direction.

The skinning process can be listed below:

1. Bring all curves to a common $p$ (degree) and $U$ (knot vector) by degree elevation and knot alignment/refinement. Note that control points are also changed when doing knot refinement.
2. For the $v$-direction, choose a degree $q$, compute the parameters $\{\bar v_k \}$ and a knot vector $V$.
3. Do $n+1$ curve interpolations across the control points of the given curves, yields the control points $\bm Q_{i,j}^w$ of the skinning surface, where $n + 1$ is the number of control points in $\bm C_i^w(u)$.

The resulting surface will have $u$-direction degree $p$, knot vector $U$, $v$-direction degree $q$, knot vector $V$ and control points $\bm Q_{i,j}^w$.

#### Knot Refinement

If the given curves have different knot vector, one needs to align them into a common $U$. Counting the knot vector element to form a common $U$ is not difficult, and a knot refinement based on Algorithm A5.4 is implemented, takes the differences on knot vector as an input.

#### Construct v-dir parameters and knot vector

By the strategy of averging, the $v$-direction paramenters are computed as follow:

$$
\bar v_k =
\begin{cases}
0, & k = 0 \\
1, & k = K - 1, \\
\bar v_{k-1} + \frac{1}{n+1} \sum_{i=0}^n \frac{|\bm P_{i, k} - \bm P_{i,k-1}|}{d_i}, & \text{otherwise}
\end{cases}.
$$

where $d_i$ is the total chord length of $\bm P_{i, 0}, \dots, \bm P_{i, K-1}$. The knot vector is then computed using Equation 9.8 from the book.

#### Interpolation

The interpolation method of $n+1$ points in $v$-direction is based on Section 9.2 and Algorithm A9.1 in the book. The method uses chord length parameterization and knot vector obtained by averaging parameters from the above step.

The interpolation for a $v$-direction curve is a linear equation

$$
\bm P^w_k = \bm C^w(\bar v_k) = \sum_{i=0}^n N_{i, p}(\bar v_k) \bm Q^w_i,
$$

where control points $\bm Q^w_i$ are $n + 1$ unknowns. The coefficient matrix has $(\text{row}, \text{col})$ element as $N_{i, p}(\bar v_k)$. Note that for each component of $\bm Q_i^w$, the linear equation has exactly the same coefficient matrix, thus allowing us to perform LU decomposition and use it to solve the linear equation $4$ times, which is more efficient than Gaussian elimination.

The implementation of Algorithm A9.1 uses Eigen library to perform LU decomposition.
