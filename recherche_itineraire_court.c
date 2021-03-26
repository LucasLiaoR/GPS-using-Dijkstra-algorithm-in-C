/* =================================================================================================================== */
/* ===================================== Projet Bastille - République ================================================ */
/* ================================ Ecrit par Lucas LIAO, Minh Phuong DO ============================================= */
/* =================================================================================================================== */

/*
Ce code a pour but de permettre à l'utilisateur de choisir parmis une liste d'intersections de rues de deux quartiers de Paris (Bastille et République). 
L'application doit fournir à l'utilisateur un itinéraire relativement détaillé pour aller de la première intersection saisie à la deuxième.
L'utilisateur se verra donc fournir en premier lieu la distance totale à parcourir avec le temps estimé selon son mode de transportation 
(qui aura été choisi au lancement de l'application mais qui peut être changé en cours d'éxécution de l'application).
Chaque trajet intermédiaire entre deux intersections sera également détaillé d'une distance à parcourir.
*/

/* =============================== */
/* ======= Fichiers d'entête ===== */
/* =============================== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* ================================ */
/* =========== Contantes ========== */
/* ================================ */
#define NB_VOIES 1000
#define NOM_MAX 300

/* ================================ */
/* ========= Type global ========== */
/* ================================ */
struct voies{
	int numero;
	int longueur;
    char nom_intersection[NOM_MAX];
};

struct itineraires{
	int num_voie;
	int longueur_deplacement;
    char nom_intersection[NOM_MAX];
};

/* ================================= */
/* === Déclarations préliminaires=== */
/* ================================= */
void saisir_pts_depart();
void saisir_pts_arrivee();
void boucle_renseigner_tableau();
void afficher_tableau_algo();
void colonne_petite_valeur_tab();
void renseigner_combinaisons_ligne();
void afficher_matrice();
void barrer_lignes();
void chemin_court_dernier_tour();
void chemin_court_boucle(int col_recherche);
void extraire_itineraire();
void nouvelle_recherche();
void chargement_data_intersections();
void choisir_pieton_voiture();
void afficher_mode_transport();
void afficher_parametrage_pieton_voiture();
char *Mid(char *chaineSource,int pos,int len);
int Trouve(char *chaineSource, char *motCle);
void conv_maj(char ch[]);
int recherche_rue(char rue_r[]);
void interprete_nom();
void partie_commun(char intersection1[],char intersection2[]);

/* =================================== */
/* ======== Variables globales ======= */
/* =================================== */
int matrice[NB_VOIES][NB_VOIES];
struct voies voie[NB_VOIES];
struct voies pts_depart;
struct voies pts_arrivee;
struct voies tab_algo[NB_VOIES][NB_VOIES];
struct itineraires itineraire[NB_VOIES];
int tour=0;
int colonne_petite_valeur;
int col_precedente;
int nb_deplacement;
int nombre_intersections=0;
int choix_pieton_voiture=0;
int bool_annulation = 0;
char token_commun[NOM_MAX];

/* ==================================== */
/* ========= Programme principal ====== */
/* ==================================== */
int main()
{
	int choix = -1;

    printf("\nBonjour ! Où allons-nous aujourd'hui ?");
    choisir_pieton_voiture();
    chargement_data_intersections();

	while (choix != 0)
	{
        printf("\n");
        printf("--------------------------------------------------------------\n");
		printf("===========================MENU===============================\n");
        printf("--------------------------------------------------------------\n");
        printf("-1- Nouvelle recherche\n")                                        ;
		printf("-2- Afficher votre mode de transportation\n")                     ;
        printf("-3- Changer le paramétrage Piéton/Voiture\n")                     ;
		printf("-0- Quitter\n")                                                   ;
		printf("==============================================================\n");
		printf("Choix : ")                                                        ;
		scanf("%d", &choix)                                                       ;

		switch (choix)
		{	
            case 1: nouvelle_recherche();
				break;
            case 2: afficher_mode_transport();
				break;
            case 3: afficher_parametrage_pieton_voiture();
                break;
			case 0: printf("Au revoir!\n");
				break;
            default: printf("Erreur ! Vous avez entré une valeur non valide ! Réessayez s'il vous plait.\n");
                    while ( getchar() != '\n' );
                break;

		}

	}


}

