1) Ne toucher que les classes rouges
2) XF dans JoystickApplication, le reste est syncrone




On doit ajouter une méthode dans NetworkEntity qui est appelée par AccelApp (en passant this en paramètre)
Suite à cet appel, on fait un push_back pour enregistrer l'application dans la liste.



## Rendu
Rendre un zip avec le projet eclipse et quelques diagrammes UML (ou timing). Ne pas envoyer le code compilé