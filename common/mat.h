#ifndef MAT_H
#define MAT_H

#include <cstring>
#include <vec.h>
 
template <size_t DimRows, size_t DimCols, class T> std::ostream& operator<<(std::ostream& out, mat<DimRows, DimCols, T>& m) {
    for (size_t i = 0; i < DimRows; i++) out << m[i] << std::endl;
    return out;
}

// determine rotation Q and symmetrix matrix A so that Q*A=F
template<class T>
void signed_polar_decomposition(const mat<2,2,T>& F, mat<2,2,T>& Q, mat<2,2,T>& A)
{
    T s=F(0,1)-F(1,0);
    if(s){
        T c=F(0,0)+F(1,1), hyp=std::sqrt(c*c+s*s);
        if(c>0){ c/=hyp; s/=hyp; }
        else{ c/=-hyp; s/=-hyp; }
        Q(0,0)=c; Q(0,1)=s; Q(1,0)=-s; Q(1,1)=c;
        A(0,0)=c*F(0,0)-s*F(1,0);
        A(1,0)=A(0,1)=s*F(0,0)+c*F(1,0);
        A(1,1)=s*F(0,1)+c*F(1,1);
    }else{ // F is already symmetric: no rotation needed
        Q(0,0)=1; Q(0,1)=0; Q(1,0)=0; Q(1,1)=1;
        A=F;
    }
}

// for symmetric A, determine c and s for rotation Q and diagonal matrix D so that Q'*A*Q=D
// (D sorted, with |D[0]|>=|D[1]|; the upper triangular entry of A is ignored)
template<class T>
void symmetric_eigenproblem(const mat<2,2,T>& A, T& c, T& s, vec<2,T>& D)
{
    s=2*A(1,0);
    if(s==0){ // already diagonal
        c=1;
        D[0]=A(0,0); D[1]=A(1,1);
    }else{
        T d=A(0,0)-A(1,1);
        T disc=std::sqrt(d*d+s*s);
        c=d+(d>0 ? disc : -disc);
        T hyp=std::sqrt(c*c+s*s);
        c/=hyp; s/=hyp;
        // there is probably a better way of computing these (more stable etc.)
        D[0]=c*c*A(0,0)+2*c*s*A(1,0)+s*s*A(1,1);
        D[1]=s*s*A(0,0)-2*c*s*A(1,0)+c*c*A(1,1);
    }
    if(std::fabs(D[0])<std::fabs(D[1])){ // if D is in the wrong order, fix it
        std::swap(D[0], D[1]);
        c=-c;
        std::swap(c, s);
    }
    if(c<0){ c=-c; s=-s; }
}



// for symmetric A, determine rotation Q and diagonal matrix D so that Q'*A*Q=D
// (D not sorted! and the upper triangular entry is ignored)
template<class T>
void symmetric_eigenproblem(const mat<2,2,T>& A, mat<2,2,T>& Q, vec<2,T>& D)
{
    T c, s;
    symmetric_eigenproblem(A, c, s, D);
    Q(0,0)=c; Q(1,0)=s; Q(0,1)=-s; Q(1,1)=c;
}

// figures out A=U*S*V' with U and V rotations, S diagonal
// (S is sorted by magnitude, and may contain both positive and negative entries)
template<class T>
void signed_svd(const mat<2,2,T>& A, mat<2,2,T>& U, mat<2,2,T>& V, vec<2,T>& S)
{
    vec<2,T> D;
    symmetric_eigenproblem(mat<2,2,T>(sqr(A(0,0))+sqr(A(0,1)),
                                      A(0,0)*A(1,0)+A(0,1)*A(1,1),
                                      0, // ignored by routine, so we don't need to fill it in
                                      sqr(A(1,0))+sqr(A(1,1))),
                           U, D);
    // form F=A'*U 
    T f00=A(0,0)*U(0,0)+A(1,0)*U(1,0),
    f10=A(0,1)*U(0,0)+A(1,1)*U(1,0),
    f01=A(0,0)*U(0,1)+A(1,0)*U(1,1),
    f11=A(0,1)*U(0,1)+A(1,1)*U(1,1);
    // do signed polar decomposition of F to get V
    T s=f01-f10, c;
    if(s){
        c=f00+f11;
        T hyp=std::sqrt(c*c+s*s);
        if(c>0){ c/=hyp; s/=hyp; }
        else{ c/=-hyp; s/=-hyp; }
    }else
        c=1;
    V(0,0)=c; V(0,1)=s; V(1,0)=-s; V(1,1)=c;
    // and finally grab the singular values from direct computation (maybe there's a better way?)
    S[0]=(U(0,0)*A(0,0)+U(1,0)*A(1,0))*V(0,0) + (U(0,0)*A(0,1)+U(1,0)*A(1,1))*V(1,0);
    S[1]=(U(0,1)*A(0,0)+U(1,1)*A(1,0))*V(0,1) + (U(0,1)*A(0,1)+U(1,1)*A(1,1))*V(1,1);
}