/* ================================================================================= */
/* ========= Afficher tableau_algo avec les déplacements possibles et choisis ====== */
/* ================================================================================= */
//Module ni visible ni accessible par l'utilisateur, il permet au développeur de vérifier le tableau qui a été renseigné par l'algorithme avec les trajets possibles.
void afficher_tableau_algo()
{
    // Cette boucle affiche les entêtes de colonnes qui correspondent à un numéro de sommet du graphe orienté
    // Le décompte du nombre d'intersection se fait par lecture du fichier de data, le calcul commence à 0 et ne prend pas en compte la dernière ligne, c'est pourquoi nous y ajoutons systématiquement 1 unité
	for (int k=0; k<nombre_intersections+1; k++)
	{
		printf("%10d  |", k);
	}	

	printf("\n");

    //Boucle qui affiche pour toutes les combinaisons de numéro d'intersection et de distance totale à parcourir en partant de la dernière plus petite combinaison dans le tour précédent
	for (int i=0; i<nombre_intersections+1; i++)
	{
		for (int j=0; j<nombre_intersections+1;j++)
		{
			printf("%3d-%7d", tab_algo[i][j].numero, tab_algo[i][j].longueur);
		}
		
		printf("\n");
	}
	
}
/* ================================================================================= */
/* ============ Afficher matrice des distances entre les rues disponbibles ========= */
/* ================================================================================= */
// Ce module non plus n'est pas accessible ni visible par l'utilisateur. Il permet au développeur d'afficher la matrice chargée par l'application. La matrice contient la distance parmi les intersections
void afficher_matrice()
{
	for (int i=0; i<nombre_intersections+1; i++)
	{
		for (int j=0; j<nombre_intersections+1; j++)
		{
			printf("%3d ", matrice[i][j]);
		}
	printf("\n");	
	}
}

/* ================================================================================= */
/* ============================== Saisir point de départ =========================== */
/* ================================================================================= */
void saisir_pts_depart()
{
    printf("\n");
    char rue_rech[NOM_MAX];
    int pts_choisi = -1;
    while (pts_choisi == -1 && bool_annulation != 1)
	{
        printf("Saisir le nom de la rue dont vous partez avec des '_' à la place des espaces et sans accents ou une partie du nom de la rue que vous cherchez (ex: Rue_de_Turbigo ou turbigo) : ");
	    scanf("%s",rue_rech);

        // rechercher le nom de le rue que l'utisateur a entré
        // recupérer le numéro entré par l'utilisateur à partir la liste affichée (c'est numéro-1 correspondant dans notre tableau)
        pts_choisi = recherche_rue(rue_rech);
        // le numéro de la rue choisie est stocké dans l'attribut numero de la variable structurée "pts_depart" ainsi que la valeur 0 dans l'attribut longueur
        pts_depart.numero = pts_choisi - 1;
	    pts_depart.longueur = 0;

        // boucle pour assigner la valeur -1 au numéro et 999999 à la longuer des autres lignes de la colonne de départ
        for (int j = 1; j<nombre_intersections+1; j++)
        {
            tab_algo[j][pts_depart.numero].numero = -1;
		    tab_algo[j][pts_depart.numero].longueur = 999999;
        }
    }
}

/* ================================================================================= */
/* ============================== Saisir point d'arrivée =========================== */
/* ================================================================================= */
void saisir_pts_arrivee()
{

    char rue_rech2[NOM_MAX];
    int pts_choisi2 = -1;
    while (pts_choisi2 == -1 && bool_annulation != 1)
	{
        printf("Saisir le nom de la rue vous souhaitez arriver avec des '_' à la place des espaces et sans accents ou une partie de la rue que vous cherchez (ex: Rue_de_Turbigo ou turbigo) : ");
        scanf("%s",rue_rech2);

        // rechercher le nom de le rue que l'utisateur a entré
        // recupérer le numéro entré par l'utilisateur
        pts_choisi2 = recherche_rue(rue_rech2);
        pts_arrivee.numero = pts_choisi2 - 1; 
	    pts_arrivee.longueur = 0;
    }

}

