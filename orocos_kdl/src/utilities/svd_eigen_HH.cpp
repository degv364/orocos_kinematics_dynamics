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

#include "svd_eigen_HH.hpp"

namespace KDL{
    
    int svd_eigen_HH(const MatrixXd& A,MatrixXd& U,VectorXd& S,MatrixXd& V,VectorXd& tmp,int maxiter,double epsilon)
    {
        //get the rows/columns of the matrix
        const int num_rows = A.rows();
        const int num_cols = A.cols();
    
        U.setZero();
        U.topLeftCorner(num_rows,num_cols)=A;

        int i, its, j, jj, k, nm, ppi;
        bool flag;
        double c(0),f(0),h(0),s(0),scale(0),x(0),y(0),z(0),g(0);

        /* Householder reduction to bidiagonal form. */
        for (i=0;i<num_cols;i++) {
            ppi=i+1;
            tmp(i)=scale*g;
            g=s=scale=0.0; 
            if (i<num_rows) {
                // compute the sum of the i-th column, starting from the i-th row
                for (k=i;k<num_rows;k++) scale += fabs(U(k,i));
                if (fabs(scale)>epsilon) {
                    // multiply the i-th column by 1.0/scale, start from the i-th element
                    // sum of squares of column i, start from the i-th element
                    for (k=i;k<num_rows;k++) {
                        U(k,i) /= scale;
                        s += U(k,i)*U(k,i);
                    }
                    f=U(i,i);  // f is the diag elem
		    if (s < 0) return -3;
                    g = -COPY_SIGN(sqrt(s),f);
                    h=f*g-s;
                    U(i,i)=f-g;
                    for (j=ppi;j<num_cols;j++) {
                        // dot product of columns i and j, starting from the i-th row
                        for (s=0.0,k=i;k<num_rows;k++) s += U(k,i)*U(k,j);
			if (h == 0) return -4;
                        f=s/h;
                        // copy the scaled i-th column into the j-th column
                        for (k=i;k<num_rows;k++) U(k,j) += f*U(k,i);
                    }
                    for (k=i;k<num_rows;k++) U(k,i) *= scale;
                }
            }
            // save singular value
            S(i)=scale*g;
            g=s=scale=0.0;
            if ((i <num_rows) && (i+1 != num_cols)) {
                // sum of row i, start from columns i+1
                for (k=ppi;k<num_cols;k++) scale += fabs(U(i,k));
                if (fabs(scale)>epsilon) {
                    for (k=ppi;k<num_cols;k++) {
                        U(i,k) /= scale;
                        s += U(i,k)*U(i,k);
                    }
                    f=U(i,ppi);
		    if (s < 0) return -5;
                    g = -COPY_SIGN(sqrt(s),f);
                    h=f*g-s;
                    U(i,ppi)=f-g;
		    if (h == 0) return -6;
                    for (k=ppi;k<num_cols;k++) tmp(k)=U(i,k)/h;
                    for (j=ppi;j<num_rows;j++) {
                        for (s=0.0,k=ppi;k<num_cols;k++) s += U(j,k)*U(i,k);
                        for (k=ppi;k<num_cols;k++) U(j,k) += s*tmp(k);
                    }
                    for (k=ppi;k<num_cols;k++) U(i,k) *= scale;
                }
            }
        }
        /* Accumulation of right-hand transformations. */
        for (i=num_cols-1;i>=0;i--) {
            if (i<num_cols-1) {
                if (fabs(g)>epsilon) {
		    if (U(i,ppi) == 0) return -7;
		    for (j=ppi;j<num_cols;j++) V(j,i)=(U(i,j)/U(i,ppi))/g;
		    for (j=ppi;j<num_cols;j++) {
		        for (s=0.0,k=ppi;k<num_cols;k++) s += U(i,k)*V(k,j);
		        for (k=ppi;k<num_cols;k++) V(k,j) += s*V(k,i);
		    }
                }
                for (j=ppi;j<num_cols;j++) V(i,j)=V(j,i)=0.0;
            }
            V(i,i)=1.0;
            g=tmp(i);
            ppi=i;
        }
        /* Accumulation of left-hand transformations. */
        for (i=num_cols-1<num_rows-1 ? num_cols-1:num_rows-1;i>=0;i--) {
            ppi=i+1;
            g=S(i);
            for (j=ppi;j<num_cols;j++) U(i,j)=0.0;
            if (fabs(g)>epsilon) {
                g=1.0/g;
                for (j=ppi;j<num_cols;j++) {
                    for (s=0.0,k=ppi;k<num_rows;k++) s += U(k,i)*U(k,j);
		    if (U(i,i) == 0) return -8;
                    f=(s/U(i,i))*g;
                    for (k=i;k<num_rows;k++) U(k,j) += f*U(k,i);
                }
                for (j=i;j<num_rows;j++) U(j,i) *= g;
            } else {
                for (j=i;j<num_rows;j++) U(j,i)=0.0;
            }
            ++U(i,i);
        }
        
        /* Diagonalization of the bidiagonal form. */
        for (k=num_cols-1;k>=0;k--) { /* Loop over singular values. */
            for (its=1;its<=maxiter;its++) {  /* Loop over allowed iterations. */
                flag=true;
                for (ppi=k;ppi>=0;ppi--) {  /* Test for splitting. */
                    nm=ppi-1;             /* Note that tmp[1] is always zero. */
                    if (tmp(ppi)==0) {
                        flag=false;
                        break;
                    }
                    if (S(nm)==0) break;
                }
                if (flag) {
                    c=0.0;           /* Cancellation of tmp[l], if l>1: */
                    s=1.0;
                    for (i=ppi;i<=k;i++) {
                        f=s*tmp(i);
                        tmp(i)=c*tmp(i);
                        if (f==0) break;
                        g=S(i);
                        h=PYTHAG(f,g);
                        S(i)=h;
			if (h == 0) return -9;
                        h=1.0/h;
                        c=g*h;
                        s=(-f*h);
                        for (j=0;j<num_rows;j++) {
                            y=U(j,nm);
                            z=U(j,i);
                            U(j,nm)=y*c+z*s;
                            U(j,i)=z*c-y*s;
                        }
                    }
                }
                z=S(k);
                
                if (ppi == k) {      /* Convergence. */
                    if (z < 0.0) {   /* Singular value is made nonnegative. */
                        S(k) = -z;
                        for (j=0;j<num_cols;j++) V(j,k)=-V(j,k);
                    }
                    break;
                }
                
                x=S(ppi);            /* Shift from bottom 2-by-2 minor: */
                nm=k-1;
                y=S(nm);
                g=tmp(nm);
                h=tmp(k);
		if (h == 0 || y == 0) return -10;
                f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
                
                g=PYTHAG(f,1.0);
		if (x == 0) return -11;
		if (f + COPY_SIGN(g,f) == 0) return -12;
                f=((x-z)*(x+z)+h*((y/(f+COPY_SIGN(g,f)))-h))/x;
                
                /* Next QR transformation: */
                c=s=1.0;
                for (j=ppi;j<=nm;j++) {
                    i=j+1;
                    g=tmp(i);
                    y=S(i);
                    h=s*g;
                    g=c*g;
                    z=PYTHAG(f,h);
                    tmp(j)=z;
		    if (z == 0) return -13;
                    c=f/z;
                    s=h/z;
                    f=x*c+g*s;
                    g=g*c-x*s;
                    h=y*s;
                    y=y*c;
                    for (jj=0;jj<num_cols;jj++) {
                        x=V(jj,j);
                        z=V(jj,i);
                        V(jj,j)=x*c+z*s;
                        V(jj,i)=z*c-x*s;
                    }
                    z=PYTHAG(f,h);
                    S(j)=z;
                    if (fabs(z)>epsilon) {
                        z=1.0/z;
                        c=f*z;
                        s=h*z;
                    }
                    f=(c*g)+(s*y);
                    x=(c*y)-(s*g);
                    for (jj=0;jj<num_rows;jj++) {
                        y=U(jj,j);
                        z=U(jj,i);
                        U(jj,j)=y*c+z*s;
                        U(jj,i)=z*c-y*s;
                    }
                }
                tmp(ppi)=0.0;
                tmp(k)=f;
                S(k)=x;
            }
        }

        //Sort eigen values:
        for (i=0; i<num_cols; i++){
            
            double S_max = S(i);
            int i_max = i;
            for (j=i+1; j<num_cols; j++){
                double Sj = S(j);
                if (Sj > S_max){
                    S_max = Sj;
                    i_max = j;
                }
            }
            if (i_max != i){
                /* swap eigenvalues */
                double tmp = S(i);
                S(i)=S(i_max);
                S(i_max)=tmp;
                
                /* swap eigenvectors */
                U.col(i).swap(U.col(i_max));
                V.col(i).swap(V.col(i_max));
            }
        }
        
        if (its == maxiter) 
            return (-2);
        else 
	    return (0);
    }
}