// 3x3 version: Get the A=QR decomposition using Householder reflections
template<class T>
void find_QR(const mat<3,3,T>& A, mat<3,3,T>& Q, mat<3,3,T>& R)
{
    Q(0,0)=1; Q(0,1)=0; Q(0,2)=0;
    Q(1,0)=0; Q(1,1)=1; Q(1,2)=0;
    Q(2,0)=0; Q(2,1)=0; Q(2,2)=1;
    R=A;
    vec<3,T> u;
    T n2, u2, n, c, d;
    
    // first column
    n2=sqr(R(0,0))+sqr(R(1,0))+sqr(R(2,0));
    u[0]=R(0,0); u[1]=R(1,0); u[2]=R(2,0); // will change u[0] in a moment
    u2=sqr(u[1])+sqr(u[2]); // will add in sqr(u[0]) when we know it
    R(1,0)=0; R(2,0)=0;
    if(u2){ // if there are entries to annihilate below the diagonal in this column
        n=std::sqrt(n2);
        u[0]+=(u[0]>0 ? n : -n);
        u2+=sqr(u[0]);
        c=2/u2;
        // update diagonal entrt of R
        R(0,0)=(u[0]>0 ? -n : n);
        // update rest of R with reflection
        // second column
        d=c*(u[0]*R(0,1)+u[1]*R(1,1)+u[2]*R(2,1));
        R(0,1)-=d*u[0]; R(1,1)-=d*u[1]; R(2,1)-=d*u[2];
        // third column
        d=c*(u[0]*R(0,2)+u[1]*R(1,2)+u[2]*R(2,2));
        R(0,2)-=d*u[0]; R(1,2)-=d*u[1]; R(2,2)-=d*u[2];
        // set Q to this symmetric reflection
        Q(0,0)-=c*u[0]*u[0]; Q(0,1)-=c*u[0]*u[1]; Q(0,2)-=c*u[0]*u[2];
        Q(1,0)=Q(0,1);       Q(1,1)-=c*u[1]*u[1]; Q(1,2)-=c*u[1]*u[2];
        Q(2,0)=Q(0,2);       Q(2,1)=Q(1,2);       Q(2,2)-=c*u[2]*u[2];
    }else{
        // still do a reflection around (1,0,0), since we want there to be exactly two reflections (to get back to rotation)
        R(0,0)=-R(0,0); R(0,1)=-R(0,1); R(0,2)=-R(0,2);
        Q(0,0)=-1;
    }
    
    // second column
    n2=sqr(R(1,1))+sqr(R(2,1));
    u[1]=R(1,1); u[2]=R(2,1); // will change u[1] in a moment
    u2=sqr(u[2]); // will add in sqr(u[1]) when we know it
    R(2,1)=0;
    if(u2){ // if there are entries to annihilate below the diagonal in this column
        n=std::sqrt(n2);
        u[1]+=(u[1]>0 ? n : -n);
        u2+=sqr(u[1]);
        c=2/u2;
        // update diagonal entrt of R
        R(1,1)=(u[1]>0 ? -n : n);
        // update rest of R with reflection
        // third column
        d=c*(u[1]*R(1,2)+u[2]*R(2,2));
        R(1,2)-=d*u[1]; R(2,2)-=d*u[2];
        // update Q by right multiplication with the reflection
        d=c*(Q(0,1)*u[1]+Q(0,2)*u[2]); Q(0,1)-=d*u[1]; Q(0,2)-=d*u[2]; // row 0
        d=c*(Q(1,1)*u[1]+Q(1,2)*u[2]); Q(1,1)-=d*u[1]; Q(1,2)-=d*u[2]; // row 1
        d=c*(Q(2,1)*u[1]+Q(2,2)*u[2]); Q(2,1)-=d*u[1]; Q(2,2)-=d*u[2]; // row 2
    }else{
        // still need to multiply in a reflection around (0,1,0), to get Q back to being a rotation
        R(1,1)=-R(1,1); R(1,2)=-R(1,2);
        Q(0,1)=-Q(0,1); Q(1,1)=-Q(1,1); Q(2,1)=-Q(2,1);
    }
    
    // flip back some signs to be closer to A
    R(0,0)=-R(0,0); R(0,1)=-R(0,1); R(0,2)=-R(0,2);
    R(1,1)=-R(1,1); R(1,2)=-R(1,2);
    Q(0,0)=-Q(0,0); Q(0,1)=-Q(0,1);
    Q(1,0)=-Q(1,0); Q(1,1)=-Q(1,1);
    Q(2,0)=-Q(2,0); Q(2,1)=-Q(2,1);
}

// Specialization for 2xN case: use a Givens rotation
template<unsigned int N, class T>
void find_QR(const mat<2,N,T>& A, mat<2,N,T>& Q, mat<2,N,T>& R)
{
    T c=A(0,0), s=A(1,0), hyp=std::sqrt(c*c+s*s);
    if(hyp){
        c/=hyp;
        s/=hyp;
        Q(0,0)=c; Q(1,0)=s; Q(0,1)=-s; Q(1,1)=c;
        R(0,0)=hyp; R(1,0)=0;
        for(unsigned int j=1; j<N; ++j){
            R(0,j)=c*A(0,j)+s*A(1,j);
            R(1,j)=-s*A(0,j)+c*A(1,j);
        }
    }else{
        Q(0,0)=1; Q(1,0)=0; Q(0,1)=0; Q(1,1)=1;
        R=A;
    }
}



#endif
