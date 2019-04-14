#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

#define MAXN 412
#define inf 0x3f3f3f3f

typedef pair<long long, int> pli;

int n;
int a[MAXN][MAXN];
int triples[MAXN][3]; // We always have n triples in this particular case.
vector<int *> idx_triples[MAXN];

vector<int> best_sol;
int best_sol_value;

double infeasible_penalty;
double removal_factor;

vector<int> sol;
int sol_value;

// If 0, solution is feasible.
int sol_violated_triples;
vector<int> delta;

// pt_tab[i] is the number of prohibted triples violated if i enters/leaves the solution. (can be negative if triple will stop being violated)
vector<int> pt_tab;

// tabu_tab[i] is the number of occurences of element i in the tabu_list (could be more than 1 due to aspiration criteria).
vector<int> tabu_tab;
list<pli> tabu_list;

vector<int> pi;

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

    for (int i = 0; i < n; i++)
        idx_triples[i].clear();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
            idx_triples[triples[i][j]].push_back(triples[i]);
    }

    // cout << n << endl;
    // for (int i = 0; i < n; i++, printf("\n"))
    //     for (int j = 0; j < n; j++)
    //         cout << " " << a[i][j];

    // for (int i = 0; i < n; i++)
    //     printf(" %d %d %d\n", triples[i][0], triples[i][1], triples[i][2]);
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
    // Amortized constant time, since there are O(n) triples randomly generated.
    for (auto triple : idx_triples[x])
    {
        int u, v;
        if (triple[0] == x)
            u = triple[1], v = triple[2];
        else if (triple[1] == x)
            u = triple[0], v = triple[2];
        else if (triple[2] == x)
            u = triple[0], v = triple[1];
        else
            assert(false);

        if (sol[u] == 1 && sol[v] == 1)
        {
            if (sol[x])
            {
                // Used to be -1, but now is 1.
                pt_tab[x] += 2;
                pt_tab[u] += 1;
                pt_tab[v] += 1;
            }
            else
            {
                // Used to be 1, but now is -1.
                pt_tab[x] -= 2;

                // Used to be 0, but now is -1.
                pt_tab[u] -= 1;
                pt_tab[v] -= 1;
            }
        }
        else if (sol[u] == 1)
            pt_tab[v] += (sol[x] ? -1 : 1);
        else if (sol[v] == 1)
            pt_tab[u] += (sol[x] ? -1 : 1);
    }
}

void neightborhood_move(bool first_improving)
{
    double max_surrogate_delta = -inf;
    int best_cand_delta = -inf;
    int best_cand = -1;

    random_shuffle(pi.begin(), pi.end());
    for (int j = 0; (!first_improving || max_surrogate_delta < 0) && j < n; j++)
    {
        int i = pi[j];
        // Non-tabu move or aspiration criteria (aspiration criteria only triggers for feasible solution).
        if (tabu_tab[i] == 0 || (sol_value + delta[i] > best_sol_value && sol_violated_triples + pt_tab[i] == 0))
        {
            if (sol_value + delta[i] > best_sol_value && sol_violated_triples + pt_tab[i] == 0)
            {
                // If we hit the aspiration criteria, don't miss the opportunity.
                max_surrogate_delta = delta[i] - infeasible_penalty * pt_tab[i] + removal_factor * sol[i];
                best_cand_delta = delta[i];
                best_cand = i;
                break;
            }

            if (delta[i] - infeasible_penalty * pt_tab[i] + removal_factor * sol[i] > max_surrogate_delta)
            {
                max_surrogate_delta = delta[i] - infeasible_penalty * pt_tab[i] + removal_factor * sol[i];
                best_cand_delta = delta[i];
                best_cand = i;
            }
        }
    }

    if (best_cand != -1)
    {
        sol_violated_triples += pt_tab[best_cand];

        insert_tabu(best_cand);
        update_deltas(best_cand);
        update_pt(best_cand);

        sol[best_cand] = (sol[best_cand] ? 0 : 1);
        sol_value += best_cand_delta;
    }
}

