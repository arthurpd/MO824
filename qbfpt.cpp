#include <bits/stdc++.h>
using namespace std;

#define MAXN 412
#define inf 0x3f3f3f3f

typedef pair<long long, int> pli;

int n;
int a[MAXN][MAXN];
int triples[MAXN][3]; // We always have n triples in this particular case.

vector<int> best_sol;
int best_sol_value;

vector<int> sol;
int sol_value;
vector<int> delta;

// pt_tab[i] is the number of prohibted triples blocking the entrance of i.
vector<int> pt_tab;

// tabu_tab[i] is the number of occurences of element i in the tabu_list (could be more than 1 due to aspiration criteria).
vector<int> tabu_tab;
list<pli> tabu_list;

long long iteration;

int l(int u, int pi_1, int pi_2)
{
    return 1 + ((pi_1 * (u - 1) + pi_2) % n);
}

void read_input(string name)
{
    freopen(name.c_str(), "r", stdin);
    cin >> n;
    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++)
            cin >> a[i][j]; // a[j][i] = 0

    for (int i = 0; i < n; i++)
    {
        int u = i + 1;
        int g = (l(u, 131, 1031) != u) ? l(u, 131, 1031) : (1 + (l(u, 131, 1031) % n));
        int h = (l(u, 193, 1093) != u && l(u, 193, 1093) != g) ? l(u, 193, 1093) : (1 + (l(u, 193, 1093) % n) != u && 1 + (l(u, 193, 1093) % n) != g ? 1 + (l(u, 193, 1093) % n) : 1 + ((l(u, 193, 1093) + 1) % n));
        triples[i][0] = u - 1; // Need to convert back to 0-based index.
        triples[i][1] = g - 1;
        triples[i][2] = h - 1;
        sort(triples[i], triples[i] + 3);
    }

    cout << n << endl;
    for (int i = 0; i < n; i++, printf("\n"))
        for (int j = 0; j < n; j++)
            cout << " " << a[i][j];
    
    for (int i = 0; i < n; i++)
        printf(" %d %d %d\n", triples[i][0], triples[i][1], triples[i][2]);
}

void remove_old_tabu(int tabu_tenure)
{
    while (!tabu_list.empty() && iteration - tabu_list.front().first > tabu_tenure)
    {
        tabu_tab[tabu_list.front().second]--;
        tabu_list.pop_front();
    }
}

void insert_tabu(int x)
{
    tabu_tab[x]++;
    tabu_list.push_back(pli(iteration, x));
}

void update_deltas(int x)
{
    delta[x] *= -1;
    for (int i = 0; i < n; i++)
    {
        if (i != x)
        {
            // if entering (sol[x] == 0), increases delta of those out and decreases of those in.
            delta[i] += (sol[x] ? -1 : 1) * (sol[i] ? -1 : 1) * (a[i][x] + a[x][i]);
        }
    }
}

void update_pt(int x)
{

}

void neightborhood_move()
{
    int max_delta = -inf;
    int best_cand = -1;

    for (int i = 0; i < n; i++)
    {
        // We can remove anyone, but only insert those not blocked by triples.
        if (sol[i] == 1 || (sol[i] == 0 && pt_tab[i] == 0))
        {
            // Non-tabu or aspiration criteria.
            if (tabu_tab[i] == 0 || sol_value + delta[i] > best_sol_value)
            {
                if (delta[i] > max_delta)
                {
                    max_delta = delta[i];
                    best_cand = i;
                }
            }
        }
    }

    if (best_cand != -1)
    {
        insert_tabu(best_cand);
        update_deltas(best_cand);
        update_pt(best_cand);

        sol[best_cand] = (sol[best_cand] ? 0 : 1);
        sol_value += max_delta;
    }
}

void tabu_search(long long num_iters, int tabu_tenure)
{
    sol = vector<int>(n, 0);
    sol_value = 0;
    pt_tab = vector<int> (n, 0);
    delta = vector<int>(n, 0);
    for (int i = 0; i < n; i++)
        delta[i] = a[i][i];

    best_sol = vector<int>(n, 0);
    best_sol_value = 0;

    tabu_tab = vector<int>(n, 0);
    tabu_list = list<pli>();

    for (iteration = 0; iteration < num_iters; iteration++)
    {
        remove_old_tabu(tabu_tenure);
        neightborhood_move();
        if (sol_value > best_sol_value)
        {
            best_sol = sol;
            best_sol_value = sol_value;

            printf(" %lld, %d\n", iteration, best_sol_value);
        }
    }
}

int main(void)
{
    read_input("pacote_atividade4/TS_Framework/instances/qbf020");
    tabu_search(10000, 20);
}
