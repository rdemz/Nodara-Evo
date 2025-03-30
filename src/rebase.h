#pragma once

#include <cstdint>

// Appelée pour traiter le rebase lorsque le bloc courant atteint un multiple de l'intervalle
void ProcessRebase(uint64_t currentBlockHeight);

// Active le circuit breaker (implémentation à personnaliser)
void ActivateCircuitBreaker();

// Pour tests, renvoie le nombre d'intervalles consécutifs d'activité faible
int GetConsecutiveLowActivity();