/* ================================================================================================================ */
/* == Scanner les valeurs des chemins qu'on a pas encore emprunté et récupération de la longueur la plus courte === */
/* ================================================================================================================ */
void colonne_petite_valeur_tab()
{
	int petit = 0;
    int tour_petit;

	if (tour != 0)
	{
		for (int i = 0; i<tour; i++)
		{
			for (int j=0; j<nombre_intersections+1; j++)
			{
                //On vérifie la valeur à la dernière ligne de la colonne pour savoir si on l'a déjà barrée (donc ayant comme valeur -1 si on y est déjà passé et qu'on y revient plus)
				if (tab_algo[nombre_intersections][j].numero != -1)
				{
                    //Si la longueur du déplacement pour arriver à cette colonne est la plus petite du tableau (en excluant les points dans lesquels on est déjà passé) 
					if (tab_algo[i][j].longueur < petit || petit==0)
					{
                        //On retient la plus petite valeur de longueur afin de pouvoir la comparer aux autres
						petit = tab_algo[i][j].longueur;
                        //On retient également la ligne à laquelle on a trouvé cette longueur afin de pouvoir la recopier au prochain tour
                        tour_petit = i;
                        //on garde cette colonne en mémoire dans la variable "colonne_petite_valeur"
						colonne_petite_valeur = j;
					}// fin if
				}
		
			}// fin boucle for
			
		}// fin boucle for

        tab_algo[tour][colonne_petite_valeur] = tab_algo[tour_petit][colonne_petite_valeur];
	}
	else
	{
        //Au premier tour, la colonne ayant la plus petite valeur est d'office le point de départ de l'utilisateur 
		colonne_petite_valeur = pts_depart.numero;
	}

    //appel de la fonction barrer_lignes afin que l'on note qu'on ne reviendra plus dans cette colonne
	barrer_lignes();
}

/* ================================================================================= */
/* ============ Barrer le reste des lignes après être passé par un point =========== */
/* ================================================================================= */
void barrer_lignes()
{
		
	for (int j=tour + 1; j<nombre_intersections+1; j++)
	{
        //On assigne la valeur -1 au numéro et 999999 dans la longueur dans les lignes libres de la colonne dans laquelle on passe (colonne_petite_valeur)
		tab_algo[j][colonne_petite_valeur].numero = -1;
		tab_algo[j][colonne_petite_valeur].longueur = 999999;

	}

}

/* ========================================================================================================================================================== */
/* ======================= Renseigner les combinaisons numero d'origine et longueur de trajet intermédiaire pour la ligne du tour actuel==================== */
/* ======================================================================================================================================================== */
void renseigner_combinaisons_ligne()
{
	for (int i=0; i<nombre_intersections+1; i++)
    {
        // Si on est à l'avant dernier tour (tour = nombre d'intersections - 1) ou que le compteur i est différent de la colonne ayant la plus petite combinaison dans le tour précédent et que on est jamais passé dans cette colonne
        // on renseigne le numéro de la colonne ayant la plus petite valeur dans le tour précédent en numéro et la longueur qui y était assignée en y ajoutant la distance nécéssaire pour se rendre en partant de cette colonne vers la colonne i,
        // on retrouve cette distance dans la matrice contenant les données de distances entre les intersections
        if ((tour == nombre_intersections) || (i != colonne_petite_valeur && tab_algo[nombre_intersections][i].numero != -1))
        {
            tab_algo[tour][i].numero = colonne_petite_valeur;
            tab_algo[tour][i].longueur = tab_algo[tour][colonne_petite_valeur].longueur + matrice[colonne_petite_valeur][i];
        }

        //Dans le cas ou il n'y a pas de trajet possible entre la colonne ayant la plus petite combinaison au tour précédent et la colonne i on renseigne dans la combinaison de la colonne i : 0 pour le numéro et 999999 pour la longueur 
        // afin de ne jamais récupérer cette colonne comme plus petite combinaison 
        if (i != colonne_petite_valeur && matrice[colonne_petite_valeur][i] == 0 && tab_algo[nombre_intersections][i].numero != -1)
        {
            tab_algo[tour][i].numero = 0;
            tab_algo[tour][i].longueur = 999999;
        }
    }  
    tour++;
}

