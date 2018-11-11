#include <Utils.hh>
#include <iostream>

using namespace std;
using namespace lcio;
using namespace marlin;

char* TLorentzVector2char(TVector3 &t, double t4)
{ 
    char* str = new char[50];
    sprintf(str, "(%f, %f, %f, %f)",t.x(),t.y(),t.z(),t4);
    return str;
}

char* TLorentzVector2char(TLorentzVector &t)
{ 
    char* str = new char[50];
    sprintf(str, "(%f, %f, %f, %f)",t.X(),t.Y(),t.Z(),t.T());
    return str;
}

string PdgId2string(int pdgId)
{
    string bar="";
    string type="?";
    if (pdgId < 0)  bar="(-)";
    switch ( abs(pdgId) ) {
        case 1: type="d"; break;
        case 2: type="u"; break;
        case 3: type="s"; break;
        case 4: type="c"; break;
        case 5: type="b"; break;
        case 6: type="t"; break;
        case 7: type="b'"; break;
        case 8: type="t'"; break;
        case 11: type="e-"; break;
        case 12: type="ve"; break;
        case 13: type="mu-"; break;
        case 14: type="vm"; break;
        case 15: type="ta-"; break;
        case 16: type="vt"; break;
        case 17: type="ta-'"; break;
        case 18: type="vt'"; break;
        case 21: type="g"; break;
        case 22: type="gm"; break;
        case 23: type="Z0"; break;
        case 24: type="W+"; break;
        case 25: type="h0"; break;
        case 211: type="pi+"; break;
        case 311: type="K0"; break;
        case 321: type="K+"; break;
        case 411: type="D+"; break;
        case 421: type="D0"; break;
        case 511: type="B0"; break;
        case 521: type="B+"; break;
        case 111: type="pi0"; break;
        case 221: type="eta"; break;
        case 213: type="rho+"; break;
        case 113: type="rho0"; break;
        case 2112: type="n0"; break;
        case 2212: type="p+"; break;
        default:  break;
    }
    return bar + type;
}