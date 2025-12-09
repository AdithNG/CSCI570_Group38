#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/resource.h>
#include <sys/time.h>
using namespace std;

int alpha_mat[4][4] = {
    {0,110,48,94},   // A
    {110,0,118,48},  // C
    {48,118,0,110},  // G
    {94,48,110,0}    // T
};
int delta_penalty = 30;

int charToIndex(char c) {
    if(c=='A') return 0;
    if(c=='C') return 1;
    if(c=='G') return 2;
    if(c=='T') return 3;
    return -1;
}
int mismatchCost(char a, char b) {
    return alpha_mat[charToIndex(a)][charToIndex(b)];
}

string trim(const string &s){
    string r = s;
    while (!r.empty() && isspace((unsigned char)r.back())) {
        r.pop_back();
    } 
    while (!r.empty() && isspace((unsigned char)r.front())) {
        r.erase(r.begin());
    } 
    return r;
}
string generateString(const vector<string>& lines, int &idx) {
    string base = trim(lines[idx++]);
    while (idx < (int)lines.size()) {
        string s = trim(lines[idx]);
        if (s.empty()) { 
            idx++; 
            continue; 
        }
        bool isNum = true;
        for (char c: s) {
            if (!isdigit((unsigned char)c)) { 
                isNum=false; 
                break; 
            }
        } 
        if (!isNum) break;
        int pos = stoi(s);
        int p = pos + 1;
        if (p < 0) p = 0;
        if (p > (int)base.size()) p = base.size();
        base = base.substr(0,p) + base + base.substr(p);
        idx++;
    }
    return base;
}


long getTotalMemoryKB() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) return usage.ru_maxrss;
    return -1;
}

// compute last row costs when aligning X with prefixes of Y
vector<int> computeLastRow(const string &X, const string &Y) {
    int m = X.size(), n = Y.size();
    vector<int> prev(n+1), curr(n+1);
    for (int j=0;j<=n;j++) {
        prev[j] = j * delta_penalty;
    } 
    for (int i=1;i<=m;i++) {
        curr[0] = i * delta_penalty;
        for (int j=1;j<=n;j++) {
            int match = prev[j-1] + mismatchCost(X[i-1], Y[j-1]);
            int delX  = prev[j] + delta_penalty;
            int delY  = curr[j-1] + delta_penalty;
            curr[j] = min(match, min(delX, delY));
        }
        prev.swap(curr);
    }
    return prev;
}

// basic DP for small sizes
pair<string,string> baseAlign(const string &X, const string &Y) {
    int m = X.size(), n = Y.size();
    vector<vector<int>> dp(m+1, vector<int>(n+1,0));
    for (int i=1;i<=m;i++) dp[i][0]=i*delta_penalty;
    for (int j=1;j<=n;j++) dp[0][j]=j*delta_penalty;
    for (int i=1;i<=m;i++) {
        for (int j=1;j<=n;j++) {
            int match = dp[i-1][j-1] + mismatchCost(X[i-1], Y[j-1]);
            int delX = dp[i-1][j] + delta_penalty;
            int delY = dp[i][j-1] + delta_penalty;
            dp[i][j] = min(match, min(delX, delY));
        }
    }
    int i=m,j=n;
    string ax="", ay="";
    while (i>0 || j>0) {
        if (i>0 && j>0 && dp[i][j] == dp[i-1][j-1] + mismatchCost(X[i-1],Y[j-1])) {
            ax.push_back(X[i-1]); ay.push_back(Y[j-1]); i--; j--;
        } else if (i>0 && dp[i][j] == dp[i-1][j] + delta_penalty) {
            ax.push_back(X[i-1]); ay.push_back('_'); i--;
        } else {
            ax.push_back('_'); ay.push_back(Y[j-1]); j--;
        }
    }
    reverse(ax.begin(), ax.end());
    reverse(ay.begin(), ay.end());
    return {ax, ay};
}

pair<string,string> spaceEfficientAlign(const string &X, const string &Y) {
    int m = X.size(), n = Y.size();
    if (m==0) return {string(n,'_'), Y};
    if (n==0) return {X, string(m,'_')};
    if (m==1 || n==1) {
        return baseAlign(X,Y);
    }
    int mid = m/2;
    string Xl = X.substr(0,mid);
    string Xr = X.substr(mid);
    vector<int> leftCost = computeLastRow(Xl, Y);
    string Xr_rev = Xr; reverse(Xr_rev.begin(), Xr_rev.end());
    string Y_rev = Y; reverse(Y_rev.begin(), Y_rev.end());
    vector<int> rightCostRev = computeLastRow(Xr_rev, Y_rev);
    vector<int> rightCost(n+1);
    for (int j=0;j<=n;j++) {
        rightCost[j] = rightCostRev[n-j];
    } 

    int split = 0;
    int best = INT_MAX;
    for (int j=0;j<=n;j++) {
        int c = leftCost[j] + rightCost[j];
        if (c < best) { 
            best = c; 
            split = j; 
        }
    }
    string Yl = Y.substr(0, split);
    string Yr = Y.substr(split);
    auto leftPair = spaceEfficientAlign(Xl, Yl);
    auto rightPair = spaceEfficientAlign(Xr, Yr);
    return {leftPair.first + rightPair.first, leftPair.second + rightPair.second};
}

int main(int argc, char** argv){
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    ifstream infile(argv[1]);
    if (!infile) { 
        cerr << "Error opening input file\n"; 
        return 1; 
    }
    vector<string> lines;
    string line;
    while (getline(infile,line)) {
        if (!line.empty()) lines.push_back(line);
    }
    infile.close();

    int idx=0;
    string X = generateString(lines, idx);
    string Y = generateString(lines, idx);

    struct timeval tstart, tend;
    gettimeofday(&tstart, nullptr);
    long mem_before = getTotalMemoryKB();

    auto aligned = spaceEfficientAlign(X,Y);
    string alignX = aligned.first;
    string alignY = aligned.second;

    int cost=0;
    for(size_t k=0;k<alignX.size();k++){
        if (alignX[k]=='_' || alignY[k]=='_') cost += delta_penalty;
        else cost += mismatchCost(alignX[k], alignY[k]);
    }

    long mem_after = getTotalMemoryKB();
    gettimeofday(&tend, nullptr);
    double elapsed_ms = (tend.tv_sec - tstart.tv_sec)*1000.0 + (tend.tv_usec - tstart.tv_usec)/1000.0;
    double mem_used_kb = (mem_after - mem_before);
    if (mem_used_kb < 0) mem_used_kb = mem_after;

    ofstream outfile(argv[2]);
    if (!outfile) { 
        cerr << "Error creating output file\n"; 
        return 1; 
    }
    outfile << cost << "\n";
    outfile << alignX << "\n";
    outfile << alignY << "\n";
    outfile << fixed << setprecision(6) << elapsed_ms << "\n";
    outfile << fixed << setprecision(0) << mem_used_kb << "\n";
    outfile.close();

    return 0;
}