/* ================================================================================= */
/* ========= Boucle pour remplir le tableau des trajets possibles et optimaux ====== */
/* ================================================================================= */
void boucle_renseigner_tableau()
{
	struct voies pts_intermediaire;

    // boucle allant de 0 au nombre d'intersections-1 car nous ne voulons aller que jusqu'à l'avant dernier tour, on reitère les fonctions colonne_petite_valeur_tab()
    // et renseigner_combinaisons_lignes afin de remplir chacunes des lignes du tableau avec les combinaisons possibles
    for (int i = 0; i< nombre_intersections; i++)
    {
        colonne_petite_valeur_tab();

		renseigner_combinaisons_ligne();
    }

    // Au dernier tour, nous n'avons pas besoin de renseigner de combinaisons sur la ligne car il n'y a plus aucun trajet possible. Relancer la fonction renseigner_combinaisons_ligne() nous amenerait à accéder à une ligne qui n'existe pas
    // la dernière ligne sera renseignée par la fonction colonne_petite_valeur_tab()
    colonne_petite_valeur_tab();

}// fin procédure

/* ====================================================================================================== */
/* ========= Récuperer le dernier déplacement à faire avant l'arrivée à la destination ================== */
/* ====================================================================================================== */
void chemin_court_dernier_tour()
{
	int petit_deplacement = 0;
	int ligne_depl = 999999;

    nb_deplacement = 0;

	for (int i = 0; i<nombre_intersections+1; i++)
	{
        // recherche de la combinaison avec la plus petite longueur parmis les combinaisons contenues dans la colonne correspondant au point d'arrivée renseigné par l'utilisateur
		if (petit_deplacement == 0 || tab_algo[i][pts_arrivee.numero].longueur <= petit_deplacement)
		{
            // stockage de la longueur de la combinaison si elle est inférieure à celle qui était stockée auparavant
			petit_deplacement = tab_algo[i][pts_arrivee.numero].longueur;
            // stockage de la colonne d'origine de la combinaison pour savoir dans quelle colonne aller chercher la prochaine combinaison du chemin le plus court(l'attribut numéro de la structure)
			col_precedente = tab_algo[i][pts_arrivee.numero].numero;

		}

	}

    // stockage des valeurs de la combinaisons dans la case 0 de la variable structurée itineraire dont chaque case correspond au trajet intermédiaire du chemin le plus court
	itineraire[0].num_voie = pts_arrivee.numero;

	itineraire[0].longueur_deplacement = petit_deplacement;

	nb_deplacement++;

}

/* ====================================================================================================== */
/* ========= Récupérer les déplacements intermédiaires avant l'arrivée à la destination ================= */
/* ====================================================================================================== */
void chemin_court_boucle(int col_recherche)
{
	int petit_deplacement = 0;
	int ligne_depl = 999999;

	for (int i = 0; i<nombre_intersections+1; i++)
	{
        // recherche de la combinaison avec la plus petite valeur de longueur dans la colonne renseignée en argument
		if (petit_deplacement == 0 || tab_algo[i][col_recherche].longueur <= petit_deplacement)
		{
            // stockage de la longueur de la combinaison si elle est inférieure à celle qui était stockée auparavant
			petit_deplacement = tab_algo[i][col_recherche].longueur;
            // stockage de la colonne d'origine de la combinaison pour savoir dans quelle colonne aller chercher la prochaine combinaison du chemin le plus court(l'attribut numéro de la structure)
			col_precedente = tab_algo[i][col_recherche].numero;
		}
	}

    // stockage des valeurs de la combinaisons dans la case 0 de la variable structurée itineraire dont chaque case correspond au trajet intermédiaire du chemin le plus court
	itineraire[nb_deplacement].num_voie = col_recherche;

	itineraire[nb_deplacement].longueur_deplacement = petit_deplacement;

	nb_deplacement++;
}

