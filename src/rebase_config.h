#pragma once

struct RebaseConfig {
    // Nombre de blocs entre chaque rebase
    const int rebaseInterval = 1000;
    // Cible d'activité : nombre moyen de transactions par bloc
    const double targetTx = 50.0;
    // Facteur de contraction si l'activité est insuffisante
    const double contractionFactor = 0.1;
    // Facteur d'expansion si l'activité dépasse la cible
    const double expansionFactor = 0.1;
    // Seuil critique pour activer le circuit breaker
    const double criticalTxThreshold = 20.0;
};

extern const RebaseConfig g_rebaseConfig;
