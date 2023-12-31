/**************************************************************************************************
Originally From: Solver.C -- (C) Niklas Een, Niklas Sorensson, 2004
Substantially modified by: Mate Soos (2010)
**************************************************************************************************/

#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

#include "Solver.h"
#include "Queue.h"
#include "CSet.h"
#include "BitArray.h"
#include <map>
#include <vector>
#include <queue>
using std::vector;
using std::map;
using std::priority_queue;

class ClauseCleaner;

class Subsumer
{
public:
    
    Subsumer(Solver& S2);
    ~Subsumer();
    const bool simplifyBySubsumption();
    void unlinkModifiedClause(vec<Lit>& origClause, ClauseSimp c);
    void unlinkModifiedClauseNoDetachNoNULL(vec<Lit>& origClause, ClauseSimp c);
    void unlinkClause(ClauseSimp cc, Var elim = var_Undef);
    ClauseSimp linkInClause(Clause& cl);
    void linkInAlreadyClause(ClauseSimp& c);
    void updateClause(ClauseSimp c);
    void newVar();
    void extendModel(Solver& solver2);
    const bool unEliminate(const Var var);
    const vec<char>& getVarElimed() const;
    const uint32_t getNumElimed() const;
    
private:
    
    friend class ClauseCleaner;
    
    //Main
    vec<ClauseSimp>        clauses;
    CSet                   learntClauses;
    vec<char>              touched;        // Is set to true when a variable is part of a removed clause. Also true initially (upon variable creation).
    vec<Var>               touched_list;   // A list of the true elements in 'touched'.
    CSet                   cl_touched;     // Clauses strengthened.
    CSet                   cl_added;       // Clauses created.
    vec<vec<ClauseSimp> >  occur;          // 'occur[index(lit)]' is a list of constraints containing 'lit'.
    vec<vec<ClauseSimp>* > iter_vecs;      // Vectors currently used for iterations. Removed clauses will be looked up and replaced by 'Clause_NULL'.
    vec<CSet* >            iter_sets;      // Sets currently used for iterations.
    Solver&                solver;         // The Solver
    
    
    vec<char>              var_elimed;     // 'eliminated[var]' is TRUE if variable has been eliminated.
    vec<char>              cannot_eliminate;//
    map<Var, vector<vector<Lit> > > elimedOutVar;
    
    // Temporaries (to reduce allocation overhead):
    //
    vec<char>           seen_tmp;       // (used in various places)
    vector<Lit>         io_tmp;         // (used for reading/writing clauses from/to disk)
    
    
    //Limits
    uint32_t numVarsElimed;
    uint32_t numMaxSubsume1;
    uint32_t numMaxSubsume0;
    uint32_t numMaxElim;
    int64_t numMaxBlockToVisit;
    uint32_t numMaxBlockVars;
    
    //Start-up
    void addFromSolver(vec<Clause*>& cs);
    void addBackToSolver();
    void removeWrong(vec<Clause*>& cs);
    void fillCannotEliminate();
    const bool treatLearnts();
    void addAllXorAsNorm();
    void addXorAsNormal3(XorClause& c);
    void addXorAsNormal4(XorClause& c);
    
    //Iterations
    void registerIteration  (CSet& iter_set) { iter_sets.push(&iter_set); }
    void unregisterIteration(CSet& iter_set) { remove(iter_sets, &iter_set); }
    void registerIteration  (vec<ClauseSimp>& iter_vec) { iter_vecs.push(&iter_vec); }
    void unregisterIteration(vec<ClauseSimp>& iter_vec) { remove(iter_vecs, &iter_vec); }
    
    // Subsumption:
    void touch(const Var x);
    void touch(const Lit p);
    bool updateOccur(Clause& c);
    void findSubsumed(Clause& ps, vec<ClauseSimp>& out_subsumed);
    void findSubsumed(const vec<Lit>& ps, const uint32_t abst, vec<ClauseSimp>& out_subsumed);
    void findSubsumed(Clause& ps, uint32_t abs, vec<ClauseSimp>& out_subsumed);
    bool isSubsumed(Clause& ps);
    uint32_t subsume0(Clause& ps);
    uint32_t subsume0(Clause& ps, uint32_t abs);
    void subsume0LearntSet(vec<Clause*>& cs);
    void subsume1(ClauseSimp& ps);
    void smaller_database();
    void almost_all_database();
    template<class T1, class T2>
    bool subset(const T1& A, const T2& B);
    bool subsetAbst(uint32_t A, uint32_t B);
    
