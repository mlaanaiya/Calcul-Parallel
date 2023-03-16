# Exercice 4 : problème aux N-corps

Ce fichier fait partie du rendu évalué pour le BE de Calcul parallèle.

## Question 1

Déterminer quels calculs peuvent être parallélisés et quelles communications mettre en
place dans le code séquentiel suivant. Proposer une réécriture parallèle avec
transmission de messages de ce code.

```
variables : force[1,...,N], data[1,...,N]
for t in 1, nb_steps do
  for i in 1, N do
    force[i] = 0
    for j in 1, N do
      force[i] = force[i] + interaction(data[i], data[j])
    end for
  end for
  for i in 1, N do
    data[i] = update(data[i], force[i])
  end for
end for
```

### Réponse Q1

Dans le code séquentiel donné, il y a deux boucles imbriquées : la première calcule les forces entre chaque paire de particules, et la seconde met à jour la position de chaque particule en utilisant les forces calculées. Les calculs dans la première boucle peuvent être parallélisés en utilisant la décomposition spatiale : chaque thread peut calculer les forces entre les particules dans une zone spécifique de l'espace. Pour cela, il faudrait diviser l'espace en plusieurs zones, allouer chaque zone à un thread et faire en sorte que chaque thread ne calcule les forces que pour les particules qui se trouvent dans sa zone. Les threads devront partager les forces avec leurs voisins à la fin de chaque itération pour assurer la cohérence des résultats.

```
variables : force[1,...,N], data[1,...,N], local_force[1,...,N], local_data[1,...,N]
for t in 1, nb_steps do
  for i in 1, N do
    local_force[i] = 0
  end for
  for i in 1, N do
    local_data[i] = data[i]
  end for
  // Distribuer les particules dans les zones
  distribute_particles(local_data)
  // Calculer les forces locales
  compute_local_forces(local_data, local_force)
  // Envoyer et recevoir les forces avec les threads voisins
  communicate_forces(local_force)
  // Mettre à jour les positions
  for i in 1, N do
    data[i] = update(data[i], force[i])
  end for
end for

```

## Question 2

Proposer une version parallèle du code suivant.

```
variables : force[1,...,N], data[1,...,N]
for t in 1, nb_steps do
  for i in 1, N do
    force[i] = 0
  end for
  for i in 1, N do
    for j in 1, i-1 do
      f = interaction(data[i],data[j])
      force[i] = force[i] + f
      force[j] = force[j] - f
    end for
  end for
  for i in 1, N do
    data[i] = update(data[i], force[i])
  end for
end for
```

### Réponse Q2

Dans cette version du code, la boucle interne est modifiée pour que les forces entre les particules soient calculées deux par deux plutôt que pour toutes les paires. Cela permet de réduire le nombre de calculs nécessaires de moitié, ce qui peut améliorer les performances en réduisant les dépendances de données. Cependant, cela ajoute une dépendance de données indirecte car la mise à jour de la force de la particule j dépend de la force de la particule i. En conséquence, cette version ne peut pas être directement parallélisée en utilisant la décomposition spatiale.

```
variables : force[1,...,N], data[1,...,N]
for t in 1, nb_steps do
  for i in 1, N do
    force[i] = 0
  end for
  for i in 1, N do
    for j in 1, i-1 do
      f = interaction(data[i],data[j])
      force[i] = force[i] + f
      force[j] = force[j] - f
    end for
  end for
  for i in 1, N do
    data[i] = update(data[i], force[i])
  end for
end for
```

## Question 3

Quels sont les inconvénients de cette version ?
Proposer une solution pour les atténuer.

### Réponse Q3

Le principal inconvénient de cette version est qu'elle ajoute une dépendance de données indirecte, ce qui peut ralentir l'exécution du code. En outre, le nombre de communications nécessaires est plus élevé que dans la version précédente, ce qui peut également entraîner des coûts supplémentaires. Pour atténuer ces inconvénients, on peut utiliser des techniques telles que l'ordonnancement dynamique pour réduire les dépendances de données et l'optimisation des communications.