/* ===================================================================================== */
/* =================== Module extraction de l'itinéraire le plus court ================= */
/* ===================================================================================== */
void extraire_itineraire()
{
    if (bool_annulation != 1)
    {
        boucle_renseigner_tableau();
        chemin_court_dernier_tour();
        char libelle_point_depart[NOM_MAX];
        char libelle_itineraire1[NOM_MAX];
        char libelle_itineraire2[NOM_MAX];

        char article[10];
        char article2[10];

        while(col_precedente != pts_depart.numero)
        {
            chemin_court_boucle(col_precedente);
        }

        // interpreter le numéro du point départ en nom de l'intersection
        interprete_nom(pts_depart.numero+1,libelle_point_depart);

        // afficher le temps estimé selon le choix du transportation
        if (choix_pieton_voiture == 1)
        {
            printf("\nTrajet total : %d mètre(s). Temps de trajet estimé à pied : %d minute(s).\n", itineraire[0].longueur_deplacement, (itineraire[0].longueur_deplacement*60)/5000);
        }
        else
        {   
            printf("\nTrajet total : %d mètre(s). Temps de trajet estimé en voiture : %d minute(s).\n", itineraire[0].longueur_deplacement, (itineraire[0].longueur_deplacement*60)/15000);
        }

        printf("En partant du point de départ: %s.\n", libelle_point_depart);

        // boucle pour récuperer les deplacements intermediaires
        for (int i = nb_deplacement-1; i>=0; i--)
        {

            //réinitialisation de l'article pour la ligne suivante
            strcpy(article, "");

            interprete_nom(itineraire[i-1].num_voie+1,libelle_itineraire1);
            interprete_nom(itineraire[i].num_voie+1,libelle_itineraire2);

            if (itineraire[i].num_voie != pts_arrivee.numero)
            {
                //verifier si les deux intersections sont sur la meme rue ou pas
                partie_commun(libelle_itineraire1,libelle_itineraire2);

                // condition de trouver la partie commue du nom de la rue
                // si oui, on affiche "suivre au long de la rue..." 
                if (strcmp(token_commun,"not_found") != 0)
                {

                    //Choix de l'article à afficher selon la première lettre du type d'avenue, "de la" pour "Rue" et "Place", "du" pour les "Boulevard", "de l'" pour les "Avenue"
                    if (libelle_itineraire2[0] == 'R')
                    {
                        strcpy(article2, "de la ");
                    }
                    else if (libelle_itineraire2[0] == 'B')
                    {
                        strcpy(article2, "du ");
                    }
                    else if (libelle_itineraire2[0] == 'P')
                    {
                        strcpy(article2, "de la ");
                    }
                    else if (libelle_itineraire2[0] == 'A')
                    {
                        strcpy(article2, "de l'");
                    }

                    //Choix de l'article à afficher selon la première lettre du type d'avenue, "de la" pour "Rue" et "Place", "du" pour les "Boulevard", "de l'" pour les "Avenue"
                    if (token_commun[0] == 'R')
                    {
                        strcpy(article, "de la ");
                    }
                    else if (token_commun[0] == 'B')
                    {
                        strcpy(article, "du ");
                    }
                    else if (token_commun[0] == 'P')
                    {
                        strcpy(article, "de la ");
                    }
                    else if (token_commun[0] == 'A')
                    {
                        strcpy(article, "de l'");
                    }

                    // ajoute le nom de la rue commune dans l'affichage
                    printf("Faire %d mètre(s) au long %s%s jusqu'au croisement %s%s.\n",abs(itineraire[i+1].longueur_deplacement - itineraire[i].longueur_deplacement), article, token_commun, article2, libelle_itineraire2);

                }    
                else // si on ne trouve pas la partie commune, affiche "aller jusqu'au croisement suivant"
                {

                    //Choix de l'article à afficher selon la première lettre du type d'avenue, "de la" pour "Rue" et "Place", "du" pour les "Boulevard", "de l'" pour les "Avenue"
                    if (libelle_itineraire2[0] == 'R')
                    {
                        strcpy(article2, "de la ");
                    }
                    else if (libelle_itineraire2[0] == 'B')
                    {
                        strcpy(article2, "du ");
                    }
                    else if (libelle_itineraire2[0] == 'P')
                    {
                        strcpy(article2, "la ");
                    }
                    else if (libelle_itineraire2[0] == 'A')
                    {
                        strcpy(article2, "de l'");
                    }

                    printf("Faire %d mètre(s) jusqu'au croisement %s%s.\n", abs(itineraire[i+1].longueur_deplacement - itineraire[i].longueur_deplacement), article2,libelle_itineraire2);
                        
                }
            } 
            else // affiche le dernier deplacement avant arriver à votre destination
            {
                printf("Enfin, faire %d mètre(s) pour arriver à votre destination, %s.\n", abs(itineraire[i+1].longueur_deplacement - itineraire[i].longueur_deplacement), libelle_itineraire2);
            }		

        } //fin boucle
    } // fin if

}

