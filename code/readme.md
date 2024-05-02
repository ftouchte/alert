# gemc_source

## ahdc_HitProcess.cc
 
``` cpp
map<string, double> ahdc_HitProcess::integrateDgt(MHit* aHit, int hitn);
```

Un hit est caractérisé par un signal d'énergie déposée sur une certaine plage horaire. La version discrétisée de ce signal est contenue dans les attributs de tout objet de type `MHit` (ou `Hit` ou `GAVHit`). Voici quelques attributs :

``` cpp
private:
	// all these infos are recorded
	// in each step of the hit
	vector<G4ThreeVector>   pos;    ///< Hit Position (Global)
	vector<G4ThreeVector>  Lpos;    ///< Hit Positions (Local to the Volume)
	vector<G4ThreeVector>  vert;    ///< Primary Vertex of track in each step
	vector<double>         edep;    ///< Energy Deposited
	vector<double>           dx;    ///< Length of the step
	vector<double>         time;    ///< Time from the start of event
	vector<G4ThreeVector>   mom;    ///< Momentum of the Track
	vector<double>            E;    ///< Energy of the track
```

L'argument `hitn` contient le numéro n du n-ième hit associé à un évènement donné. Pour l'observer, on peut intelligemment ajouter le code ci-dessous dans la définition de la méthode :

``` cpp
std::ofstream output("output.txt",std::ofstream::app);
// ...
if (!output.fail()){
        if (hitn == 1) {
            output << "# NEW EVENT" << std::endl; 
        }
        output << "hitn : " << hitn << std::endl;
        output << "time : " << stepTime << std::endl;
}
```
Il suffit alors d'utiliser la fonction `./showFile.cpp` pour voir les correspondanes.

**Important :** La numérotation de `hitn` commence à 1. Le `time` ainsi défini croit avec `hitn` sauf pour le dernier dont la valuer semble aléatoire.




