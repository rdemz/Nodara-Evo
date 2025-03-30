#include "rebase.h"
#include "rebase_config.h"
#include "logging.h"
#include "chain.h"        // Pour accéder à chainActive (la chaîne de blocs)
#include "validation.h"   // Pour la déclaration de chainActive
#include <vector>

// --- Variables globales pour cet exemple ---
// Dans un déploiement complet, ces variables seront gérées par le module de consensus ou via une base de données.
uint64_t totalSupply = 2100000000000000ULL;  // Exemple : 21 millions de coins exprimés en satoshis
std::vector<double> walletBalances = { 1000000000, 500000000, 200000000 }; // Soldes simulés de quelques portefeuilles

// Variable interne pour suivre le nombre d'intervalles consécutifs d'activité faible
static int consecutiveLowActivity = 0;

// Fonction pour récupérer le nombre de transactions d'un bloc réel via chainActive.
// chainActive est un index (généralement de type CChain) qui contient les blocs validés.
uint32_t GetTransactionCountForBlock(uint64_t blockHeight) {
    if (blockHeight > static_cast<uint64_t>(chainActive.Height()))
         return 0;
    const CBlockIndex* pindex = chainActive[blockHeight];
    if (pindex == nullptr) return 0;
    // pindex->nTx correspond au nombre de transactions dans le bloc.
    return pindex->nTx;
}

void ProcessRebase(uint64_t currentBlockHeight) {
    if (currentBlockHeight % g_rebaseConfig.rebaseInterval != 0)
        return;

    double totalTransactions = 0.0;
    // Parcourir les derniers "rebaseInterval" blocs
    for (uint64_t i = currentBlockHeight - g_rebaseConfig.rebaseInterval; i < currentBlockHeight; ++i) {
        totalTransactions += GetTransactionCountForBlock(i);
    }
    double avgTx = totalTransactions / g_rebaseConfig.rebaseInterval;
    double ratio = avgTx / g_rebaseConfig.targetTx;
    double rebaseFactor = 1.0;

    if (ratio < 1.0) {
        // Faible activité : contraction de l'offre
        rebaseFactor = 1.0 - (1.0 - ratio) * g_rebaseConfig.contractionFactor;
    } else {
        // Activité supérieure à la cible : expansion de l'offre
        rebaseFactor = 1.0 + (ratio - 1.0) * g_rebaseConfig.expansionFactor;
    }

    // Mise à jour de l'offre totale
    totalSupply = static_cast<uint64_t>(totalSupply * rebaseFactor);
    // Mise à jour des soldes de portefeuilles
    for (auto &balance : walletBalances) {
        balance *= rebaseFactor;
    }

    LogPrintf("Rebase effectué au bloc %llu: facteur = %f, nouvelle offre = %llu\n",
              currentBlockHeight, rebaseFactor, totalSupply);

    // Vérification pour le circuit breaker
    if (avgTx < g_rebaseConfig.criticalTxThreshold) {
        consecutiveLowActivity++;
    } else {
        consecutiveLowActivity = 0;
    }
    if (consecutiveLowActivity >= 3) {
        ActivateCircuitBreaker();
        LogPrintf("Circuit breaker activé: activité faible pendant %d intervalles consécutifs\n", consecutiveLowActivity);
    }
}

void ActivateCircuitBreaker() {
    // Implémentez ici la logique spécifique de suspension des opérations critiques.
    // Par exemple, définir un flag global ou marquer le réseau en mode maintenance.
    LogPrintf("Circuit breaker : Suspension des opérations critiques\n");
}

int GetConsecutiveLowActivity() {
    return consecutiveLowActivity;
}