    void orderVarsForElim(vec<Var>& order);
    int  substitute(Lit x, Clause& def, vec<Clause*>& poss, vec<Clause*>& negs, vec<Clause*>& new_clauses);
    bool maybeEliminate(Var x);
    void MigrateToPsNs(vec<ClauseSimp>& poss, vec<ClauseSimp>& negs, vec<ClauseSimp>& ps, vec<ClauseSimp>& ns, const Var x);
    void DeallocPsNs(vec<ClauseSimp>& ps, vec<ClauseSimp>& ns);
    bool merge(Clause& ps, Clause& qs, Lit without_p, Lit without_q, vec<Lit>& out_clause);
    
    
    //hyperBinRes
    void addFromSolverAll(vec<Clause*>& cs);
    const bool hyperBinRes();
    const bool hyperUtility(vec<ClauseSimp>& iter, const Lit lit, BitArray& inside, vec<ClauseSimp>& addToClauses, uint32_t& hyperBinAdded, uint32_t& hyperBinUnitary);
    
    //merging
    //vector<char> merge();
    //const bool checkIfSame(const Lit var1, const Lit var2);
    
    class VarOcc {
        public:
            VarOcc(const Var& v, const uint32_t num) :
                var(v)
                , occurnum(num)
            {}
            Var var;
            uint32_t occurnum;
    };
    
    struct MyComp {
        const bool operator() (const VarOcc& l1, const VarOcc& l2) const {
            return l1.occurnum > l2.occurnum;
        }
    };
    
    void blockedClauseRemoval();
    const bool allTautology(const vec<Lit>& ps, const Lit lit);
    uint32_t numblockedClauseRemoved;
    const bool tryOneSetting(const Lit lit, const Lit negLit);
    priority_queue<VarOcc, vector<VarOcc>, MyComp> touchedBlockedVars;
    vec<bool> touchedBlockedVarsBool;
    void touchBlockedVar(const Var x);
    double blockTime;
    
    
    //validity checking
    void verifyIntegrity();
    
    uint32_t clauses_subsumed;
    uint32_t literals_removed;
    uint32_t origNClauses;
    uint32_t numCalls;
    bool fullSubsume;
    uint32_t clauseID;
    uint32_t numElimed;
};

template <class T, class T2>
void maybeRemove(vec<T>& ws, const T2& elem)
{
    if (ws.size() > 0)
        removeW(ws, elem);
}

inline void Subsumer::touch(const Var x)
{
    if (!touched[x]) {
        touched[x] = 1;
        touched_list.push(x);
    }
}

inline void Subsumer::touchBlockedVar(const Var x)
{
    if (!touchedBlockedVarsBool[x]) {
        touchedBlockedVars.push(VarOcc(x, occur[Lit(x, false).toInt()].size()*occur[Lit(x, true).toInt()].size()));
        touchedBlockedVarsBool[x] = 1;
    }
}

inline void Subsumer::touch(const Lit p)
{
    touch(p.var());
}

inline bool Subsumer::updateOccur(Clause& c)
{
    return !c.learnt();
}

inline bool Subsumer::subsetAbst(uint32_t A, uint32_t B)
{
    return !(A & ~B);
}

// Assumes 'seen' is cleared (will leave it cleared)
template<class T1, class T2>
bool Subsumer::subset(const T1& A, const T2& B)
{
    for (uint32_t i = 0; i != B.size(); i++)
        seen_tmp[B[i].toInt()] = 1;
    for (uint32_t i = 0; i != A.size(); i++) {
        if (!seen_tmp[A[i].toInt()]) {
            for (uint32_t i = 0; i != B.size(); i++)
                seen_tmp[B[i].toInt()] = 0;
            return false;
        }
    }
    for (uint32_t i = 0; i != B.size(); i++)
        seen_tmp[B[i].toInt()] = 0;
    return true;
}

inline void Subsumer::newVar()
{
    occur       .push();
    occur       .push();
    seen_tmp    .push(0);       // (one for each polarity)
    seen_tmp    .push(0);
    touched     .push(1);
    var_elimed  .push(0);
    touchedBlockedVarsBool.push(0);
    cannot_eliminate.push(0);
}

inline const vec<char>& Subsumer::getVarElimed() const
{
    return var_elimed;
}

inline const uint32_t Subsumer::getNumElimed() const
{
    return numElimed;
}

#endif //SIMPLIFIER_H
