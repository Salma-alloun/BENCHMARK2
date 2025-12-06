# ArbreBenchmark

Ce projet compare les performances de différentes structures arborescentes (AVL, Red-Black, set, map, unordered_set) sur de grands jeux de données en C++.

## Contenu

- `main.cpp` : code principal pour le benchmark
- `benchmark_results.txt` : résultats des tests

## Résultats principaux

| Structure | Insertion (s) | Recherche (s) | Suppression (s) |
|-----------|---------------|---------------|----------------|
| unordered_set | 2.039 | 0.811 | 1.072 |
| set STL | 5.807 | 6.329 | 12.373 |
| map STL | 12.384 | 10.058 | 12.577 |
| AVL Tree | 14.753 | 7.546 | 14.847 |
| Red-Black Tree | 9.531 | 9.299 | 9.260 |

## Conclusion

Le Red-Black Tree offre un bon compromis entre vitesse et ordre, tandis que unordered_set est le plus rapide pour l'accès brut sans ordre.