vector<pair<double, double>> tabu_search(chrono::nanoseconds max_duration, chrono::nanoseconds no_improvement_duration, int tabu_tenure, bool first_improving, bool strategic_oscillation, bool use_removal_factor)
{
    removal_factor = 0.0;
    infeasible_penalty = strategic_oscillation ? 100.0 : 2.0 * inf;

    sol = vector<int>(n, 0);
    sol_value = 0;
    sol_violated_triples = 0;
    pt_tab = vector<int>(n, 0);
    delta = vector<int>(n, 0);
    for (int i = 0; i < n; i++)
        delta[i] = a[i][i];

    best_sol = vector<int>(n, 0);
    best_sol_value = 0;

    tabu_tab = vector<int>(n, 0);
    tabu_list = list<pli>();

    pi = vector<int>();
    for (int i = 0; i < n; i++)
        pi.push_back(i);

    vector<pair<double, double>> results;
    auto start_time = chrono::high_resolution_clock::now();
    auto last_improvement = chrono::high_resolution_clock::now();
    for (iteration = 0; chrono::high_resolution_clock::now() - last_improvement < no_improvement_duration && chrono::high_resolution_clock::now() - start_time < max_duration; iteration++)
    {
        remove_old_tabu(tabu_tenure);
        neightborhood_move(first_improving);
        if (sol_value > best_sol_value && sol_violated_triples == 0)
        {
            best_sol = sol;
            best_sol_value = sol_value;
            last_improvement = chrono::high_resolution_clock::now();

            results.push_back(pair<double, double> (chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - start_time).count(), best_sol_value));
            // cout << " " << iteration << ", " << chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - start_time).count() << ", " << best_sol_value << ", " << infeasible_penalty << endl;
        }

        if (strategic_oscillation)
        {
            if (sol_violated_triples == 0)
            {
                infeasible_penalty *= 0.99;
            }
            else
            {
                infeasible_penalty *= 1.01;
            }
        }

        if (use_removal_factor && iteration % 1000 == 0)
        {
            removal_factor = 5 * (rand() / (double) RAND_MAX);
        }
    }

    results.push_back(pair<double, double> (chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - start_time).count(), best_sol_value));
    cout << " " << iteration << ", " << chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - start_time).count() << ", " << best_sol_value << ", " << infeasible_penalty << endl;

    return results;
}

void experiment(string out_file, chrono::nanoseconds max_duration, chrono::nanoseconds no_improvement_duration, int tabu_tenure, bool first_improving, bool strategic_oscillation, bool use_removal_factor)
{
    srand(rand());
    vector<vector<pair<double, double>>> results;
    vector<int> j;
    for (int i = 0; i < 20; i++)
    {
        results.push_back(tabu_search(max_duration, no_improvement_duration, tabu_tenure, first_improving, strategic_oscillation, use_removal_factor));
        j.push_back(0);
    }

    vector<pair<double, double>> agg;
    while (true)
    {
        int choice = -1;
        for (int i = 0; i < results.size(); i++)
            if (j[i] + 1 < results[i].size() && (choice == -1 || results[i][j[i] + 1].first < results[choice][j[choice] + 1].first))
                choice = i;
        
        if (choice == -1)
            break;
        
        j[choice]++;

        double y = 0;
        for (int i = 0; i < results.size(); i++)
            y += results[i][j[i]].second;
        
        y /= results.size();
        agg.push_back(pair<double, double> (results[choice][j[choice]].first, y));
    }

    ofstream out;
    out.open (out_file + ".csv");
    for (int i = 0; i < agg.size(); i++)
    {
        out << agg[i].first << "," << agg[i].second << endl;
    }

    out.close();

    out.open (out_file + ".sol");

    double miny = inf, maxy = -inf;
    for (int i = 0; i < results.size(); i++)
    {
        miny = min(miny, results[i].back().second);
        maxy = max(maxy, results[i].back().second);
    }

    out << miny << " " << agg.back().second << " " << maxy;

    out.close();
}

int main(void)
{
    srand(20);
    read_input("pacote_atividade4/TS_Framework/instances/qbf400");
    int t = fork();
    if (t == 0)
    {
        experiment("exp1", chrono::nanoseconds(30ll * 60ll * 1000000000ll), chrono::nanoseconds(60ll * 1000000000ll), ceil(n / 8.0), false, false, false);
        return 0;
    }

    t = fork();
    if (t == 0)
    {
        experiment("exp2", chrono::nanoseconds(30ll * 60ll * 1000000000ll), chrono::nanoseconds(60ll * 1000000000ll), ceil(n / 8.0), true, false, false);
        return 0;
    }
    
    t = fork();
    if (t == 0)
    {
        experiment("exp3", chrono::nanoseconds(30ll * 60ll * 1000000000ll), chrono::nanoseconds(60ll * 1000000000ll), ceil(n / 16.0), false, false, false);
        return 0;
    }

    t = fork();
    if (t == 0)
    {
        experiment("exp4", chrono::nanoseconds(30ll * 60ll * 1000000000ll), chrono::nanoseconds(60ll * 1000000000ll), ceil(n / 8.0), false, true, false);
        return 0;
    }

    t = fork();
    if (t == 0)
    {
        experiment("exp5", chrono::nanoseconds(30ll * 60ll * 1000000000ll), chrono::nanoseconds(60ll * 1000000000ll), ceil(n / 8.0), false, false, true);
        return 0;
    }

    while(1);
}
