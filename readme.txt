README pour le projet de programmation et systèmes sous Linux
Auteur : Mathias ROESLER
Date : Décembre 2018

*-------------------------------------- *
*		RÉSUMÉ			*
*-------------------------------------- *

Le projet consiste à créer un serveur de messagerie mettant en lien les 
clients deux à deux au fur et à mesure qu'ils se connectent. De plus, les
signaux SIGUSR1 et SIGUSR2 sont déroutés pour qu'ils puissent respectivement
arrêter toutes les connexions ainsi qu'arrêter le serveur et afficher le
nombre de clients connectés.

Le répertoire ProjetLinux2018_ROESLER_MATHIAS contient les éléments suivants :
	
	- src : répertoire contenant les fichiers .c utilisés dans le projet ;
	- include : répertoire contenant les fichiers .h associés aux fichiers
	  .c ;
	- server.c : fichier .c qui crée l'exécutable server ;
	- client.c : fichier .c qui crée l'exécutable client ;
	- makefile : fichier servant à la compilation du projet.

Le serveur peut accepter au maximum 4 connexions. Cette limite est fixée par
la variable MAX_CONNECTION qui est définie dans le fichier create.h situé dans
le dossier include. La variable peut être changée à tout moment pour augmenter
le nombre maximal de connexions, la recompilation du projet est alors
nécessaire.

*-------------------------------------- *
*		COMPILATION		*	
*-------------------------------------- *

Le projet se compile en tapant la commande suivante dans le terminal :

	$ make all clean

La commande crée deux fichiers exécutable : client et server. 

*-------------------------------------- *
*		EXÉCUTION		*	
*-------------------------------------- *

Le projet s'exécute en plusieurs étapes. Le programme peut être utilisé sur la
machine locale ou bien en réseau. Dans les deux cas, la procédure est la même.

Étape 1 : Lancer le serveur en exécutant le programme server

Le programme server s'exécute en tapant la commande suivante dans le terminal :

	$ ./server [adresse IP] [numéro de port] &

Pour utiliser le projet sur machine locale, il faut remplacer l'argument
[adresse IP] par l'adresse 127.0.0.1
Pour utiliser le projet en réseau, il faut remplacer l'argument [adresse IP] par
l'adresse IP de la machine. Elle peut être obtenue en tapant la commande
suivante dans le terminal :

	 $ ifconfig

L'argument [numéro de port] est choisi par l'utilisateur. Le numéro choisi
doit être strictement supérieur à 1023.

Étape 2 : Connecter un client en exécutant le programme client

Le programme client s'exécute en tapant la commande suivante dans le terminal :

	$ ./client [adresse IP] [numéro de port]

L'argument [adresse IP] doit correspondre à l'argument [adresse IP] du programme
server, 127.0.0.1 dans le cas où le projet est utilisé sur machine locale,
l'adresse IP de la machine dans le cas où le projet est utilisé en réseau.
L'argument [numéro de port] doit correspondre à l'argument [numéro de port] du
programme server.

Une fois que deux clients se sont connectés, ils sont mis en relation et
peuvent s'envoyer des messages l'un après l'autre. Il est possible à tout
moment d'obtenir le nombre de clients connectés au serveur en tapant la
commande suivante dans le terminal :

	$ kill -n 12 [pid]

où l'argument [pid] est remplacé par le numéro d'identification du processus server.

*-------------------------------------- *
*		TERMINAISON		*	
*-------------------------------------- *

Si un client veut se déconnecter, il suffit de taper le mot exit. Ceci
notifiera le deuxième client que la connexion a été terminée.

Si l'on souhaite arrêter le serveur, il suffit d'envoyer le signal SIGUSR1 au
serveur. Pour cela il faut taper la commande suivante dans le terminal :

	$ kill -n 10 [pid]

où l'argument [pid] est remplacé par le numéro d'identification du processus server.