/* ===================================================================================== */
/* ====================== Module pour faire une nouvelle recherche ===================== */
/* ===================================================================================== */
void nouvelle_recherche()
{

    tour = 0;

    //boucle pour remettre le tableau avec les valeurs neutres pour pouvoir refaire une nouvelle recherche d'itinéraire
    for (int i = 0; i<nombre_intersections+1; i++)
    {
        for (int j = 0; j<nombre_intersections+1; j++)
        {
            tab_algo[i][j].numero = 0;
            tab_algo[i][j].longueur = 0;
        }
    }

    //Resaisie de nouveaux points de départ et d'arrivée pour une nouvelle recherche
    saisir_pts_depart();
    printf("\n");
    saisir_pts_arrivee();
    extraire_itineraire();

    bool_annulation = 0;

}

/* ================================================================================================== */
/* ================== Module de choix pour paramétrer le GPS en mode voiture ou piéton ============== */
/* ================================================================================================== */
void choisir_pieton_voiture()
{

    while (choix_pieton_voiture != 1 && choix_pieton_voiture != 2)
    {
        printf("\n");
        printf("Etes vous à pied ou en voiture ?\n");
        printf("-1- Pour piéton\n");
        printf("-2- Pour voiture\n");
        printf("Choix : ");

        scanf("%d", &choix_pieton_voiture);

        if (choix_pieton_voiture == 1)
        {
            printf("Vous avez configuré le GPS en mode piéton.\n");
        }
        else if (choix_pieton_voiture == 2)
        {
            printf("Vous avez configuré le GPS en mode voiture.\n");
        }
        else
        {
            printf("Vous avez rentré une option invalide ! Veuillez recommencer.\n");
            while ( getchar() != '\n' );
        }            
    }       
}

/* ================================================================================================== */
/* =============================== Chargement du fichier de data  =================================== */
/* ================= (fichier différent selon valeur de la variable choix_pieton_voiture)  ========== */
/* ================================================================================================== */
void chargement_data_intersections()
{
    FILE *f1;

    nombre_intersections = 0;

    // chargement des fichier matrice différents selon le choix de l'utilisateur
    if (choix_pieton_voiture == 1)
    {
        f1 = fopen("matrice_pieton.txt", "r");
    }
    else if (choix_pieton_voiture == 2)
    {
        f1 = fopen("matrice_voiture.txt", "r");
    }
    
    int voie;
    int x=0;
    int y=0;
    int retour;
    char chr;
    
    if (f1 == NULL)
    {
        printf("Erreur ! Fichier non lu\n");
    }
    else
    { 
        //Calcul du nombre de ligne dans le fichier, ce qui donne le nombre d'intersections à gérer par le GPS
        chr = getc(f1);
        while (chr != EOF)
        {
        //Incrémentation de la variable nombre_intersections à chaque fois que le lecteur arrive sur un retour à la ligne
        if (chr == '\n')
        {
            nombre_intersections++;
        }
        //Lecture du char suivant
        chr = getc(f1);
        }

        //Ramener le pointeur au tout début du fichier
        rewind(f1);

        while (!feof(f1))
        {
            retour = fscanf(f1, "%d,", &voie);

            if (retour != EOF)
            {
                matrice[x][y] = voie;
                y++;
            }

            if (y>=nombre_intersections+1)
            {
                x++;
                y=0;
            }

        }

        fclose(f1);

    }

}

/* ============================================================================== */
/* ================== Module d'afficher le choix du transportation ============== */
/* ============================================================================== */
void afficher_mode_transport()
{
    switch(choix_pieton_voiture)
    {
        case 1 : printf("\nVous avez configuré le GPS en mode piéton.\n");
            break;
        case 2 : printf("\nVous avez configuré le GPS en mode voiture.\n");
            break;
    }
}
/* ============================================================================== */
/* ================== Module de changer le choix du transportation ============== */
/* ============================================================================== */
void afficher_parametrage_pieton_voiture()
{
    printf("\n");
	char rep[2];
    int error = 1;
    while (error == 1)
    {
        if (choix_pieton_voiture == 1)
        {
            printf("Mode activé : Piéton.\n");
            printf("Voulez-vous changer votre mode en voiture? (O/N) :");
        }
        else if (choix_pieton_voiture == 2)
        {
            printf("Mode activé : Voiture.\n");
            printf("Voulez-vous changer votre mode à piéton? (O/N) :");
        }
	scanf("%s",rep);
    conv_maj(rep);
    
        if ((strcmp(&rep[0],"O") == 0 || (strcmp(&rep[0],"N") == 0 )))
        {
            if (strcmp(&rep[0],"O")==0 && choix_pieton_voiture == 1)
	        {
		        choix_pieton_voiture = 2;
                chargement_data_intersections();
		        printf("Mode de transportation changé. Mode activé : Voiture\n");
	        }
	        else if (strcmp(&rep[0],"O")==0 && choix_pieton_voiture == 2)
	        {
		        choix_pieton_voiture = 1;
                chargement_data_intersections();
		        printf("Mode de transportation changé. Mode activé : Piéton\n");
	        }
            else if (strcmp(&rep[0],"N")==0)
            {
                printf("Mode de transportation inchangé.\n");
            }
            error = 0;
        }
        else
        {
            printf("La réponse incorrecte. Merci de répondre O/N !\n");
            error = 1;
        }
    }

}

