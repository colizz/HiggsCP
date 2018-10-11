#include <Utils.hh>
#include <iostream>

using namespace std;

void PrintTLorentzVector(TVector3 &t, double t4)
{ 
    printf("(%f, %f, %f, %f)",t.x(),t.y(),t.z(),t4);
}

void PrintTLorentzVector(TLorentzVector &t)
{ 
    printf("(%f, %f, %f, %f)",t.X(),t.Y(),t.Z(),t.T());
}

