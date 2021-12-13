1) Ne toucher que les classes rouges
2) XF dans JoystickApplication, le reste est syncrone




On doit ajouter une méthode dans NetworkEntity qui est appelée par AccelApp (en passant this en paramètre)
Suite à cet appel, on fait un push_back pour enregistrer l'application dans la liste.



## Rendu
Rendre un zip avec le projet eclipse et quelques diagrammes UML (ou timing). Ne pas envoyer le code compilé




## Réception du beacon
On crast d'abord les données en frame. Comme on ne peut pas déclarer de variables dans un switch
on cast également en beacon (et plus tard en MultiPDU) pour pouvoir utiliser ces types de données.

On test le type de frame
- Si c'est un beacon, on fait clignoter la led et on appelle le ``onBeaconReceived`` du time slot manager



## MPDU
Écrire le sensor ID avant de faire setType() (vu qu'ils sont dans le même byte)

## Test
1) Lancer le MeshSimulator
   1) Ouvrir le fichier DesetNET.msim
2) Lancer le Testbench
3) Démarrer les capteurs (Qt ou demo)



## Joystick
Ne pas utiliser le PublishRequest (?) pour publier l'événement (ne JAMAIS bloquer l'ISR).

l'ISR va juste queue un événement pour la machine d'état

C'est pour cette raison qu'on va avoir une machine d'état dans Joystick