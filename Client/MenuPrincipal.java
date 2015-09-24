public class MenuPrincipal implements Menu {
	public void lancerMenu()
	{
		boolean quitter = false;
		while(true)
		{
			// Presentation des differents choix pour l'utilisateur
			System.out.println("\n\n		 Login : " + Client.id_client);
			System.out.println("		 -----------------------------------------------");
			System.out.println("		| Que voulez vous faire ?                       |");
			System.out.println("		|                                               |");
			System.out.println("		|    1 -> Changer mon login                     |");
			System.out.println("		|    2 -> Avoir la liste des radios disponibles |");
			if(Client.tableau_diffuseurs_choisis.size() > 0)
				System.out.println("		|    3 -> Fermer une connexion avec une radio   |");
			if(!Client.verbose)
				System.out.println("		|    4 -> Activer mode verbose                  |");
			if(Client.verbose)
				System.out.println("		|    5 -> Desactiver mode verbose               |");
			System.out.println("		|    0 -> Quitter le programme                  |");
			System.out.println("		 -----------------------------------------------");
			System.out.print("Votre choix : ");
			// Lecture du choix
			int choix = 0;
			try
			{
				choix = Client.sc.nextInt();
				while(choix < 0 || choix > 5)
				{
					System.out.println("Seulement 5 choix sont disponibles");
					choix = Client.sc.nextInt();
				}
			}
			catch(Exception e)
			{
				System.out.println("Il faut rentrer un numero, voici la liste disponible");
				Client.sc.nextLine();
				continue;
			}
			if(Client.verbose)
				System.out.println("/////------------ Bon choix, lancement de la bonne méthode... ------------/////");
			// Gestion du choix de l'utilisateur
			switch(choix)
			{
				// Si on veut changer son identifiant
				case 1:
				{
					if(Client.verbose)
						System.out.println("/////------------ Changement identifiant lancé... ------------/////");
					System.out.println("\n -----------------------------------------------");
					System.out.print("| Rentrez votre nouvel identifiant (au plus 8 caracteres) : ");
					if(Client.changeLogin())
						System.out.println("| Votre identifiant a ete change -> " + Client.id_client);
					else
						System.out.println("| Vous avez garde le meme login -> " + Client.id_client);
					
					// On a changer l'identifiant, on relance le menu principal
					if(Client.verbose)
						System.out.println("/////------------ Changement identifiant finit ------------/////");
					break;
				}
				
				// Si l'utilisateur veut avoir acces aux diffuseurs disponbible depuis un certain gestionnaire
				case 2:
				{
					choixGestionnaire();
					break;
				}
				case 3:	MenuDiffuseur.suppressionRadio(); break;
				case 4: Client.verbose = true; break;
				case 5: Client.verbose = false; break;
				// Fin du programme
				case 0: 
					if(Client.verbose)
						System.out.println("/////------------ Fermeture du programme demandée ------------/////");
					System.out.println("Aurevoir."); Client.sc.close(); System.exit(0);
				default: System.out.println("Probleme menu principal."); new MenuPrincipal().lancerMenu();
			}
			
			// Fin du programme
			if(quitter)
				break;
		}
	}
	
	public void choixGestionnaire()
	{
		if(Client.verbose)
			System.out.println("/////------------ Choix du gestionnaire lance... ------------/////");
		while(true)
		{
			Client.sc.nextLine();
			
			// L'utilisateur doit rentrer les donnees pour se connecter au gestionnaire de son choix
			System.out.println("\n -----------------------------------------------");
			System.out.print("| Veuillez donner l'adresse du gestionnaire a utiliser :   ");
			Client.addr_gestionnaire = Client.sc.nextLine();
			System.out.print("| Veuillez donner le port du gestionnaire a utiliser :     ");
			try
			{
				Client.port_gestionnaire = Client.sc.nextInt();
			}
			catch(Exception e)
			{
				System.out.println("Il faut rentrer un numero de port, recommencez :");
				System.out.println();
				continue;
			}
			if(Client.verbose)
				System.out.println("/////------------ Port et adresse enregistres ------------/////");
			// On lance le menu du gestionnaire pour le choix d'un diffuseur
			new MenuGestionnaire().lancerMenu();
			break;
		}
	}
}
