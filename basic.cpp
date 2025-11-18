#include <bits/stdc++.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
using namespace std;

// ==================== Penalties ====================
const int DELTA = 30;
int ALPHA[4][4] = {
    {0, 110, 48, 94},  // A
    {110, 0, 118, 48}, // C
    {48, 118, 0, 110}, // G
    {94, 48, 110, 0}   // T
};

int baseIndex(char c)
{
    switch (c)
    {
    case 'A':
        return 0;
    case 'C':
        return 1;
    case 'G':
        return 2;
    case 'T':
        return 3;
    default:
        return 0;
    }
}

// ==================== Helpers ====================
string trim(const string &s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos)
        return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool isIntegerLine(const string &s)
{
    if (s.empty())
        return false;
    for (char c : s)
    {
        if (!isdigit((unsigned char)c))
            return false;
    }
    return true;
}

// ==================== Generator ====================
string expandString(const string &base, const vector<int> &indices)
{
    string s = base;
    for (int n : indices)
    {
        string left = s.substr(0, n + 1);
        string right = (n + 1 < (int)s.size()) ? s.substr(n + 1) : "";
        s = left + s + right;
    }
    return s;
}

pair<string, string> parseInput(const string &path)
{
    ifstream in(path);
    if (!in)
    {
        cerr << "Error: cannot open input file\n";
        exit(1);
    }

    vector<string> lines;
    string line;
    while (getline(in, line))
    {
        string t = trim(line);
        if (!t.empty())
            lines.push_back(t);
    }

    if (lines.empty())
    {
        cerr << "Error: empty input file\n";
        exit(1);
    }

    size_t it = 0;
    string s0 = lines[it++];
    vector<int> s_idx;
    while (it < lines.size() && isIntegerLine(lines[it]))
    {
        s_idx.push_back(stoi(lines[it++]));
    }

    if (it >= lines.size())
    {
        cerr << "Error: missing second base string\n";
        exit(1);
    }

    string t0 = lines[it++];
    vector<int> t_idx;
    while (it < lines.size() && isIntegerLine(lines[it]))
    {
        t_idx.push_back(stoi(lines[it++]));
    }

    string X = expandString(s0, s_idx);
    string Y = expandString(t0, t_idx);
    return {X, Y};
}

// ==================== DP Alignment ====================
struct AlignmentResult
{
    int cost;
    string ax, ay;
};

AlignmentResult alignBasic(const string &X, const string &Y)
{
    int m = X.size(), n = Y.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    for (int i = 1; i <= m; i++)
        dp[i][0] = i * DELTA;
    for (int j = 1; j <= n; j++)
        dp[0][j] = j * DELTA;

    for (int i = 1; i <= m; i++)
    {
        int ix = baseIndex(X[i - 1]);
        for (int j = 1; j <= n; j++)
        {
            int iy = baseIndex(Y[j - 1]);

            int costDiag = dp[i - 1][j - 1] + ALPHA[ix][iy];
            int costUp = dp[i - 1][j] + DELTA;
            int costLeft = dp[i][j - 1] + DELTA;

            dp[i][j] = min({costDiag, costUp, costLeft});
        }
    }

    // traceback
    int i = m, j = n;
    string ax, ay;
    while (i > 0 || j > 0)
    {
        if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + ALPHA[baseIndex(X[i - 1])][baseIndex(Y[j - 1])])
        {
            ax.push_back(X[i - 1]);
            ay.push_back(Y[j - 1]);
            i--;
            j--;
        }
        else if (i > 0 && dp[i][j] == dp[i - 1][j] + DELTA)
        {
            ax.push_back(X[i - 1]);
            ay.push_back('_');
            i--;
        }
        else
        {
            ax.push_back('_');
            ay.push_back(Y[j - 1]);
            j--;
        }
    }
    reverse(ax.begin(), ax.end());
    reverse(ay.begin(), ay.end());

    return {dp[m][n], ax, ay};
}

// ==================== Memory + Time (spec version) ====================
long getTotalMemory()
{
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0)
        return usage.ru_maxrss;
    return -1; // shouldn't happen
}

// ==================== Main ====================
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: ./basic input.txt output.txt\n";
        return 1;
    }

    auto [X, Y] = parseInput(argv[1]);

    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    AlignmentResult result = alignBasic(X, Y);

    gettimeofday(&end, NULL);
    long mem = getTotalMemory();

    long seconds = end.tv_sec - begin.tv_sec;
    long micros = end.tv_usec - begin.tv_usec;
    double total_time_ms = seconds * 1000.0 + micros * 0.001;

    ofstream out(argv[2]);
    out << result.cost << "\n";
    out << result.ax << "\n";
    out << result.ay << "\n";
    out << total_time_ms << "\n";
    out << mem << "\n";
}