/* =============================================================================================== */
/* ================== Module de trouver des intersections à partir un nom de la rue ============== */
/* =============================================================================================== */

/* ------------------ Mid -------------------- */
//renvoie un extrait de la chaîne, commençant au caractère pos et faisant len caractères de long
char * Mid(char *chaineSource,int pos,int len) {
  // Retourne la sous-chaine de la chaine chaineSource
  // depuis la position pos sur la longueur len
 
  char * dest=NULL;                        
  if (len>0) {                  
    /* allocation et mise à zéro */          
    dest = (char *)calloc(len+1, 1);      
    /* vérification de la réussite de l'allocation*/  
    if(NULL != dest) {
        strncat(dest,chaineSource+pos,len);            
    }
  }                                      
  return dest;                            
}
/* ------------------ Trouve -------------------- */
// renvoie un nombre correspondant à la position de mot clé dans chaîne source
// Si le mot clé n’est pas comprise dans chaîne source, la fonction renvoie -1
int Trouve(char *chaineSource, char*motCle)
{
    int i, found, rang;

    found = 0;
    i = 0;
    rang = 0;

    while(!found && i < strlen(chaineSource))
    {
        if(strcmp(Mid(chaineSource, i, strlen(motCle)), motCle) == 0)
        {
            rang = i;
            found = 1;
        }
        else
        { rang = -1;}
        i++;
    }

    return rang;
}
/* --------------------------------------------------- */
/* ----- Convertir nom de la rue en majuscule -------- */
/* --------------------------------------------------- */
void conv_maj(char ch[])
{
    int i;
    int taille;
    taille = strlen(ch);
    
    for (i=0; i <taille; i++)
    {
        // exclure les caracteres spéciaux dans le nom de la rue
        if(strncmp(&ch[i],"_",1) !=0 && strncmp(&ch[i],"/",1) !=0 )
            {ch[i]=toupper(ch[i]);}
    }
}
/* ------------------------------------------ */
/* -------- Rechercher nom de la rue -------- */
/* ------------------------------------------ */
int recherche_rue(char rue_r[])
{
    FILE *f1;

    f1 = fopen("Liste_intersections.txt", "r");

    int retour;
    int i = 0;
    char intersection[NOM_MAX];
    int pos;
    int compteur=0;
    int pts = -1;
    char nom_rue[NOM_MAX];

    // chargement la liste de nom de l'intersections et la mettre dans le tableau
    if (f1 == NULL)
    {
        printf("Erreur ! Fichier non lu\n");
        exit(0);
    }
    else
    {
        while (!feof(f1))
        {
            retour = fscanf(f1, "%s", intersection);

            if (retour != EOF)
            {
                strcpy(voie[i].nom_intersection, intersection);
                i++;
            }

        }

        fclose(f1);
        conv_maj(rue_r);

        // la boucle pour vérifier si la chaine de caractere recherché existe dans la liste
        // pos renvoie -1 à chaque fois il ne trouve pas le nom de la rue cherché dans un tour de boucle k
        // compteur renvoie le nombre d'intersections s'il ne le trouve pas dans toute la liste
        for (int k=0;k<i;k++)
        {
            conv_maj(voie[k].nom_intersection);
            pos = Trouve(voie[k].nom_intersection,rue_r);
            if (pos == -1)
            {
                compteur++;
            }
        } // fin boucle

        if (compteur==i)
        {
            printf("La rue non trouvée ! Veuillez ressayer! \n");
        }
        else
        {
            while (pts == -1 && bool_annulation != 1)
            {
                printf("---------------------------------------------------------------\n");
                printf("Les intersections sur cette rue sont : \n");

                // boucle pour afficher tous les intersections qui contiennent le nom de la rue cherché si trouvé (pos != -1)
                for (int j = 0; j<i; j++)
                {
                    conv_maj(voie[j].nom_intersection);
                    pos = Trouve(voie[j].nom_intersection,rue_r);
                    if (pos != -1)
                    {
                        printf("%3d - %s\n",j+1, voie[j].nom_intersection);
                    }
                } // fin boucle

                printf("---------------------------------------------------------------\n");
                printf("Veuillez choisir un point parmi les intersections ci-dessus (Saisir -1 pour annuler la recherche) : ");
                scanf("%d",&pts);
                
                // condition pour vérifier si le numéro choisi existe (vu qu'on n'a que 332 intersections)
                if (pts<1 || pts>332)
                {
                    if (pts == -1)
                    {
                        bool_annulation = 1;
                        printf("Annulation de la recherche.\n");
                    }
                    else
                    {
                        pts = -1;
                        printf("Vous avez entré une valeur non valide. Veuillez ressayer !\n");
                        while ( getchar() != '\n' );
                    }
                }
                else
                {
                    interprete_nom(pts,nom_rue);
                    printf("Vous avez choisi le point numéro %d - %s.\n",pts,nom_rue);
                }
                
                
            }
        }
    }
    return pts; // retourne le numéro de l'intersection selon la liste affichée aux utilisateurs
}
/* ============================================================================== */
/* ================== Interpreter le nom de l'intersection ====================== */
/* ============================================================================== */
void interprete_nom(int num, char texte[])
{
    FILE *f1;

    f1 = fopen("Liste_intersections.txt", "r");

    int i = 0;
    char intersection[NOM_MAX];
    int retour;

    // charger les données à partir du fichier et les mettre dans le tableau
    if (f1 == NULL)
    {
        printf("Erreur ! Fichier non lu\n");
        exit(0);
    }
    else
    {
        while (!feof(f1))
        {
            retour = fscanf(f1, "%s", intersection);

            if (retour != EOF)
            {
                strcpy(voie[i].nom_intersection, intersection);
                i++;
            }

        }

        fclose(f1);

        // la liste d'intersections qui affiche aux utilisateurs commence par 1 alors que notre tableau commence par 0 => num-1
        // si la boucle renvoie le nom d'intersection correspondant dans notre tableau
        for (int j = 0; j<i; j++)
        {
            if ((num-1) == j)
            {
                conv_maj(voie[j].nom_intersection);
                strcpy(texte,voie[j].nom_intersection);
            }
        }
    }

}
/* ============================================================================== */
/* ======= Vérifier si deux intersections sont sur la meme rue ================== */
/* ============================================================================== */
// le module est pour le but de trouver un nom de la rue commun entre les deux intersections
void partie_commun(char intersection1[],char intersection2[])
{
    char A[NOM_MAX];
    char B[NOM_MAX];
    const char sep[2] = "/"; // caractere de séparateur
    int i=0;
    char *tokens[4];
    char *temp;

    strcpy(A,intersection1);
    strcpy(B,intersection2);
    conv_maj(A);
    conv_maj(B);

    // couper les parties séparées par "/" dans la premiere chaine de caractere et les stocker dans token
    temp = strtok(A, sep);
    while (temp != NULL)
    {
        tokens[i] = temp;
        temp = strtok(NULL, sep);
        i++;
    }

    //trouver la partie commune et retourner la chaine commune par comparer chaque partie coupée avec la deuxieme chaine de caractere
    if (Trouve(B,tokens[0]) != -1)
    {
        //printf("La partie commun est %s :",tokens[0]);
        strcpy(token_commun,tokens[0]);
    }
    else if (Trouve(B,tokens[1]) != -1)
    {
        //printf("La partie commun est %s :",tokens[1]);
        strcpy(token_commun,tokens[1]);
        
    }
    else if (Trouve(B,tokens[2]) != -1)
    {
        //printf("La partie commun est %s :",tokens[2]);
        strcpy(token_commun,tokens[2]);
    }
    else
    {
        strcpy(token_commun,"not_found");
    }
    
    
}