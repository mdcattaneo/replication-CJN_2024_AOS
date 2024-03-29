#include <Rcpp.h>
using namespace Rcpp;

NumericVector objfnc(NumericVector A, NumericVector Psi, NumericVector Aboot,
                NumericVector Psiboot, NumericVector M, double x0,
                double thetahat, NumericVector evals){
  int n = A.size();
  int lenout = evals.size();
  NumericVector out(lenout);
  
  for (int k=0; k<lenout; k++){
    NumericVector tmp = A[A>evals[k]];
    double obj = 0.0;
    for (int i=0; i<n; i ++){
      obj += (Aboot[i] <= evals[k])*Psiboot[i] - (A[i] <= evals[k])*Psi[i] + thetahat*(A[i] <= evals[k]);
    }
    out[k] = obj/n + M[0]*pow(evals[k]-x0,2) + M[1]*pow(evals[k]-x0,4);
  }
  return(out);
}


NumericVector ECDF(NumericVector X, NumericVector evals){
  int N = X.size();
  int lenE = evals.size();
  NumericVector out(lenE);
  for (int k=0; k<lenE;k++){
    double tmp = 0.0;
    for (int i=0; i<N;i++){
      tmp += (X[i] <= evals[k]);
    }
    out[k] = tmp/N;
  }
  return(out);
}

//[[Rcpp::export]]
double upsilon_hat(NumericVector A, NumericVector Psi,double x0,
               double thetahat){
  int n = A.size();
  double obj = 0.0;
  for (int j=0;j<n;j++){
    obj += Psi[j]*(A[j]<=x0) - thetahat*(A[j]<=x0);
  }
  double out = obj/n;
  return(out);
}


//[[Rcpp::export]]
double Boot(NumericVector A, NumericVector Psi, NumericVector Aboot,
                  NumericVector Psiboot,NumericVector M, double x0,double thetahat){
  NumericVector Grid1 = A[A<=x0];
  NumericVector Grid2 = A[A>=x0];
  int N1 = Grid1.size();
  NumericVector mins(N1);
  NumericVector f_g1 = objfnc(A, Psi, Aboot, Psiboot,M,x0,thetahat,Grid1);
  NumericVector f_g2 = objfnc(A, Psi, Aboot, Psiboot,M,x0,thetahat,Grid2);
  NumericVector Phi1 = ECDF(Aboot,Grid1);
  NumericVector Phi2 = ECDF(Aboot,Grid2);
  for (int i=0; i< N1; i++){
    NumericVector denom = Phi2-Phi1[i];
    denom[denom==0] = 1.0;
    NumericVector tmp = (f_g2 - f_g1[i])/denom;
    mins[i] = min(tmp);
  }
  return max(mins);
}


NumericVector PIGamma(NumericVector G, NumericVector M,
                      double thetahat, NumericVector grid){
  int J = grid.size();
  NumericVector out(J);
  
  for (int k=0; k<J; k++){
    out[k] = G[k] + M[0]*pow(grid[k],2) + M[1]*pow(grid[k],4);
  }
  return(out);
}

//[[Rcpp::export]]
double PlugIn(NumericVector grid, NumericVector G,
                  NumericVector M, double thetahat){
  NumericVector Grid1 = grid[grid<0];
  NumericVector G1 = G[grid<0];
  NumericVector Grid2 = grid[grid>=0];
  NumericVector G2 = G[grid>=0];
  int N1 = Grid1.size();
  NumericVector mins(N1);
  NumericVector f_g1 = PIGamma(G1,M,thetahat,Grid1);
  NumericVector f_g2 = PIGamma(G2,M,thetahat,Grid2);
  for (int i=0; i< N1; i++){
    NumericVector denom = Grid2-Grid1[i];
    NumericVector tmp = (f_g2 - f_g1[i])/denom;
    mins[i] = min(tmp);
  }
  return max(mins);
}