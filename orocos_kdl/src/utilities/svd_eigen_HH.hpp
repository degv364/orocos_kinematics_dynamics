// Copyright  (C)  2007  Ruben Smits <ruben dot smits at mech dot kuleuven dot be>

// Version: 1.0
// Author: Ruben Smits <ruben dot smits at mech dot kuleuven dot be>
// Maintainer: Ruben Smits <ruben dot smits at mech dot kuleuven dot be>
// URL: http://www.orocos.org/kdl

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


//Based on the svd of the KDL-0.2 library by Erwin Aertbelien
#ifndef SVD_EIGEN_HH_HPP
#define SVD_EIGEN_HH_HPP


#include <Eigen/Core>
#include <algorithm>

using namespace Eigen;

namespace KDL
{
    inline double PYTHAG(double a,double b) {
        double at,bt,ct;
        at = fabs(a);
        bt = fabs(b);
        if (at > bt ) {
            ct=bt/at;
            return at*sqrt(1.0+ct*ct);
        } else {
            if (bt==0)
                return 0.0;
            else {
                ct=at/bt;
                return bt*sqrt(1.0+ct*ct);
            }
        }
    }


    inline double COPY_SIGN(double target,double source) {
        return ((source) >= 0.0 ? fabs(target) : -fabs(target));
    }

    /**
     * Singular value decomposition based on household rotations
     *
     * @param A matrix(mxn) From where singular values are computed
     * @param U matrix(mxn) Left singular vectors stored here
     * @param S vector(n)   Singular values stored here
     * @param V matrix(nxn) Right singular vectors stored here
     * @param tmp vector(n) Storing temporal data
     * @param maxiter defaults to 150
     * @param epsilon defaults to 1e-300
     *
     * @return 0 if no errors, -2 if maxiter exceeded, < -2 for numerical erors
     */
    int svd_eigen_HH(const MatrixXd& A,MatrixXd& U,VectorXd& S,MatrixXd& V,VectorXd& tmp,int maxiter=150,double epsilon=1e-300);
}
#endif
